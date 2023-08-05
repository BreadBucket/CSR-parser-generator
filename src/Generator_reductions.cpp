#include <vector>
#include <ostream>

#include "utils.hpp"
#include "Tab.hpp"
#include "Symbol.hpp"
#include "Reduction.hpp"
#include "Document.hpp"
#include "CSRException.hpp"


using namespace std;
using namespace csr;


// ----------------------------------- [ Prototypes ] --------------------------------------- //


namespace csr::GeneratorTemplate {
	void generateReductions(ostream& out, const Tab& tab, Document& doc);
}


static void handleRightSymbol(ostream&, const Reduction&, const Reduction::RightSymbol&);


// ----------------------------------- [ Functions ] ---------------------------------------- //


static void writeReductionComment(ostream& out, const Reduction& r){
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


// ----------------------------------- [ Functions ] ---------------------------------------- //


static void getToken(ostream& out, int i){
	if (i < 0)
		out << "(CSRToken*)tokenStack.v[tokenStack.count - " << (-i) << + "]";
	else
		out << "(CSRToken*)tokenStack.v[" << i << + "]";
}


static void constructSymbol(ostream& out, const Reduction& r, const Reduction::SymbolConstructor& ctr){
	if (ctr.symbol->cname.empty()){
		throw CSRException("Internal error: Empty symbol c-name.");
	}
	
	out << "createToken(dfa, " << ctr.symbol->cname << ", " << ctr.args.size();
	
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


static void handleRightSymbol(ostream& out, const Reduction& r, const Reduction::RightSymbol& rsym){
	if (holds_alternative<Reduction::SymbolCopy>(rsym)){
		const Reduction::SymbolCopy cpy = get<Reduction::SymbolCopy>(rsym);
		getToken(out, -r.left.size() + (int)cpy);
	} else {
		const Reduction::SymbolConstructor ctr = get<Reduction::SymbolConstructor>(rsym);
		constructSymbol(out, r, ctr);
	}
}


static void writeReduction(ostream& out, const Tab& tab, const Reduction& r){
	const Tab tab1 = tab + 1;
	
	// Construct symbols to buffer
	for (int _i = 0, i_ = r.right.size() - 1 ; i_ >= 0 ; _i++, i_--){
		const Reduction::RightSymbol& sym = r.right[i_];
		
		out << tab << "Stack_push(tokenBuffer, ";
		handleRightSymbol(out, r, sym);
		out << ");\n";
		
	}
	
	// Pop symbols and states from stack
	out << tab << "DFA_popTokens(dfa, " << r.left.size() << ");\n";
	out << tab << "nextStateId = DFA_popStates(dfa, " << (r.left.size() - 1) << ");\n";
	
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


void GeneratorTemplate::generateReductions(ostream& out, const Tab& tab, Document& doc){
	const Tab tab1 = tab + 1;
	
	for (const shared_ptr<Reduction>& r : doc.reductions){
		if (r == nullptr)
			continue;
		else if (r != *doc.reductions.begin())
			out << '\n';
		
		out << tab << "__" << r->cname << ": { ";
		writeReductionComment(out, *r);
		out << '\n';
		writeReduction(out, tab1, *r);
		out << tab << "} goto __REDUCTIONS_EPILOGUE;\n";
	}
}


// ------------------------------------------------------------------------------------------ //