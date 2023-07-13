#include "CLI.hpp"

extern "C" {
	#include <getopt.h>
}

#include <cstdint>
#include <cstring>
#include <string>
#include <stdexcept>
#include <iostream>

#include "util/utils.hpp"


using namespace std;
using namespace CLI;


// ------------------------------------[ Variables ] ---------------------------------------- //



namespace CLI {
	const char* programName = "";
	const char* inputFilePath = nullptr;
	const char* outputFilePath = nullptr;
	int tabSize = 4;
}


// ----------------------------------- [ Constants ] ---------------------------------------- //


enum OptionID : int {
	NONE = INT32_MIN,
	IN,
	OUT,
	TAB_SIZE
} selected_opt;


const char* const short_options = "i:o:t:";


const struct option long_options[] = {
	{"input",   required_argument, (int*)&selected_opt, OptionID::IN},
	{"output",  required_argument, (int*)&selected_opt, OptionID::OUT},
	{"tabSize", required_argument, (int*)&selected_opt, OptionID::TAB_SIZE},
	{0, 0, 0, 0}
};


// ----------------------------------- [ Functions ] ---------------------------------------- //


OptionID shortOptionToLong(char c){
	switch (c){
		case 'i':
			return OptionID::IN;
		case 'o':
			return OptionID::OUT;
		case 't':
			return OptionID::TAB_SIZE;
		default:
			return OptionID::NONE;
	}
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


int getLength(void const* const* v){
	int n = 0;
	while (*v != nullptr){
		n++;
		v++;
	}
	return n;
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


void CLI::parse(int argc, char const* const* argv){
	if (argv == nullptr || *argv == nullptr){
		return;
	}
	
	// Modifyable copy of args
	const int len = getLength((void const* const*)argv);
	const char** v = new const char*[len];
	copy(&argv[0], &argv[len], v);
	
	
	opterr = 0;
	optind = 1;
	programName = v[0];
	bool inputSpecified = false;
	
	while (true){
		selected_opt = OptionID::NONE;
		char c = getopt_long(argc, (char* const*)v, short_options, long_options, NULL);
		
		// Error
		if (c == '?'){
			if (optopt >= 0 && shortOptionToLong(optopt) == OptionID::NONE)
				throw runtime_error(string("Unrecognized option '").append(v[optind-1]).append("'."));
			else
				throw runtime_error(string("Missing option argument '").append(v[optind-1]).append("'."));
		}
		
		else if (c > 0){
			selected_opt = shortOptionToLong(c);
		} else if (c < 0){
			break;
		}
		
		// Handle long options
		switch (selected_opt){
			
			case OptionID::IN:
				inputSpecified = true;
				inputFilePath = optarg;
				break;
			
			case OptionID::OUT:
				outputFilePath = optarg;
				break;
			
			case OptionID::TAB_SIZE:
				tabSize = atoi(optarg);
				if (tabSize < 0)
					throw runtime_error("Option 'tabSize' must be a positive integer.");
				break;
			
		}
		
	};
	
	
	// Non-option arguments
	while (optind < argc){
		
		if (!inputSpecified){
			inputFilePath = v[optind];
			inputSpecified = true;
		} else {
			throw runtime_error(string("Input file already specified, additional argument forbidden: '").append(v[optind]).append("'."));
		}
		
		optind++;
	}
	
	
	delete[] v;
	return;
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


void CLI::clear(){
	programName = "";
	inputFilePath = nullptr;
	outputFilePath = nullptr;
	tabSize = 4;
}


// ------------------------------------------------------------------------------------------ //