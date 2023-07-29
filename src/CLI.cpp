#include "CLI.hpp"

extern "C" {
	#include <getopt.h>
}

#include <cstdint>
#include <cstring>
#include <string>
#include <stdexcept>
#include <iostream>


using namespace std;
using namespace CLI;


// ------------------------------------[ Variables ] ---------------------------------------- //



namespace CLI {
	string programName = "program";
	string inputFilePath = "";
	string outputFilePath = "0";
	
	string graph_outputFilePath = "";
	string graph_outputFormat = "";
	
	bool verifyReduction = true;
	
	bool unicode = true;
	bool ansi = true;
	int tabSize = 4;
	
	bool help = false;
}


// ----------------------------------- [ Constants ] ---------------------------------------- //


enum OptionID : int {
	NONE = INT32_MIN,
	HELP,
	IN,
	OUT,
	GRAPH_OUTPATH,
	GRAPH_FORMAT,
	UNICODE,
	ASCII,
	ANSI,
	TAB_SIZE,
	VERIFY_REDUCTION,
} selected_opt;


const char* const short_options = "i:o:g:t:pa";


const struct option long_options[] = {
	{"help",        no_argument,       (int*)&selected_opt, OptionID::HELP             },
	{"input",       required_argument, (int*)&selected_opt, OptionID::IN               },
	{"output",      required_argument, (int*)&selected_opt, OptionID::OUT              },
	{"graph",       required_argument, (int*)&selected_opt, OptionID::GRAPH_OUTPATH    },
	{"graphFormat", required_argument, (int*)&selected_opt, OptionID::GRAPH_FORMAT     },
	{"ascii",       no_argument,       (int*)&selected_opt, OptionID::ASCII            },
	{"tabSize",     required_argument, (int*)&selected_opt, OptionID::TAB_SIZE         },
	{"productions", no_argument,       (int*)&selected_opt, OptionID::VERIFY_REDUCTION },
	{0, 0, 0, 0}
};


// ----------------------------------- [ Functions ] ---------------------------------------- //


OptionID shortOptionToLong(char c){
	switch (c){
		case 'i':
			return OptionID::IN;
		case 'o':
			return OptionID::OUT;
		case 'g':
			return OptionID::GRAPH_OUTPATH;
		case 't':
			return OptionID::TAB_SIZE;
		case 'p':
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
	
	// Special case for 'help'
	if (argc == 2 && argv[1] != nullptr){
		if (strcmp(argv[1], "help") == 0){
			help = true;
			return;
		}
	}
	
	// Modifyable copy of args
	const int len = getLength((void const* const*)argv);
	const char** v = new const char*[len];
	copy(&argv[0], &argv[len], v);
	
	
	opterr = 0;
	optind = 1;
	programName = v[0];
	int prevOpt = optind;
	
	while (true){
		selected_opt = OptionID::NONE;
		const int c = getopt_long(argc, (char* const*)v, short_options, long_options, NULL);
		
		// Error
		if (c == '?'){
			if (optopt >= 0 && (shortOptionToLong(optopt) == OptionID::NONE)){
				int i = (optind > prevOpt) ? optind - 1 : optind;
				throw runtime_error(string("Unrecognized option '").append(v[i]).append("'."));
			} else {
				throw runtime_error(string("Missing option argument '").append(v[optind-1]).append("'."));
			}
		}
		
		else if (c > 0){
			selected_opt = shortOptionToLong(c);
		} else if (c < 0){
			break;
		}
		
		// Handle long options
		switch (selected_opt){
			
			case OptionID::HELP:
				help = true;
				break;
			
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
		
		prevOpt = optind;
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
	outputFilePath = "1";
	graph_outputFilePath.clear();
	graph_outputFormat.clear();
	verifyReduction = true;
	unicode = true;
	ansi = true;
	tabSize = 4;
	help = false;
}


// ------------------------------------------------------------------------------------------ //