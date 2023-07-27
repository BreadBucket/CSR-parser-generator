#include "CLI.hpp"

extern "C" {
	#include <getopt.h>
}

#include <cstdint>
#include <string>
#include <stdexcept>
#include <iostream>


using namespace std;
using namespace CLI;


// ------------------------------------[ Variables ] ---------------------------------------- //



namespace CLI {
	string programName = "program";
	string inputFilePath;
	string outputFilePath;
	
	string graph_outputFilePath;
	string graph_outputFormat;
	
	bool unicode = true;
	bool ansi = true;
	int tabSize = 4;
	bool verifyReduction = true;
}


// ----------------------------------- [ Constants ] ---------------------------------------- //


enum OptionID : int {
	NONE = INT32_MIN,
	IN,
	OUT,
	GRAPH_OUTPATH,
	GRAPH_FORMAT,
	UNICODE,
	ASCII,
	ANSI,
	TAB_SIZE,
	VERIFY_REDUCTION
} selected_opt;


const char* const short_options = "i:o:t:ra";


const struct option long_options[] = {
	{"input",       required_argument, (int*)&selected_opt, OptionID::IN               },
	{"output",      required_argument, (int*)&selected_opt, OptionID::OUT              },
	{"graph",       required_argument, (int*)&selected_opt, OptionID::GRAPH_OUTPATH    },
	{"graphFormat", required_argument, (int*)&selected_opt, OptionID::GRAPH_FORMAT     },
	{"ascii",       no_argument,       (int*)&selected_opt, OptionID::ASCII            },
	{"tabSize",     required_argument, (int*)&selected_opt, OptionID::TAB_SIZE         },
	{"skipCheck",   no_argument,       (int*)&selected_opt, OptionID::VERIFY_REDUCTION },
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
		case 'r':
			return OptionID::VERIFY_REDUCTION;
		case 'a':
			return OptionID::ASCII;
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
				if (inputFilePath.empty())
					inputFilePath = optarg;
				else
					throw runtime_error(string("Input file already specified, additional argument forbidden: '").append(v[optind-1]).append("'."));
				break;
			
			case OptionID::OUT:
				if (outputFilePath.empty())
					outputFilePath = optarg;
				else
					throw runtime_error(string("Output file already specified, additional argument forbidden: '").append(v[optind-1]).append("'."));
				break;
			
			case OptionID::GRAPH_OUTPATH:
				if (graph_outputFilePath.empty())
					graph_outputFilePath = optarg;
				else
					throw runtime_error(string("Graph output file already specified, additional argument forbidden: '").append(v[optind-1]).append("'."));
				break;
			
			case OptionID::GRAPH_FORMAT:
				if (graph_outputFormat.empty())
					graph_outputFormat = optarg;
				else
					throw runtime_error(string("Graph output format file already specified, additional argument forbidden: '").append(v[optind-1]).append("'."));
				break;
			
			case OptionID::TAB_SIZE:
				tabSize = atoi(optarg);
				if (tabSize < 0)
					throw runtime_error("Option 'tabSize' must be a positive integer.");
				break;
			
			case OptionID::ASCII:
				unicode = false;
				break;
			
			case OptionID::UNICODE:
				unicode = true;
				break;
			
			case OptionID::VERIFY_REDUCTION:
				verifyReduction = false;
				break;
			
		}
		
	};
	
	
	// Non-option arguments
	while (optind < argc){
		
		if (inputFilePath.empty()){
			inputFilePath = v[optind];
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
	programName = "program";
	inputFilePath.clear();
	outputFilePath.clear();
	graph_outputFilePath.clear();
	graph_outputFormat.clear();
	unicode = true;
	tabSize = 4;
	verifyReduction = true;
}


// ------------------------------------------------------------------------------------------ //