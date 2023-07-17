#include <unistd.h>

#include <cstdint>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include "util/ANSI.h"
#include "CLI.hpp"
#include "Parser.hpp"
#include "Generator.hpp"


using namespace std;
using namespace csg;


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


Parser* parseInput(const char* inputPath = nullptr){
	// Get input stream
	ifstream* inf = nullptr;
	istream* in;
	
	if (!isatty(fileno(stdin))){
		in = &cin;
	} else {
		inf = new ifstream(inputPath);
		in = inf;
	}
	
	// Parse
	Parser* parser = new Parser();
	parser->tabSize = CLI::tabSize;
	
	try {
		parser->parse(*in);
	}
	catch (const ParserException& e) {
		fprintf(stderr, ANSI_BOLD "%s:%d:%d: " ANSI_RED "error" ANSI_RESET ": %s\n", inputPath, e.loc.row+1, e.loc.col+1, e.what());
	}
	catch (const exception& e) {
		fprintf(stderr, ANSI_BOLD "%s" ANSI_RESET ": " ANSI_RED "error" ANSI_RESET ": %s\n", CLI::programName, e.what());
	}
	
	
	// Close input file
	if (inf != nullptr){
		inf->close();
		delete inf;
	}
	
	return parser;
}


// --------------------------------- [ Main Functions ] ------------------------------------- //


int main(int argc, char const* const* argv){
	printf("================================\n");	// DEBUG
	CLI::inputFilePath = "test/test.csg";	// DEBUG
	
	
	if (!parseCLI(argc, argv))
		return 1;
	
	Parser* p = parseInput();
	if (p == nullptr)
		return 1;
	
	
	
	Generator g;
	g.reductions = p->reductions;
	g.code = p->codeSegments;
	
	g.headerFile = &cout;
	g.cFile = &cout;
	
	g.generate();
	
	
	return 0;
}


// ------------------------------------------------------------------------------------------ //