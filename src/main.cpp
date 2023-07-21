#include <cstdint>
#include <memory>
#include <tuple>
#include <string>
#include <vector>
#include <set>

#include <iostream>
#include <fstream>
#include <sstream>

extern "C" {
	#include <unistd.h>
}


#include "util/ANSI.h"
#include "CLI.hpp"
#include "Symbol.hpp"
#include "Parser.hpp"
#include "Graph.hpp"
#include "Generator.hpp"


using namespace std;
using namespace CSR;


// ----------------------------------- [ Functions ] ---------------------------------------- //


void errLoc(const char* file, const Location& loc){
	fprintf(stderr, "%s:%d:%d", file, loc.row+1, loc.col+1);
}


template<typename ...T>
void err(const char* file, const char* format, T... args){
	fprintf(stderr, ANSI_BOLD "%s: " ANSI_RED "error" ANSI_RESET ": ", file);
	fprintf(stderr, format, args...);
}


template<typename ...T>
void err(const char* file, const Location& loc, const char* format, T... args){
	if (loc.valid()){
		fprintf(stderr, ANSI_BOLD "%s:%d:%d: " ANSI_RED "error" ANSI_RESET ": ", file, loc.row+1, loc.col+1);
		fprintf(stderr, format, args...);
	} else {
		err(file, format, args...);
	}
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


bool parseCLI(int argc, char const* const* argv){
	try {
		CLI::parse(argc, (char* const*)argv);
		return true;
	} catch (const exception& e) {
		fprintf(stderr, ANSI_RED "%s" ANSI_RESET ": %s\n", argv[0], e.what());
		return false;
	}
}


Document* parseInput(const char* inputPath = nullptr){
	// Get input stream
	unique_ptr<ifstream> inf;
	istream* in;
	const char* docName = CLI::programName.c_str();
	
	// Open input stream (file is auto-closed)
	if (!isatty(fileno(stdin))){
		in = &cin;
	} else {
		inf = make_unique<ifstream>(inputPath);
		
		if (inf->fail()){
			err(CLI::programName.c_str(), "Failed to open input file '%s'.\n", inputPath);
			return nullptr;
		}
		
		in = inf.get();
		docName = inputPath;
	}
	
	// Parse
	unique_ptr<Parser> parser = make_unique<Parser>();
	Document* doc = nullptr;
	
	try {
		parser->tabSize = CLI::tabSize;
		doc = parser->parse(*in);
	} catch (const ParserException& e) {
		err(docName, e.loc, "%s\n", e.what());
		return nullptr;
	} catch (const exception& e) {
		err(CLI::programName.c_str(), "%s\n", e.what());
		return nullptr;
	}
	
	if (doc != nullptr){
		doc->name = docName;
	}
	
	return doc;
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


bool validateReduction(const Document& doc){
	int i;
	if (CLI::verifyReduction && !Reduction::validateSize(doc.reductions, &i)){
		err(doc.name.c_str(), doc.reductions[i].loc, "Reduction produces more symbols than it consumes.\n");
		return false;
	}
	return true;
}


// --------------------------------- [ Main Functions ] ------------------------------------- //


int main(int argc, char const* const* argv){
	printf("================================\n");	// DEBUG
	
	
	if (!parseCLI(argc, argv))
		return 1;
	
	Document* doc = parseInput(CLI::inputFilePath.c_str());
	if (doc == nullptr)
		return 1;
	
	if (!validateReduction(*doc)){
		return 1;
	}
	
	
	
	
	Graph g;
	try {
		g.create(doc->reductions);
	} catch (const DuplicateReduction& e) {
		err(doc->name.c_str(), doc->reductions[e.i].loc, "Duplicate left side of reduction. Previously declared at " ANSI_BOLD);
		errLoc(doc->name.c_str(), doc->reductions[e.prev_i].loc);
		fprintf(stderr, ANSI_RESET ".\n");
		return 0;
	} catch (const GraphException& e) {
		if (e.i >= 0)
			err(doc->name.c_str(), doc->reductions[e.i].loc, "%s\n", e.what());
		else
			err(doc->name.c_str(), "%s\n", e.what());
		return 0;
	}
	
	
	
	
	delete doc;
	return 0;
}


// ------------------------------------------------------------------------------------------ //