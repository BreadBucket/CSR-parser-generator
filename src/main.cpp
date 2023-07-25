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
#include "GraphSerializer.hpp"
#include "Generator.hpp"

#include "ptr.hpp"


using namespace std;
using namespace csr;


// ----------------------------------- [ Functions ] ---------------------------------------- //


void errLoc(const string& file, const Location& loc){
	fprintf(stderr, "%s:%d:%d", file.c_str(), loc.row+1, loc.col+1);
}


template<typename ...T>
void err(const string& file, const char* format, T... args){
	fprintf(stderr, ANSI_BOLD "%s: " ANSI_RED "error" ANSI_RESET ": ", file.c_str());
	fprintf(stderr, format, args...);
}


template<typename ...T>
void err(const string& file, const Location& loc, const char* format, T... args){
	if (loc.valid()){
		fprintf(stderr, ANSI_BOLD "%s:%d:%d: " ANSI_RED "error" ANSI_RESET ": ", file.c_str(), loc.row+1, loc.col+1);
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
		err(argv[0], e.what());
		return false;
	}
}


Document* parseInput(const string& inputPath){
	// Get input stream
	unique_ptr<ifstream> inf;
	istream* in;
	const string* docName = &CLI::programName;
	
	
	// Open stdin
	if (inputPath.size() == 0){
		if (!isatty(fileno(stdin))){
			in = &cin;
		} else {
			err(CLI::programName, "No input file or pipe specified.\n");
			return nullptr;
		}
	}
	
	// Open file (file is auto-closed)
	else {
		inf = make_unique<ifstream>(inputPath);
		
		if (inf->fail()){
			err(CLI::programName, "Failed to open input file '%s'.\n", inputPath);
			return nullptr;
		}
		
		in = inf.get();
		docName = &inputPath;
	}
	
	// Parse
	unique_ptr<Parser> parser = make_unique<Parser>();
	Document* doc = nullptr;
	
	try {
		parser->tabSize = CLI::tabSize;
		doc = parser->parse(*in);
	} catch (const ParserException& e) {
		err(*docName, e.loc, "%s\n", e.what());
		return nullptr;
	} catch (const exception& e) {
		err(CLI::programName, "%s\n", e.what());
		return nullptr;
	}
	
	if (doc != nullptr){
		doc->name = *docName;
	}
	
	return doc;
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


bool handleGraphSerializationOption(const Graph& graph){
	const string& path = CLI::graph_outputFilePath;
	const string& format = CLI::graph_outputFormat;
	
	int std = -1;
	GraphSerializer gs = {};
	gs.unicode = CLI::unicode;
	gs.ansi = CLI::ansi;
	
	ostream* stream = nullptr;
	ofstream fileStream;
	
	
	// Check for std stream
	if (path.empty()){
		return true;
	} else if (path == "0"){
		err(CLI::programName, "Invalid stream specifier: '%d'.\n", std);
		return false;
	} else if (path == "1"){
		std = 1;
		stream = &cout;
	} else if (path == "2"){
		std = 2;
		stream = &cerr;
	}
	
	
	// Get graph format
	if (!CLI::graph_outputFormat.empty()){
		if (CLI::graph_outputFormat.starts_with('.'))
			gs.format = GraphSerializer::getFormat(CLI::graph_outputFormat);
		else
			gs.format = GraphSerializer::getFormat("." + CLI::graph_outputFormat);
	} else {
		if (stream == nullptr)
			gs.format = GraphSerializer::getFormat(CLI::graph_outputFilePath);
		else
			gs.format = GraphSerializer::Format::TXT;
	}
	
	// Verify format
	if (gs.format == GraphSerializer::Format::UNKNOWN){
		if (CLI::graph_outputFormat.empty())
			err(CLI::programName, "Unknown graph format.\n");
		else
			err(CLI::programName, "Unknown graph format: \"%s\".", CLI::graph_outputFormat.c_str());
		return false;
	}
	
	
	// Open file (auto-closed)
	if (stream == nullptr){
		fileStream.open(CLI::graph_outputFilePath);
		stream = &fileStream;
		
		if (fileStream.fail()){
			err(CLI::programName, "Failed to create graph output file '%s'.\n", CLI::graph_outputFilePath.c_str());
			return false;
		}
		
	}
	
	
	gs.serialize(graph, *stream);
	return true;
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


bool validateReduction(const Document& doc){
	int i;
	if (CLI::verifyReduction && !ParsedReduction::validateSize(doc.reductions, &i)){
		err(doc.name, doc.reductions[i].loc, "Reduction produces more symbols than it consumes.\n");
		return false;
	}
	
	int a, b;
	if (!ParsedReduction::distinct(doc.reductions, &a, &b)){
		err(doc.name, doc.reductions[b].loc, "Duplicate left side of reduction. Previously declared at " ANSI_BOLD);
		errLoc(doc.name, doc.reductions[a].loc);
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
		err(doc->name, e.loc, "%s\n", e.what());
		return 1;
	} catch (const exception& e){
		err(doc->name, "%s\n", e.what());
		return 1;
	}
	
	
	// Enumerate symbols
	unique_ptr<SymbolEnum> symbolEnum = make_unique<SymbolEnum>(make_shared<Symbol>(-1, "null"));
	enumerate(doc->reductions, *symbolEnum);
	
	// Create reduction objects
	vector<shared_ptr<Symbol>> symbols = symbolEnum->getSymbols();
	vector<shared_ptr<Reduction>> reductions = createReductions<shared_ptr<Reduction>>(doc->reductions, *symbolEnum);
	
	// Build graph
	unique_ptr<Graph> graph = make_unique<Graph>();
	graph->build(reductions);
	
	// Serialize graph
	if (!handleGraphSerializationOption(*graph)){
		return 1;
	}
	
	
	
	
	
	
	// Generate source code
	unique_ptr<Generator> generator = make_unique<Generator>();
	generator->generate(*graph, symbols);
	// generator->generateTokenEnum(symbols);
	// generator->generateSwitch(*graph);
	
	
	
	
	return 0;
}


// ------------------------------------------------------------------------------------------ //