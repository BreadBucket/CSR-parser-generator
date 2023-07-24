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


#include "ANSI.h"
#include "CLI.hpp"
#include "Parser.hpp"
#include "SymbolEnum.hpp"
#include "ParserAbstraction.hpp"
#include "Graph.hpp"
#include "GraphWriter.hpp"
#include "Generator.hpp"

#include "ptr.hpp"


using namespace std;
using namespace csr;


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


Document* parseInput(const string& inputPath){
	// Get input stream
	unique_ptr<ifstream> inf;
	istream* in;
	string docName = CLI::programName.c_str();
	
	
	// Open stdin
	if (inputPath.size() == 0){
		if (!isatty(fileno(stdin))){
			in = &cin;
		} else {
			err(CLI::programName.c_str(), "No input file or pipe specified.\n");
			return nullptr;
		}
	}
	
	// Open file (file is auto-closed)
	else {
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
		err(docName.c_str(), e.loc, "%s\n", e.what());
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


bool handleGraphOption(const Graph& graph){
	const string& path = CLI::graph_outputFilePath;
	const string& format = CLI::graph_outputFormat;
	
	int std = -1;
	GraphWriter gw = {};
	gw.unicode = CLI::unicode;
	gw.ansi = CLI::ansi;
	
	// Check for std stream
	if (path.empty())
		return true;
	else if (path == "0")
		std = 0;
	else if (path == "1")
		std = 1;
	else if (path == "2")
		std = 2;
	
	
	// Get graph format
	if (!CLI::graph_outputFormat.empty()){
		if (CLI::graph_outputFormat.starts_with('.'))
			gw.format = GraphWriter::getFormat(CLI::graph_outputFormat);
		else
			gw.format = GraphWriter::getFormat("." + CLI::graph_outputFormat);
	} else {
		if (std < 0)
			gw.format = GraphWriter::getFormat(CLI::graph_outputFilePath);
		else
			gw.format = GraphWriter::Format::TXT;
	}
	
	// Verify format
	if (gw.format == GraphWriter::Format::UNKNOWN){
		if (CLI::graph_outputFormat.empty())
			err(CLI::programName.c_str(), "Unknown graph format.\n");
		else
			err(CLI::programName.c_str(), "Unknown graph format: \"%s\".", CLI::graph_outputFormat.c_str());
		return false;
	}
	
	
	ostream* stream;
	ofstream file;
	
	// Std stream
	if (std >= 0){
		if (std == 1)
			stream = &cout;
		else if (std == 2)
			stream = &cerr;
		else {
			err(CLI::programName.c_str(), "Invalid stream specifier: '%d'.\n", std);
		}
	}
	
	// Open file (auto-closed)
	else {
		file.open(CLI::graph_outputFilePath);
		stream = &file;
		
		if (file.fail()){
			err(CLI::programName.c_str(), "Failed to create graph output file '%s'.\n", CLI::graph_outputFilePath.c_str());
			return false;
		}
		
	}
	
	
	gw.write(graph, *stream);
	return true;
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


bool validateReduction(const Document& doc){
	int i;
	if (CLI::verifyReduction && !ParsedReduction::validateSize(doc.reductions, &i)){
		err(doc.name.c_str(), doc.reductions[i].loc, "Reduction produces more symbols than it consumes.\n");
		return false;
	}
	
	int a, b;
	if (!ParsedReduction::distinct(doc.reductions, &a, &b)){
		err(doc.name.c_str(), doc.reductions[b].loc, "Duplicate left side of reduction. Previously declared at " ANSI_BOLD);
		errLoc(doc.name.c_str(), doc.reductions[a].loc);
		fprintf(stderr, ANSI_RESET ".\n");
		return false;
	}
	
	return true;
}


// --------------------------------- [ Main Functions ] ------------------------------------- //


int main(int argc, char const* const* argv){
	printf("================================\n");	// DEBUG
	
	
	if (!parseCLI(argc, argv))
		return 1;
	
	unique_ptr<Document> doc = ptr(parseInput(CLI::inputFilePath));
	if (doc == nullptr){
		return 1;
	}
	
	if (!validateReduction(*doc)){
		return 1;
	}
	
	
	// Resolve symbol aliases
	try {
		for (ParsedReduction& r : doc->reductions)
			r.resolveSymbolAliases();
	} catch (const ParserException& e){
		err(doc->name.c_str(), e.loc, "%s\n", e.what());
		return 1;
	} catch (const exception& e){
		err(doc->name.c_str(), "%s\n", e.what());
		return 1;
	}
	
	
	// Enumerate symbols
	unique_ptr<SymbolEnum> symbolEnum = make_unique<SymbolEnum>(make_shared<Symbol>(-1, "null"));
	enumerate(doc->reductions, *symbolEnum);
	
	vector<shared_ptr<Reduction>> reductions = createReductions<shared_ptr<Reduction>>(doc->reductions, *symbolEnum);
	
	unique_ptr<Graph> g = make_unique<Graph>();
	g->build(reductions);
	
	if (!handleGraphOption(*g)){
		return 1;
	}
	
	
	
	
	
	return 0;
}


// ------------------------------------------------------------------------------------------ //