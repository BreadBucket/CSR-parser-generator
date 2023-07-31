#include "Generator.hpp"
#include <fstream>

#include "Tab.hpp"
#include "ANSI.h"

using namespace std;
using namespace csr;


// ----------------------------------- [ Prototypes ] --------------------------------------- //


void handleRightSymbol(ostream&, const Reduction&, const Reduction::RightSymbol&);


// ----------------------------------- [ Functions ] ---------------------------------------- //


inline string& getCName(Symbol& sym){
	if (sym.cname.empty())
		Generator::generateEnumName(sym.name, sym.cname);
	return sym.cname;
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


void getToken(ostream& out, int i){
	if (i < 0)
		out << "(Token*)tokenStack.v[tokenStack.count - " << (-i) << + "]";
	else
		out << "(Token*)tokenStack.v[" << i << + "]";
}


void constructSymbol(ostream& out, const Reduction& r, const Reduction::SymbolConstructor& ctr){
	if (ctr.symbol->cname.empty())
		Generator::generateEnumName(ctr.symbol->name, ctr.symbol->cname);
	
	out << "createToken(" << ctr.symbol->cname << ", " << ctr.args.size();
	
	if (ctr.args.size() > 0){
		out << ", ";
		
		for (int i = 0 ; i < ctr.args.size() ; i++){
			if (i > 0)
				out << ", ";
			handleRightSymbol(out, r, ctr.args[i]);
		}
		
	}
	
	out << ")";
}


void handleRightSymbol(ostream& out, const Reduction& r, const Reduction::RightSymbol& rsym){
	if (holds_alternative<Reduction::SymbolCopy>(rsym)){
		const Reduction::SymbolCopy cpy = get<Reduction::SymbolCopy>(rsym);
		getToken(out, -r.left.size() + (int)cpy);
	} else {
		const Reduction::SymbolConstructor ctr = get<Reduction::SymbolConstructor>(rsym);
		constructSymbol(out, r, ctr);
	}
}


void writeReductionComment(ostream& out, const Reduction& r){
	out << "// ";
	
	for (int i = 0 ; i < r.left.size() ; i++){
		const shared_ptr<Symbol>& sym = r.left[i];
		if (sym != nullptr)
			out << sym->name << ' ';
		else
			out << "? ";
	}
	
	out << "->";
	
	for (int i = 0 ; i < r.right.size() ; i++){
		const shared_ptr<Symbol>& sym = r.getEffectiveRightSymbol(r.right[i]);
		if (sym != nullptr)
			out << ' ' << sym->name;
		else
			out << " ?";
	}
	
}


void writeReduction(ostream& out, const Tab& tab, const Reduction& r){
	const Tab tab1 = tab + 1;
	
	// Construct symbols to buffer
	for (int _i = 0, i_ = r.right.size() - 1 ; i_ >= 0 ; _i++, i_--){
		const Reduction::RightSymbol& sym = r.right[i_];
		
		out << tab << "Stack_push(tokenBuffer, ";
		handleRightSymbol(out, r, sym);
		out << ");\n";
		
	}
	
	out << tab << "\n";
	
	// Pop symbols and states from stack
	out << tab << "DFA_popToken(dfa, " << r.left.size() << ");\n";
	out << tab << "nextStateId = DFA_popState(dfa, " << (r.left.size() - 1) << ");\n";
	
	out << tab << "\n";
	
	out << tab << "pushNextState = false;\n";
	out << tab << "halt = false;\n";
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


void writeTransitionCase(ostream& out, const Tab& tab, const Connection& c){
	Tab tab1 = tab + 1;
	
	// Generate token name
	if (c.symbol->cname.empty()){
		Generator::generateEnumName(c.symbol->name, c.symbol->cname);
	}
	
	out << tab << "case " << c.symbol->cname << ":";
	
	if (c.to != nullptr){
		out << '\n';
		out << tab1 << "nextStateId = " << c.to->id << ";\n";
		out << tab1 << "break;\n";
	} else if (c.reductionItem != nullptr){
		out << " {\n" << tab1;
		writeReductionComment(out, *c.reductionItem);
		out << '\n';
		writeReduction(out, tab1, *c.reductionItem);
		out << tab << "} break;\n";
	}
	
}


void writeTransitionSwitch(ostream& out, const Tab& tab, const vector<Connection*>& transitions){
	const Tab tab1 = tab + 1;
	const Tab tab2 = tab + 2;
	
	out << tab << "switch (currentTokenId){\n";
	
	for (const Connection* c : transitions){
		if (c != nullptr && c->symbol != nullptr)
			writeTransitionCase(out, tab1, *c);
	}
	
	out << tab1 << "default:\n";
	out << tab2 << "nextStateId = 0;\n";
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
	
	out << tab << "switch (currentStateId){\n";
	
	for (const State* state : states){
		if (state != nullptr)
			writeStateCase(out, tab1, *state);
	}
	
	out << tab1 << "default:\n";
	out << tab2 << "break;\n";
	out << tab << "}\n";
}


// ------------------------------------------------------------------------------------------ //