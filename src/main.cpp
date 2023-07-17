#include <cstdint>
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
	
	bool success = false;
	try {
		parser->parse(*in);
		success = true;
	} catch (const ParserException& e) {
		fprintf(stderr, ANSI_BOLD "%s:%d:%d: " ANSI_RED "error" ANSI_RESET ": %s\n", inputPath, e.loc.row+1, e.loc.col+1, e.what());
	} catch (const exception& e) {
		fprintf(stderr, ANSI_BOLD "%s" ANSI_RESET ": " ANSI_RED "error" ANSI_RESET ": %s\n", CLI::programName, e.what());
	}
	
	
	// Close input file
	if (inf != nullptr){
		inf->close();
		delete inf;
	}
	
	if (success)
		return parser;
	else {
		delete parser;
		return nullptr;
	}
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
	CLI::inputFilePath = "test/test.csg";	// DEBUG
	
	
	if (!parseCLI(argc, argv))
		return 1;
	
	Parser* p = parseInput(CLI::inputFilePath);
	if (p == nullptr)
		return 1;
	
	
	// Enumerate symbol
	vector<tuple<string,int>> symEnum = distinct(*p->reductions);
	
	printf("Symbol enum:\n");
	for (auto& t : symEnum){
		printf("  %s = %d\n", get<0>(t).c_str(), get<1>(t));
	}
	
	
	// Generator g;
	// g.reductions = p->reductions;
	// g.code = p->codeSegments;
	
	// g.headerFile = &cout;
	// g.cFile = &cout;
	
	// g.generate();
	
	
	return 0;
}


// ------------------------------------------------------------------------------------------ //