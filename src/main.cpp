#include <unistd.h>

#include <cstdint>
#include <string>
#include <fstream>
#include <iostream>

#include "util/ANSI.h"
#include "CLI.hpp"
#include "Parser.hpp"


using namespace std;
using namespace csg;


// ----------------------------------- [ Functions ] ---------------------------------------- //





// --------------------------------- [ Main Functions ] ------------------------------------- //


int main(int argc, char const* const* argv){
	printf("================================\n");	// DEBUG
	
	// Parse CLI options
	try {
		CLI::parse(argc, (char* const*)argv);
		CLI::inputFilePath = "test/test.csg";
	} catch (const exception& e) {
		fprintf(stderr, ANSI_RED "%s" ANSI_RESET ": %s\n", argv[0], e.what());
	}
	
	
	// Get input stream
	ifstream* inf = nullptr;
	istream* in;
	
	if (!isatty(fileno(stdin))){
		in = &cin;
	} else {
		in = inf = new ifstream(CLI::inputFilePath);
	}
	
	
	// Parse
	{
		Parser* parser = new Parser();
		parser->tabSize = CLI::tabSize;
		
		try {
			parser->parse(*in);
		}
		catch (const ParserException& e) {
			fprintf(stderr, ANSI_BOLD "%s:%d:%d: " ANSI_RED "error" ANSI_RESET ": %s\n", CLI::inputFilePath, e.loc.row+1, e.loc.col+1, e.what());
		}
		catch (const exception& e) {
			fprintf(stderr, ANSI_BOLD "%s" ANSI_RESET ": " ANSI_RED "error" ANSI_RESET ": %s\n", CLI::programName, e.what());
		}
		
		delete parser;
	}
	
	
	// Close input file
	if (inf != nullptr){
		inf->close();
		delete inf;
	}
	
	return 0;
}


// ------------------------------------------------------------------------------------------ //