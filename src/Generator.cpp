#include "Generator.hpp"
#include <regex>

#include "Tab.hpp"
#include "Symbol.hpp"
#include "SourceString.hpp"
#include "SymbolEnum.hpp"
#include "Reduction.hpp"
#include "Document.hpp"
#include "Graph.hpp"
#include "data.hpp"


using namespace std;
using namespace csr;


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


void Generator::generateReductionItemNames(const vector<Connection*>& connections){
	static long guid = 0;
	for (Connection* c : connections){
		if (c != nullptr && c->reductionItem != nullptr){
			c->reductionItem->cname = "REDUCTIONITEM_" + to_string(guid++);
			convertBadChars(c->reductionItem->cname);
		}
	}
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


static void writeUserCode(ostream& out, const vector<SourceString>& v){
	for (const SourceString& s : v){
		if (s.size() > 0)
			out << '\n' << s;
	}
}


// ------------------------------------------------------------------------------------------ //


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


Tab TemplateParser::getTab(const char* a, const char* b){
	Tab tab = {};
	
	if (!isspace(*a)){
		return tab;
	}
	
	tab.c = *a;
	tab.n = 0;
	
	while (a != b){
		if (*a != tab.c)
			break;
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
				"([^\\S\n]*)//[^\\S\n]*[$]BEGIN[^\\S\n]+(\\S+)[^\\S\n]*.*?\n"
				"([\\S\\s]*?)\n?"
				"[^\\S\n]*//[^\\S\n]*?[$]END[^\\S\n]*.*?"
			")|("
				"(.*?)([^\\S\n]*)//[^\\S\n]*[$]MACRO[^\\S\n]+(\\S+)[^\\S\n]*.*?"
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
	const int i_macro_body = i_macro + 1;
	const int i_macro_tab = i_macro_body + 1;
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
			body = m[i_macro_body].str();
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


// ----------------------------------- [ Functions ] ---------------------------------------- //


void Generator::processTemplate(ostream& out, const char* data){
	TemplateParser t = { out, data };
	while (t.next()){
		
		// Regular content macros
		if (t.macro == "enum"){
			out << '\n';
			Generator::generateTokenEnum(out, t.tab, doc->symEnum->getSymbols());
		} else if (t.macro == "token_name_switch"){
			out << '\n';
			Generator::generateTokenEnumNameSwitch(out, t.tab, doc->symEnum->getSymbols());
		} else if (t.macro == "state_switch"){
			out << '\n';
			Generator::generateStateSwitch(out, t.tab, doc->graph->states);
		} else if (t.macro == "transition_switch"){
			out << '\n';
			Generator::generateTransitionSwitch(out, t.tab, doc->graph->states);
		} else if (t.macro == "reduction_items"){
			out << '\n';
			Generator::generateReductionItems(out, t.tab, *doc->graph);
		} else if (t.macro == "reductions"){
			out << '\n';
			Generator::generateReductions(out, t.tab, *doc);
		}
		
		// Main header macros
		else if (t.macro == "include_header"){
			if (!headerPath.empty()){
				out << '\n';
				out << t.tab << "#include \"" << headerPath << "\"";
			}
		} else if (t.macro == "_inline_header"){
			if (inline_header){
				out << '\n';
				processTemplate(out, t.body.c_str());
			}
		} else if (t.macro == "inline_header"){
			out << '\n';
			processTemplate(out, data::template_DFA_inline_h);
		}
		
		// Token header macros
		else if (t.macro == "include_tokenHeader"){
			if (!tokenHeaderPath.empty()){
				out << '\n';
				out << t.tab << "#include \"" << tokenHeaderPath << "\"";
			}
		} else if (t.macro == "include_tokenHeader-noMainHeader"){
			if (!tokenHeaderPath.empty() && headerPath.empty()){
				out << '\n';
				out << t.tab << "#include \"" << tokenHeaderPath << "\"";
			}
		} else if (t.macro == "_inline_tokenHeader"){
			if (inline_tokenHeader){
				out << '\n';
				processTemplate(out, t.body.c_str());
			}
		} else if (t.macro == "inline_tokenHeader"){
			processTemplate(out, data::template_tokenHeader_inline_h);
		}
		
		// User code
		else if (t.macro == "_usercode"){
			if (doc->code.size() > 0){
				out << '\n';
				processTemplate(out, t.body.c_str());
			}
		} else if (t.macro == "usercode"){
			writeUserCode(out, doc->code);
		}
		
	}
};


void Generator::generate(Document& doc){
	if (doc.symEnum == nullptr || doc.graph == nullptr)
		return;
	else if (out_tokenHeader.isVoid() && out_header.isVoid() && out.isVoid())
		return;
	
	this->doc = &doc;
	
	generateSymbolNames(doc.symEnum->getSymbols());
	generateReductionNames(doc.reductions);
	generateReductionItemNames(doc.graph->connections);
	generateStateNames(doc.graph->states);
	
	if (!out_tokenHeader.isVoid())
		processTemplate(out_tokenHeader, data::template_TokenHeader_h);
	if (!out_header.isVoid())
		processTemplate(out_header, data::template_DFA_h);
	if (!out.isVoid())
		processTemplate(out, data::template_DFA_c);
	
}


// ------------------------------------------------------------------------------------------ //