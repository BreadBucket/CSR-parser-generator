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
	printf(data::help_txt, name);	// help.txt contains format var for program name
	printf("\n");
}


bool parseCLI(int argc, char const* const* argv){
	try {
		CLI::parse(argc, (char* const*)argv);
		return true;
	} catch (const exception& e) {
		err(string(argv[0]), "%s\n", e.what());
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
			err("Failed to deduce graph format from output file \"%s\".\n", CLI::graph_outputFilePath->c_str());
		return false;
	}
	
	
	gs.serialize(out, doc);
	return true;
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


bool handleCodeGeneration(Document& doc){
	unique_ptr<Generator> generator = make_unique<Generator>();
	generator->inline_header = true;
	generator->inline_tokenHeader = true;
	
	
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
	
	
	// Open C stream
	try {
		if (CLI::outputFilePath.has_value() && !CLI::outputFilePath->empty())
			generator->out.open(*CLI::outputFilePath);
	} catch (const exception& e){
		err(CLI::programName, "Failed to open output file. %s\n", e.what());
		return false;
	}
	
	// Open header stream
	try {
		if (CLI::outputHeaderFilePath.has_value()){
			
			if (!CLI::outputHeaderFilePath->empty()){
				generator->out_header.open(*CLI::outputHeaderFilePath);
				
				if (generator->out_header.good() && generator->out_header.isFile()){
					generator->headerPath = generator->out_header.getFileName();
				}
				
				generator->inline_header = !generator->out_header.isFile();
			} else {
				generator->inline_header = false;
			}
			
		}
	} catch (const exception& e){
		err(CLI::programName, "Failed to open output header file. %s\n", e.what());
		return false;
	}
	
	// Open token header stream
	try {
		if (CLI::outputHeaderFilePath_token.has_value()){
			
			if (!CLI::outputHeaderFilePath_token->empty()){
				generator->out_tokenHeader.open(*CLI::outputHeaderFilePath_token);
				
				if (generator->out_tokenHeader.good() && generator->out_tokenHeader.isFile()){
					generator->tokenHeaderPath = generator->out_tokenHeader.getFileName();
				}
				
				generator->inline_tokenHeader = !generator->out_tokenHeader.isFile();
			} else {
				generator->inline_tokenHeader = false;
			}
			
		}
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
	if (doc.parsedReductions.size() <= 0){
		err(doc.name, "No reductions detected.\n");
		return false;
	}
	
	int i;
	if (!CLI::allowProductions && !ParsedReduction::validateLength(doc.parsedReductions, &i)){
		err(doc.name, doc.parsedReductions[i]->loc, "Reduction produces more symbols than it consumes. Allow productions using option '--productions'.\n");
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
	// printf("================================\n");	// DEBUG
	unique_ptr<Document> doc = make_unique<Document>();
	
	
	// Parse command line options
	if (argc <= 1){
		help();
		return 1;
	} if (!parseCLI(argc, argv)){
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
	
	
	// Enumerate symbols, create reductions and resolve aliases
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