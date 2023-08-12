#include "Generator.hpp"

#include <vector>
#include <ostream>

#include "Tab.hpp"
#include "Symbol.hpp"
#include "Reduction.hpp"
#include "Graph.hpp"
#include "CSRException.hpp"


using namespace std;
using namespace csr;


// ----------------------------------- [ Functions ] ---------------------------------------- //


void Generator::generateStateSwitch(ostream& out, const Tab& tab, const vector<State*> states){
	Tab tab1 = tab + 1;
	out << tab << "switch (_currentStateId){\n";
	
	for (const State* state : states){
		if (state != nullptr){
			out << tab1 << "case " << state->id << ":\t";
			out << "goto __" << state->cname << ";\n";
		}
	}
	
	out << tab << "}";
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


static void writeTransitionCase(ostream& out, const Tab& tab, const Connection& c){
	Tab tab1 = tab + 1;
	
	if (c.symbol->cname.empty()){
		throw CSRException("Internal error: Empty symbol c-name.");
	}
	
	out << tab << "case " << c.symbol->cname << ":";
	
	if (c.to != nullptr){
		out << '\n';
		out << tab1 << "_nextStateId = " << c.to->id << ";\n";
		out << tab1 << "break;\n";
	} else if (c.reductionItem != nullptr){
		out << '\n';
		out << tab1 << "goto __" << c.reductionItem->cname << ";\n";
	}
	
}


static void writeTransitionSwitch(ostream& out, const Tab& tab, const vector<Connection*>& transitions){
	const Tab tab1 = tab + 1;
	const Tab tab2 = tab + 2;
	
	out << tab << "switch (_currentTokenId){\n";
	
	for (const Connection* c : transitions){
		if (c != nullptr && c->symbol != nullptr)
			writeTransitionCase(out, tab1, *c);
	}
	
	out << tab1 << "default:\n";
	out << tab2 << "_nextStateId = 0;\n";
	out << tab2 << "break;\n";
	out << tab1 << "}\n";
}


void Generator::generateTransitionSwitch(ostream& out, const Tab& tab, const vector<State*> states){
	Tab tab1 = tab + 1;
	
	for (int i = 0 ; i < states.size() ; i++){
		if (states[i] != nullptr){
			if (i > 0)
				out << '\n';
			out << tab << "__" << states[i]->cname << ": {\n";
			writeTransitionSwitch(out, tab1, states[i]->connections);
			out << tab << "} goto __STATE_END;";
		}
	}
	
}


// ------------------------------------------------------------------------------------------ //