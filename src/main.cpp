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
#include "ParsedReduction.hpp"
#include "Parser.hpp"
#include "SymbolEnum.hpp"
#include "AliasResolver.hpp"
#include "Graph.hpp"
#include "GraphSerializer.hpp"
#include "Generator.hpp"
#include "Document.hpp"
#include "data.hpp"
#include "Error.hpp"


using namespace std;
using namespace csr;


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


bool parseInput(const optional<string>& inputPath, Document& doc){
	NamedStream<istream> in = {};
	
	
	// Default: pipe
	if (!inputPath.has_value()){
		if (!isatty(fileno(stdin))){
			in.open("0");
		} else {
			err("No input file or pipe specified.\n");
			return false;
		}
	}
	
	// Path or std
	else {
		try {
			in.open(*inputPath);
			if (in.path.empty())
				in.path = CLI::programName;
		} catch (const exception& e){
			err("Failed to open input file. %s\n", e.what());
			return false;
		}
	}
	
	
	// Set document name
	if (in.isFile())
		doc.name = in.path;
	else
		doc.name = CLI::programName;
	
	
	unique_ptr<Parser> parser = make_unique<Parser>();
	
	// Parse
	try {
		parser->tabSize = CLI::tabSize;
		parser->parse(in);
	} catch (const ParserException& e) {
		err(in.path, e.loc, "%s\n", e.what());
		return false;
	} catch (const exception& e) {
		err(CLI::programName, "%s\n", e.what());
		return false;
	}
	
	
	// Move parsed data
	doc.parsedReductions.clear();
	doc.parsedReductions.reserve(parser->reductions.size());
	for (ParsedReduction& r : parser->reductions){
		doc.parsedReductions.emplace_back(make_shared<ParsedReduction>(move(r)));
	}
	
	doc.code = move(parser->code);
	
	
	return true;
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


bool handleGraphSerializationOption(const Document& doc){
	if (!CLI::graph_outputFilePath.has_value()){
		return true;
	}
	
	NamedStream<ostream> out;
	
	try {
		out.open(*CLI::graph_outputFilePath);
	} catch (const exception& e){
		err("Failed to open graph output file. %s\n", e.what());
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
			err("Unknown graph format: \"%s\".", CLI::graph_outputFormat->c_str());
		else
			err("Unknown graph format.\n");
		return false;
	}
	
	
	gs.serialize(out, doc);
	return true;
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


bool handleCodeGeneration(Document& doc){
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
		generator->generate(doc);
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
	if (CLI::verifyReduction && !ParsedReduction::validateSize(doc.parsedReductions, &i)){
		err(doc.name, doc.parsedReductions[i]->loc, "Reduction produces more symbols than it consumes.\n");
		return false;
	}
	
	int a, b;
	if (!ParsedReduction::distinct(doc.parsedReductions, &a, &b)){
		err(doc.name, doc.parsedReductions[b]->loc, "Duplicate left side of reduction. Previously declared at " ANSI_BOLD);
		errLoc(doc.name, doc.parsedReductions[a]->loc);
		fprintf(stderr, ANSI_RESET ".\n");
		return false;
	}
	
	return true;
}


// --------------------------------- [ Main Functions ] ------------------------------------- //


int main(int argc, char const* const* argv){
	printf("================================\n");	// DEBUG
	unique_ptr<Document> doc = make_unique<Document>();
	
	
	// Parse command line options
	if (!parseCLI(argc, argv)){
		return 1;
	} else if (CLI::help){
		help();
		return 0;
	}
	
	
	// Parse csr document
	if (!parseInput(CLI::inputFilePath, *doc)){
		return 1;
	} else if (!validateReduction(*doc)){
		return 1;
	}
	
	
	// Enumerate symbols and create reductions
	try {
		doc->symEnum = make_shared<SymbolEnum>();
		convertReductions(doc->parsedReductions, doc->reductions, *doc->symEnum);
	} catch (const ParserException& e){
		err(doc->name, e.loc, "%s\n", e.what());
		return 1;
	} catch (const exception& e){
		err(doc->name, "%s\n", e.what());
		return 1;
	}
	
	
	// Build graph
	doc->graph = make_shared<Graph>();
	doc->graph->build(doc->reductions);
	
	
	// Serialize graph
	if (!handleGraphSerializationOption(*doc)){
		return 1;
	}
	
	
	// Generate source code
	if (!handleCodeGeneration(*doc)){
		return 1;
	}
	
	
	return 0;
}


// ------------------------------------------------------------------------------------------ //