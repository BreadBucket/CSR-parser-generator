#include "CLI.hpp"

extern "C" {
	#include <getopt.h>
}

#include <cstdint>
#include <cstring>
#include <unordered_set>
#include <stdexcept>
#include <iostream>


using namespace std;
using namespace CLI;


// ------------------------------------[ Variables ] ---------------------------------------- //



namespace CLI {
	string programName = "csrpg";
	
	optional<string> inputFilePath;
	
	optional<string> outputFilePath;
	optional<string> outputHeaderFilePath;
	optional<string> outputHeaderFilePath_token;
	
	optional<string> graph_outputFilePath;
	optional<string> graph_outputFormat;
	
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
	OUT_HEADER,
	OUT_HEADER_TOKEN,
	GRAPH_OUTPATH,
	GRAPH_FORMAT,
	UNICODE,
	ASCII,
	ANSI,
	TAB_SIZE,
	VERIFY_REDUCTION,
} selected_opt;


const char* const short_options = "i:o:h:g:t:pa";


const struct option long_options[] = {
	{"help",        no_argument,       (int*)&selected_opt, OptionID::HELP             },
	{"input",       required_argument, (int*)&selected_opt, OptionID::IN               },
	{"output",      required_argument, (int*)&selected_opt, OptionID::OUT              },
	{"header",      required_argument, (int*)&selected_opt, OptionID::OUT_HEADER       },
	{"token",       required_argument, (int*)&selected_opt, OptionID::OUT_HEADER_TOKEN },
	{"graph",       optional_argument, (int*)&selected_opt, OptionID::GRAPH_OUTPATH    },
	{"graphFormat", required_argument, (int*)&selected_opt, OptionID::GRAPH_FORMAT     },
	{"productions", no_argument,       (int*)&selected_opt, OptionID::VERIFY_REDUCTION },
	{"tabSize",     required_argument, (int*)&selected_opt, OptionID::TAB_SIZE         },
	{"ascii",       no_argument,       (int*)&selected_opt, OptionID::ASCII            },
	{0, 0, 0, 0}
};


// ----------------------------------- [ Functions ] ---------------------------------------- //


OptionID shortOptionToLong(char c){
	switch (c){
		case 'i':
			return OptionID::IN;
		case 'o':
			return OptionID::OUT;
		case 'h':
			return OptionID::OUT_HEADER;
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
	
	
	// Mark already processed options
	unordered_set<OptionID> marked = {};
	auto mark = [&](OptionID id){
		auto p = marked.emplace(id);
		if (!get<1>(p))
			throw runtime_error(string("Duplicate argument forbidden: '").append(v[prevOpt]).append("'."));
	};
	
	
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
				mark(selected_opt);
				inputFilePath = optarg;
				break;
			
			case OptionID::OUT:
				mark(selected_opt);
				outputFilePath = optarg;
				break;
			
			case OptionID::OUT_HEADER:
				mark(selected_opt);
				outputHeaderFilePath = optarg;
				break;
			
			case OptionID::OUT_HEADER_TOKEN:
				mark(selected_opt);
				outputHeaderFilePath_token = optarg;
				break;
			
			case OptionID::GRAPH_OUTPATH:
				if (optarg != nullptr){
					mark(selected_opt);
					graph_outputFilePath = optarg;
				} else {
					graph_outputFilePath = "1";
				}
				break;
			
			case OptionID::GRAPH_FORMAT:
				mark(selected_opt);
				graph_outputFormat = optarg;
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
		prevOpt = optind;	// optind moved to first non-opt argument
		
		if (marked.contains(OptionID::IN)){
			throw runtime_error(string("Multiple input paths specified: '").append(v[prevOpt]).append("'."));
		}
		
		mark(OptionID::IN);
		inputFilePath = v[optind];
		optind++;
	}
	
	
	delete[] v;
	return;
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


void CLI::clear(){
	programName = "csrpg";
	inputFilePath.reset();
	outputFilePath.reset();
	outputHeaderFilePath.reset();
	outputHeaderFilePath_token.reset();
	graph_outputFilePath.reset();
	graph_outputFormat.reset();
	verifyReduction = true;
	unicode = true;
	ansi = true;
	tabSize = 4;
	help = false;
}


// ------------------------------------------------------------------------------------------ //