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


#include "util/ANSI.h"
#include "CLI.hpp"
#include "Parser.hpp"
#include "Generator.hpp"


using namespace std;
using namespace CSR;


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


unique_ptr<Parser> parseInput(const char* inputPath = nullptr){
	// Get input stream
	unique_ptr<ifstream> inf;
	istream* in;
	
	// Open input stream
	if (!isatty(fileno(stdin))){
		in = &cin;
	} else {
		inf = make_unique<ifstream>(inputPath);
		
		if (inf->fail()){
			fprintf(stderr, ANSI_BOLD "%s" ANSI_RESET ": " ANSI_RED "error" ANSI_RESET ": Failed to open input file '%s'.\n", CLI::programName, inputPath);
			return nullptr;
		}
		
		in = inf.get();
	}
	
	// Parse
	unique_ptr<Parser> parser = make_unique<Parser>();
	parser->tabSize = CLI::tabSize;
	
	try {
		parser->parse(*in);
	} catch (const ParserException& e) {
		fprintf(stderr, ANSI_BOLD "%s:%d:%d: " ANSI_RED "error" ANSI_RESET ": %s\n", inputPath, e.loc.row+1, e.loc.col+1, e.what());
		return nullptr;
	} catch (const exception& e) {
		fprintf(stderr, ANSI_BOLD "%s" ANSI_RESET ": " ANSI_RED "error" ANSI_RESET ": %s\n", CLI::programName, e.what());
		return nullptr;
	}
	
	
	return parser;
}


// ----------------------------------- [ Functions ] ---------------------------------------- //




vector<tuple<string,int>> distinct(const vector<Reduction>& v){
	auto cmp = [](const string& a, const string& b) -> bool {
		if (a.size() < b.size())
			return true;
		else if (a.size() == b.size())
			return a < b;
		else
			return false;
	};
	
	set<string,decltype(cmp)> symbols;
	vector<tuple<string,int>> symbolEnum;
	
	for (int i = 0 ; i < v.size() ; i++){
		for (int ii = 0 ; ii < v[i].left.size() ; ii++)
			symbols.insert(v[i].left[ii].name);
		for (int ii = 0 ; ii < v[i].right.size() ; ii++)
			symbols.insert(v[i].right[ii].name);
	}
	
	int val = 1;
	for (auto p = symbols.begin() ; p != symbols.end() ; p++){
		string& sym = const_cast<string&>(*p);
		symbolEnum.emplace_back(move(sym), val);
		val++;
	}
	
	return symbolEnum;
}


// --------------------------------- [ Main Functions ] ------------------------------------- //


int main(int argc, char const* const* argv){
	printf("================================\n");	// DEBUG
	// CLI::inputFilePath = "test/test.csg";	// DEBUG
	
	
	if (!parseCLI(argc, argv))
		return 1;
	
	unique_ptr<Parser> p = parseInput(CLI::inputFilePath);
	if (p == nullptr)
		return 1;
	
	
	// Enumerate symbol
	vector<tuple<string,int>> symEnum = distinct(*p->reductions);
	
	printf("Symbol enum:\n");
	for (auto& t : symEnum){
		printf("  %s = %d\n", get<0>(t).c_str(), get<1>(t));
	}
	
	
	return 0;
}


// ------------------------------------------------------------------------------------------ //