#include "ParserAbstraction.hpp"

using namespace std;
using namespace csr;


// ----------------------------------- [ Functions ] ---------------------------------------- //


void csr::enumerate(const vector<ParsedSymbol>& v, SymbolEnum& out){
	for (const ParsedSymbol& s : v){
		if (!s.name.name.empty())
			out.put(s.name.name);
	}
}


void csr::enumerate(const vector<ParsedReduction>& v, SymbolEnum& out){
	for (const ParsedReduction& r : v){
		enumerate(r.left, out);
		enumerate(r.right, out);
	}
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


Reduction* csr::createReduction(const ParsedReduction& pr, const SymbolEnum& symEnum){
	Reduction* r = new Reduction();
	
	r->left.reserve(pr.left.size());
	r->right.reserve(pr.right.size());
	
	for (const ParsedSymbol& s : pr.left){
		shared_ptr<Symbol> p = symEnum.share(s.getName());
		r->left.emplace_back(move(p));
	}
	
	for (const ParsedSymbol& s : pr.right){
		shared_ptr<Symbol> p = symEnum.share(s.getName());
		r->right.emplace_back(move(p));
	}
	
	return r;
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


template<typename T>
vector<T> csr::createReductions(const vector<ParsedReduction>& v, const SymbolEnum& symEnum){
	vector<T> r = {};
	r.reserve(v.size());
	
	for (int i = 0 ; i < v.size() ; i++){
		Reduction* p = createReduction(v[i], symEnum);
		p->id = ReductionID(i);
		r.emplace_back(p);
	}
	
	return r;
}


// Explicit instantiations for the pointer types
template vector<Reduction*> csr::createReductions<Reduction*>(const vector<ParsedReduction>& v, const SymbolEnum& symEnum);
template vector<const Reduction*> csr::createReductions<const Reduction*>(const vector<ParsedReduction>& v, const SymbolEnum& symEnum);
template vector<unique_ptr<Reduction>> csr::createReductions<unique_ptr<Reduction>>(const vector<ParsedReduction>& v, const SymbolEnum& symEnum);
template vector<unique_ptr<const Reduction>> csr::createReductions<unique_ptr<const Reduction>>(const vector<ParsedReduction>& v, const SymbolEnum& symEnum);
template vector<shared_ptr<Reduction>> csr::createReductions<shared_ptr<Reduction>>(const vector<ParsedReduction>& v, const SymbolEnum& symEnum);
template vector<shared_ptr<const Reduction>> csr::createReductions<	shared_ptr<const Reduction>>(const vector<ParsedReduction>& v, const SymbolEnum& symEnum);


// ------------------------------------------------------------------------------------------ //