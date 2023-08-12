#include "Generator.hpp"

#include <vector>
#include <ostream>

#include "utils.hpp"
#include "Tab.hpp"
#include "Symbol.hpp"
#include "Reduction.hpp"
#include "ParsedReduction.hpp"
#include "Document.hpp"
#include "CSRException.hpp"


using namespace std;
using namespace csr;


// ----------------------------------- [ Prototypes ] --------------------------------------- //


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


static ostream& getToken(ostream& out, int i){
	if (i < 0)
		out << "(CSRToken*)_tokenStack->v[_tokenStack->count - " << (-i) << "]";
	else
		out << "(CSRToken*)_tokenStack->v[" << i << "]";
	return out;
}


static ostream& getTokenAddr(ostream& out, int i){
	if (i < 0)
		out << "(CSRToken**)&_tokenStack->v[_tokenStack->count - " << (-i) << "]";
	else
		out << "(CSRToken**)&_tokenStack->v[" << to_string(i) << "]";
	return out;
}


static ostream& getBufferTokenAddr(ostream& out, int i){
	if (i < 0)
		out << "(CSRToken**)&_tokenBuffer->v[_tokenBuffer->count - " << (-i) << "]";
	else
		out << "(CSRToken**)&_tokenBuffer->v[" << to_string(i) << "]";
	return out;
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


static void constructSymbol(ostream& out, const Reduction& r, const Reduction::SymbolConstructor& ctr){
	if (ctr.symbol->cname.empty()){
		throw CSRException("Internal error: Empty symbol c-name.");
	}
	
	out << "DFA_createToken(_dfa, " << ctr.symbol->cname << ", " << ctr.args.size();
	
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
	
	const SourceString* code = nullptr;
	if (r.source != nullptr && !r.source->code.empty()){
		code = &r.source->code;
	}
	
	const int _popTokens = r.left.size();
	const int _popStates = (r.left.size() - 1);
	
	
	// Construct symbols to buffer
	for (int i = r.right.size() - 1 ; i >= 0 ; i--){
		out << tab << "Stack_push(_tokenBuffer, ";
		handleRightSymbol(out, r, r.right[i]);
		out << ");\n";
	}
	
	
	if (code != nullptr){
		out << tab << '\n';
		out << tab << "int const _matchedTokens_count = " << r.left.size() << ";\n";
		out << tab << "int const _bufferedTokens_count = " << r.right.size() << ";\n";
		out << tab << "CSRToken** const _matchedTokens = "; getTokenAddr(out, -r.left.size()); out << ";\n";
		out << tab << "CSRToken** const _bufferedTokens = "; getBufferTokenAddr(out, -r.right.size()); out << ";\n";
		out << tab << "int _popTokens = " << _popTokens << ";\n";
		out << tab << "int _popStates = " << _popStates << ";\n";
		out << *code << '\n';
		out << tab << '\n';
	}
	
	// Pop symbols and states from stack
	if (code != nullptr){
		out << tab << "DFA_popTokens(_dfa, _popTokens);\n";
		out << tab << "DFA_popStates(_dfa, _popStates);\n";
	} else {
		out << tab << "DFA_popTokens(_dfa, " << _popTokens << ");\n";
		out << tab << "DFA_popStates(_dfa, " << _popStates << ");\n";
	}
	
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


void Generator::generateReductions(ostream& out, const Tab& tab, Document& doc){
	const Tab tab1 = tab + 1;
	
	for (const shared_ptr<Reduction>& r : doc.reductions){
		if (r == nullptr)
			continue;
		else if (r != *doc.reductions.begin())
			out << '\n';
		
		out << tab;
		writeReductionComment(out, *r);
		out << '\n';
		
		out << tab << "__" << r->cname << ": { ";
		out << '\n';
		writeReduction(out, tab1, *r);
		out << tab << "} goto __REDUCTIONS_END;\n";
	}
}


// ------------------------------------------------------------------------------------------ //