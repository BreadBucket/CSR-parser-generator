#include "Generator.hpp"
#include <iostream>
#include <fstream>
#include <regex>

#include "Tab.hpp"
#include "data.hpp"
#include "Warning.hpp"


using namespace std;
using namespace csr;


// DEBUG
#include "ANSI.h"


// ----------------------------------- [ Prototypes ] --------------------------------------- //


namespace csr::GeneratorTemplate {
	void generateTokenEnum(ostream& out, const Tab& tab, const vector<shared_ptr<Symbol>>& symbols);
	void generateStateSwitch(ostream& out, const Tab& tab, const vector<State*> states);
	void generateTransitionSwitch(ostream& out, const Tab& tab, const vector<State*> states);
	void generateReductions(ostream& out, const Tab& tab, const vector<shared_ptr<Reduction>>& reductions);
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


void Generator::convertBadChars(string& s){
	for (int i = 0 ; i < s.size() ; i++){
		char c = s[i];
		if (!isalpha(c) && !isdigit(c))
			s[i] = '_';
	}
	
}

void Generator::generateSymbolNames(const vector<shared_ptr<Symbol>>& symbols){
	for (const shared_ptr<Symbol> sym : symbols){
		if (sym == nullptr){
			continue;
		} else if (sym->name.empty()){
			sym->cname = "TOKEN_" + to_string(reinterpret_cast<long>(&sym->name));
		} else {
			sym->cname = "TOKEN_" + sym->name;
			convertBadChars(sym->cname);
		}
	}
}


void Generator::generateReductionNames(const vector<shared_ptr<Reduction>>& reductions){
	for (int i = 0 ; i < reductions.size() ; i++){
		if (reductions[i] != nullptr){
			reductions[i]->cname = "REDUCTION_" + to_string(reductions[i]->id);
			convertBadChars(reductions[i]->cname);
		}
	}
}

void Generator::generateStateNames(const vector<State*>& states){
	for (State* state : states){
		if (state != nullptr){
			state->cname = "STATE_" + to_string(state->id);
			convertBadChars(state->cname);
		}
	}
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


void GeneratorTemplate::generateTokenEnum(ostream& out, const Tab& tab, const vector<shared_ptr<Symbol>>& symbols){
	for (int i = 0 ; i < symbols.size() ; i++){
		if (i > 0)
			out << ",\n";
		
		if (symbols[i]->cname.empty()){
			throw CSRException("Internal error: Empty symbol c-name.");
		}
		
		out << tab << symbols[i]->cname;
	}
}


// ----------------------------------- [ Structures ] --------------------------------------- //


struct TemplateParser {
// ---------------------------------------------------------------- //
public:
	ostream& out;
	const char* p = nullptr;
	
public:
	string macro = {};
	string body = {};
	Tab tab = {};
	
public:
	cmatch m;
	
// ---------------------------------------------------------------- //
public:
	bool next();
	static Tab getTab(const char* a, const char* b);
	
// ---------------------------------------------------------------- //
};


// ----------------------------------- [ Functions ] ---------------------------------------- //


Tab TemplateParser::getTab(const char* a, const char* b){
	Tab tab = {};
	
	if (*a == 0){
		return tab;
	}
	
	tab.c = *a;
	tab.n = 0;
	
	while (a != b){
		if (*a == tab.c)
			tab.n++;
		a++;
	}
	
	if (tab.c == '\t'){
		tab.w = 1;
	} else if (tab.c == ' '){
		if (tab.n % 4 == 0)
			tab.w = 4;
		else if (tab.n % 2 == 0)
			tab.w = 2;
		else
			tab.w = 1;
	} else {
		tab.w = 1;
	}
	
	return tab;
};



bool TemplateParser::next(){
	static const regex r = regex(
		"^(?:"
			"("
				"([^\\S\n]*)//[^\\S\n]*[$]START[^\\S\n]+(\\S+)[^\\S\n]*[$].*?\n"
				"([\\S\\s]*?)\n?"
				"[^\\S\n]*//[^\\S\n]*?[$]END[^\\S\n]*[$].*?"
			")|("
				"([^\\S\n]*)//[^\\S\n]*[$]MACRO[^\\S\n]+(\\S+)[^\\S\n]*[$].*?"
			")|("
				".*?"
			")"
		")(?=\n|$)"
		,
		regex_constants::optimize
	);
	
	const int i_region = 1;
	const int i_region_tab = i_region + 1;
	const int i_region_arg = i_region_tab + 1;
	const int i_region_body = i_region_arg + 1;
	const int i_macro = i_region_body + 1;
	const int i_macro_tab = i_macro + 1;
	const int i_macro_arg = i_macro_tab + 1;
	const int i_text = i_macro_arg + 1;
	
	
	// Pipe plain text until macro line
	while (true){
		bool newline = (*p == '\n');
		
		if (*p == '\0'){
			return false;
		} else if (*p == '\n'){
			p++;
		}
		
		// Match and move pointer
		if (!regex_search(p, m, r)){
			if (newline)
				out << '\n';
			return false;
		} else {
			p = m[0].second;
		}
		
		// Region
		if (m[i_region].matched){
			macro = m[i_region_arg].str();
			body = m[i_region_body].str();
			tab = getTab(m[i_region_tab].first, m[i_region_tab].second);
			return true;
		}
		
		// Macro
		else if (m[i_macro].matched){
			macro = m[i_macro_arg].str();
			body.clear();
			tab = getTab(m[i_macro_tab].first, m[i_macro_tab].second);
			return true;
		}
		
		// Plain text
		else {
			if (newline)
				out << '\n';
			out.write(m[0].first, m[0].length());
		}
		
		continue;
	}
	
	return false;
}


void Generator::generate(const Graph& graph, const SymbolEnum& symEnum){
	// ofstream file = ofstream("obj/switch.c");
	// ostream& out = file;
	ostream& out = cout;
	
	
	TemplateParser t = { out, data::template_DFA_c };
	
	
	generateSymbolNames(symEnum.getSymbols());
	generateReductionNames(graph.getReductions());
	generateStateNames(graph.states);
	
	
	// writeTokenEnum(cout, {' ', 2}, symEnum.getSymbols());
	// GeneratorTemplate::generateStateSwitch(cout, {' ', 2}, graph.states);
	// GeneratorTemplate::generateReductions(cout, {' ', 2}, graph.getReductions());
	
	while (t.next()){
		cout << '\n';
		if (t.macro == "enum")
			GeneratorTemplate::generateTokenEnum(out, t.tab, symEnum.getSymbols());
		else if (t.macro == "state_switch")
			GeneratorTemplate::generateStateSwitch(out, t.tab, graph.states);
		else if (t.macro == "transition_switch")
			GeneratorTemplate::generateTransitionSwitch(out, t.tab, graph.states);
		else if (t.macro == "reductions")
			GeneratorTemplate::generateReductions(out, t.tab, graph.getReductions());
	}
	
	
}


// ------------------------------------------------------------------------------------------ //