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
#include "NamedStream.hpp"
#include "CLI.hpp"
#include "Parser.hpp"
#include "SymbolEnum.hpp"
#include "AliasResolver.hpp"
#include "Graph.hpp"
#include "GraphSerializer.hpp"
#include "Generator.hpp"
#include "data.hpp"

#include "ptr.hpp"
#include <functional>


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


void help(){
	const char* name = CLI::programName.c_str();
	printf(data::help_txt, name);
	printf("\n");
}


bool parseCLI(int argc, char const* const* argv){
	try {
		CLI::parse(argc, (char* const*)argv);
		return true;
	} catch (const exception& e) {
		err(argv[0], "%s\n", e.what());
		return false;
	}
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


Document* parseInput(const optional<string>& inputPath){
	NamedStream<istream> in = {};
	in.path = CLI::programName;
	
	// Pipe
	if (!inputPath.has_value()){
		if (!isatty(fileno(stdin))){
			in.open("0");
		} else {
			err(CLI::programName, "No input file or pipe specified.\n");
			return nullptr;
		}
	}
	
	// Path or std
	else {
		try {
			in.open(*inputPath);
			if (in.path.empty())
				in.path = CLI::programName;
		} catch (const exception& e){
			err(CLI::programName, "Failed to open input file. %s\n", e.what());
			return nullptr;
		}
	}
	
	
	// Parse
	unique_ptr<Parser> parser = make_unique<Parser>();
	Document* doc = nullptr;
	
	try {
		parser->tabSize = CLI::tabSize;
		doc = parser->parse(in);
	} catch (const ParserException& e) {
		err(in.path, e.loc, "%s\n", e.what());
		return nullptr;
	} catch (const exception& e) {
		err(CLI::programName, "%s\n", e.what());
		return nullptr;
	}
	
	if (doc != nullptr){
		doc->name = in.path;
	}
	
	return doc;
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


bool handleGraphSerializationOption(const Graph& graph, const SymbolEnum& symEnum){
	if (!CLI::graph_outputFilePath.has_value()){
		return true;
	}
	
	NamedStream<ostream> out;
	
	try {
		out.open(*CLI::graph_outputFilePath);
	} catch (const exception& e){
		err(CLI::programName, "Failed to open graph output file. %s\n", e.what());
		return false;
	}
	
	
	GraphSerializer gs = {};
	gs.unicode = CLI::unicode;
	gs.ansi = CLI::ansi;
	
	
	// Get graph format
	if (CLI::graph_outputFormat.has_value()){
		if (CLI::graph_outputFormat->starts_with('.'))
			gs.format = GraphSerializer::getFormat(*CLI::graph_outputFormat);
		else
			gs.format = GraphSerializer::getFormat("." + *CLI::graph_outputFormat);
	} else {
		if (out.isFile())
			gs.format = GraphSerializer::getFormat(*CLI::graph_outputFilePath);
		else
			gs.format = GraphSerializer::Format::TXT;
	}
	
	// Verify format
	if (gs.format == GraphSerializer::Format::UNKNOWN){
		if (CLI::graph_outputFormat.has_value())
			err(CLI::programName, "Unknown graph format: \"%s\".", CLI::graph_outputFormat->c_str());
		else
			err(CLI::programName, "Unknown graph format.\n");
		return false;
	}
	
	
	gs.serialize(out, graph, symEnum);
	return true;
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


bool handleCodeGeneration(const Document& doc, const Graph& graph, const SymbolEnum& symEnum){
	unique_ptr<Generator> generator = make_unique<Generator>();
	
	// Open default
	if (!CLI::outputFilePath.has_value() &
		!CLI::outputHeaderFilePath.has_value() &
		!CLI::outputHeaderFilePath_token.has_value() &
		!CLI::graph_outputFilePath.has_value()
	){
		try {
			generator->out.open("1");
		} catch (const exception& e){
			err(CLI::programName, "Failed to open stdout for output. %s\n", e.what());
			return false;
		}
	}
	
	
	// Open output streams
	try {
		if (CLI::outputFilePath.has_value())
			generator->out.open(*CLI::outputFilePath);
	} catch (const exception& e){
		err(CLI::programName, "Failed to open output file. %s\n", e.what());
		return false;
	}
	
	try {
		if (CLI::outputHeaderFilePath.has_value())
			generator->out_header.open(*CLI::outputHeaderFilePath);
	} catch (const exception& e){
		err(CLI::programName, "Failed to open output header file. %s\n", e.what());
		return false;
	}
	
	try {
		if (CLI::outputHeaderFilePath_token.has_value())
			generator->out_tokenHeader.open(*CLI::outputHeaderFilePath_token);
	} catch (const exception& e){
		err(CLI::programName, "Failed to open output token header file. %s\n", e.what());
		return false;
	}
	
	
	// Run generator
	try {
		generator->generate(graph, symEnum);
	} catch (const CSRException& e){
		err(doc.name, "%s\n", e.what());
		return false;
	} catch (const exception& e){
		err(CLI::programName, "%s\n", e.what());
		return false;
	}
	
	
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
	
	
	// Parse command line options
	if (!parseCLI(argc, argv)){
		return 1;
	} else if (CLI::help){
		help();
		return 0;
	}
	
	
	// Parse csr document
	unique_ptr<Document> doc = ptr(parseInput(CLI::inputFilePath));
	if (doc == nullptr){
		return 1;
	}
	if (!validateReduction(*doc)){
		return 1;
	}
	
	
	// Enumerate symbols and create reductions
	vector<shared_ptr<Reduction>> reductions;
	shared_ptr<SymbolEnum> symEnum = make_shared<SymbolEnum>();
	
	try {
		convertReductions(doc->reductions, reductions, *symEnum);
	} catch (const ParserException& e){
		err(doc->name, e.loc, "%s\n", e.what());
		return 1;
	} catch (const exception& e){
		err(doc->name, "%s\n", e.what());
		return 1;
	}
	
	
	// Build graph
	unique_ptr<Graph> graph = make_unique<Graph>();
	graph->build(reductions);
	
	
	// Serialize graph
	if (!handleGraphSerializationOption(*graph, *symEnum)){
		return 1;
	}
	
	
	// Generate source code
	if (!handleCodeGeneration(*doc, *graph, *symEnum)){
		return 1;
	}
	
	
	return 0;
}


// ------------------------------------------------------------------------------------------ //