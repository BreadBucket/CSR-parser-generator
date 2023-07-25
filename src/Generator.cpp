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


void writeTransitionCase(ostream& out, const Tab& tab, const Connection& c){
	Tab tab1 = tab + 1;
	
	// Generate token name
	if (c.symbol->cname.empty()){
		Generator::generateEnumName(c.symbol->name, c.symbol->cname);
	}
	
	out << tab << "case " << c.symbol->cname << ":\n";
	
	if (c.to != nullptr){
		out << tab1 << "nextState = " << c.to->id << ";\n";
	} else if (c.reductionItem != nullptr){
		out << tab1 << ANSI_RED "REDUCTION\n" ANSI_RESET;
	}
	
	out << tab1 << "break;\n";
}


void writeTransitionSwitch(ostream& out, const Tab& tab, const vector<Connection*>& transitions){
	Tab tab1 = tab + 1;
	Tab tab2 = tab + 2;
	
	out << tab << "switch (currentToken){\n";
	
	for (const Connection* c : transitions){
		if (c != nullptr && c->symbol != nullptr)
			writeTransitionCase(out, tab1, *c);
	}
	
	out << tab1 << "default:\n";
	out << tab2 << "nextState = 0;\n";
	out << tab2 << "break;\n";
	out << tab1 << "}\n";
}



void writeStateCase(ostream& out, const Tab& tab, const State& state){
	Tab tab1 = tab + 1;
	Tab tab2 = tab + 2;
	
	out << tab << "case " << state.id << ":\n";
	
	writeTransitionSwitch(out, tab1, state.connections);
	
	out << tab1 << "break;\n";
	out << tab1 << "\n";
}


void writeStateSwitch(ostream& out, const Tab& tab, const vector<State*> states){
	Tab tab1 = tab + 1;
	Tab tab2 = tab + 2;
	
	out << tab << "switch (currentState){\n";
	
	for (const State* state : states){
		if (state != nullptr)
			writeStateCase(out, tab1, *state);
	}
	
	out << tab1 << "default:\n";
	out << tab2 << "break;\n";
	out << tab << "}\n";
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


void writeTokenEnum(ostream& out, const Tab& tab, const vector<shared_ptr<Symbol>>& symbols){
	Tab tab1 = tab + 1;
	out << tab << "typedef enum {\n";
	
	for (int i = 0 ; i < symbols.size() ; i++){
		if (symbols[i]->cname.empty())
			Generator::generateEnumName(symbols[i]->name, symbols[i]->cname);
		
		out << tab1 << symbols[i]->cname;
		if (i+1 < symbols.size())
			out << ",";
		out << "\n";
	}
	
	out << tab << "} TokenID;\n";
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


void Generator::generate(const Graph& graph, const std::vector<std::shared_ptr<Symbol>>& symbols){
	ostream& out = cout;
	
	const char* p = data::template_DFA;
	string macro;
	Tab tab;
	
	
	// writeStateSwitch(cout, {' ', 2}, graph.states);
	writeTokenEnum(cout, {' ', 2}, symbols);
	
	
	// while (pipeUntilMacro(p, out, macro, &tab)){
		// if (macro == "switch")
		// 	writeStateSwitch(out, tab, graph.states);
		// if (macro == "enum")
		// 	writeTokenEnum(out, tab, symbols);
	// }
	
	
}


// ------------------------------------------------------------------------------------------ //