#include "Generator.hpp"
#include <fstream>
#include <regex>

#include "Tab.hpp"
#include "data.hpp"
#include "Warning.hpp"


using namespace std;
using namespace csr;


// DEBUG
#include "ANSI.h"


// ------------------------------------[ Properties ] --------------------------------------- //


void writeStateSwitch(ostream& out, const Tab& tab, const vector<State*> states);
void writeReduction(ostream& out, const Tab& tab, const Reduction& r);
void writeTokenEnum(ostream& out, const Tab& tab, const vector<shared_ptr<Symbol>>& symbols);


// ----------------------------------- [ Functions ] ---------------------------------------- //


void Generator::generateEnumName(const string& src, string& out){
	out = "TOKEN_" + src;
	
	if (src.empty()){
		out += to_string(reinterpret_cast<long>(&src));
		return;
	}
	
	for (int i = 0 ; i < out.size() ; i++){
		char c = out[i];
		if (!isalpha(c) && !isdigit(c) && c != '_')
			out[i] = '_';
	}
	
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


void writeTokenEnum(ostream& out, const Tab& tab, const vector<shared_ptr<Symbol>>& symbols){
	for (int i = 0 ; i < symbols.size() ; i++){
		if (symbols[i]->cname.empty())
			Generator::generateEnumName(symbols[i]->name, symbols[i]->cname);
		
		out << tab << symbols[i]->cname;
		if (i+1 < symbols.size())
			out << ",";
			
		out << "\n";
	}
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


bool pipeUntilMacro(const char*& p, ostream& stream, string& out_macro, Tab* tab){
	static const regex r = regex(
		"([\\S\\s]+?\n)"				// Match everything before macro
		"(\t*| *)[^\\S\n]*//[^\\S\n]*"		// Match comment
		"[$][^\\S\n]*MACRO[^\\S\n]*[$]"	// Match "$MACRO$"
		"[^\\S\n]*(\\S+)[^\\S\n]*"		// Match macro name
		".*?\n",						// Until EOL
		regex_constants::optimize
	);
	
	if (p == nullptr){
		return false;
	}
	
	cmatch m;
	if (regex_search(p, m, r)){
		stream.write(m[1].first, m[1].length());
		out_macro = m[3].str();
		
		if (tab != nullptr){
			if (m[2].length() > 0){
				if (*m[2].first == ' ')
					*tab = Tab(m[2].length()/4, ' ', 4);
				else
					*tab = Tab(m[2].length(), '\t', 1);
			}
		}
		
		p = m[0].second;
		return true;
	}
	
	// No macro found, pipe the rest of the data
	else {
		stream << p;
		p = nullptr;
		return false;
	}
	
}


void Generator::generate(const Graph& graph, const SymbolEnum& symEnum){
	ofstream file = ofstream("obj/switch.c");
	// ostream& out = cout;
	ostream& out = file;
	
	const char* p = data::template_DFA_c;
	string macro;
	Tab tab;
	
	
	
	// writeStateSwitch(cout, {' ', 2}, graph.states);
	// writeTokenEnum(cout, {' ', 2}, symEnum.getSymbols());
	// writeReduction(cout, {' ', 2}, *graph.getReductions()[0]);
	
	
	while (pipeUntilMacro(p, out, macro, &tab)){
		if (macro == "switch")
			writeStateSwitch(out, tab, graph.states);
		else if (macro == "enum")
			writeTokenEnum(out, tab, symEnum.getSymbols());
	}
	
	
}


// ------------------------------------------------------------------------------------------ //