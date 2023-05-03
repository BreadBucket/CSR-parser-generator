#include <unistd.h>

#include <cstdint>
#include <string>
#include <fstream>
#include <iostream>

#include "csg_parser.hpp"
#include "csg_rule.hpp"

using namespace std;
using namespace csg;


// ----------------------------------- [ Functions ] ---------------------------------------- //


void f(){
	
}


// --------------------------------- [ Main Functions ] ------------------------------------- //


int main(int argc, char** arg){
	string file_path = "test/test.csg";
	
	// Get input
	ifstream* inf = nullptr;
	istream* in;
	
	if (!isatty(fileno(stdin))){
		in = &cin;
	} else {
		in = inf = new ifstream(file_path);
	}
	
	
	f();
	
	// Parser* parser = new Parser();
	// parser->parse(*in);
	// delete parser;
	
	
	// Close file
	if (inf != nullptr){
		inf->close();
		delete inf;
	}
	
	return 0;
}


// ------------------------------------------------------------------------------------------ //