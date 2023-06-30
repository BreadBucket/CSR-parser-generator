#include <unistd.h>

#include <cstdint>
#include <string>
#include <fstream>
#include <iostream>

#include "util/ANSI.h"
#include "Parser.hpp"


using namespace std;
using namespace csg;


// ----------------------------------- [ Functions ] ---------------------------------------- //


void f(){
	
}


// --------------------------------- [ Main Functions ] ------------------------------------- //


int main(int argc, char** arg){
	printf("================================\n");
	string file_path = "test/test.csg";
	
	// Get input
	ifstream* inf = nullptr;
	istream* in;
	
	if (!isatty(fileno(stdin))){
		in = &cin;
	} else {
		in = inf = new ifstream(file_path);
	}
	
	
	// Parse
	{
		Parser* parser = new Parser();
		
		try {
			parser->parse(*in);
		}
		catch (const ParserException& e) {
			fprintf(stderr, ANSI_BOLD "%s:%d:%d: " ANSI_RED "error" ANSI_RESET ": %s\n", file_path.c_str(), e.loc.row+1, e.loc.col+1, e.what());
		}
		catch (const exception& e) {
			fprintf(stderr, ANSI_RED "error" ANSI_RESET ":");
		}
		
		delete parser;
	}
	
	// Close file
	if (inf != nullptr){
		inf->close();
		delete inf;
	}
	
	return 0;
}


// ------------------------------------------------------------------------------------------ //