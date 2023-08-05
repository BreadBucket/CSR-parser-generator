#include "AliasResolver.hpp"

#include <unordered_map>

#include "ParsedSymbol.hpp"
#include "ParserException.hpp"
#include "Error.hpp"

using namespace std;
using namespace csr;


// ---------------------------------- [ Structures ] ---------------------------------------- //


struct AliasResolver {
// ---------------------------------------------------------------- //
public:
	vector<shared_ptr<Reduction>>* reductions;
	SymbolEnum* symEnum;
	
// ---------------------------------------------------------------- //
private:
	const ParsedReduction* parsedReduction = nullptr;
	Reduction* reduction;
	unordered_map<string,int> aliases;
	
// ---------------------------------------------------------------- //
public:
	void resolve(const ParsedReduction&);
	
private:
	void resolveLeft();
	void resolveRight();
	
	/**
	 * @brief Recursively resolve RightSymbol and its constructor.
	 *        Function relies on a valid `aliases` map.
	 * @param psym ParsedSymbol to resolve recursively.
	 * @param sym Output RightSymbol if there is no error.
	 * @return true If symbol was successfuly resolved. Just a precaution, function shouldn't fail.
	 */
	bool resolve_rightSymbol(const ParsedSymbol& psym, Reduction::RightSymbol& sym);
	
// ---------------------------------------------------------------- //
};


// ----------------------------------- [ Functions ] ---------------------------------------- //


void AliasResolver::resolve(const ParsedReduction& r){
	this->parsedReduction = &r;
	
	this->reduction = new Reduction();
	this->reduction->id = ReductionID(reductions->size());
	reductions->emplace_back(this->reduction);
	
	aliases.clear();
	resolveLeft();
	resolveRight();
}


void csr::convertReductions(const vector<ParsedReduction>& v, vector<shared_ptr<Reduction>>& r, SymbolEnum& e){
	AliasResolver o = {};
	o.reductions = &r;
	o.symEnum = &e;
	
	for (const ParsedReduction& pr : v){
		o.resolve(pr);
	}
	
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


void AliasResolver::resolveLeft(){
	reduction->left.reserve(parsedReduction->left.size());
	
	// Left side
	for (int i = 0 ; i < parsedReduction->left.size() ; i++){
		const ParsedSymbol& sym = parsedReduction->left[i];
		
		// Automatic alias (index of symbol)
		if (!sym.getAlias().empty()){
			auto f = aliases.try_emplace(sym.getAlias(), i);
			if (!get<1>(f))
				throw ParserException(sym.getAlias().start, "Duplicate alias.");
		}
		
		// Automatic index alias
		aliases.try_emplace(to_string(i+1), i);
		
		// Add symbol to left side
		shared_ptr<Symbol> p = symEnum->put(sym.getName());
		reduction->left.emplace_back(move(p));
	}
	
}


// // ----------------------------------- [ Functions ] ---------------------------------------- //


void AliasResolver::resolveRight(){
	reduction->right.reserve(parsedReduction->right.size());
	
	for (const ParsedSymbol& psym : parsedReduction->right){
		Reduction::RightSymbol& sym = reduction->right.emplace_back(-1);
		
		if (!resolve_rightSymbol(psym, sym)){
			reduction->right.pop_back();
			warn("Failed to construct a symbol. Symbol dropped.");
		}
		
	}
	
}


inline bool isIndex(const string& s){
	for (unsigned char c : s){
		if (!isdigit(c))
			return false;
	}
	return !s.empty();
}


bool AliasResolver::resolve_rightSymbol(const ParsedSymbol& psym, Reduction::RightSymbol& sym){
	// Check for index alias
	if (isIndex(psym.getName())){
		auto p = aliases.find(psym.getName());
		
		if (p == aliases.end()){
			if (psym.getName() == "0")
				throw ParserException(psym.getName().start, "Missing referenced symbol in index alias. Symbol index alias starts with 1.");
			else
				throw ParserException(psym.getName().start, "Missing referenced symbol in index alias.");
			return false;
		} else if (!psym.getAlias().empty()){
			throw ParserException(psym.getAlias().start, "Index alias cannot have another alias.");
			return false;
		} else if (psym.constructor.has_value()){
			throw ParserException(psym.getAlias().start, "Index alias cannot have a constructor.");
			return false;
		}
		
		sym.emplace<Reduction::SymbolCopy>(p->second);
		return true;
	}
	
	// Resolve alias
	else if (!psym.getAlias().empty()){
		
		// Aliased symbol cannot have a constructor
		if (psym.constructor.has_value()){
			throw ParserException(psym.getAlias().start, "Aliased symbol copy cannot have a constructor.");
		}
		
		auto p = aliases.find(psym.getAlias());
		if (p == aliases.end()){
			throw ParserException(psym.getAlias().start, "Missing alias reference.");
		}
		
		sym.emplace<Reduction::SymbolCopy>(p->second);
		return true;
	}
	
	else if (psym.getName().empty()){
		return false;
	}
	
	// Name could be alias
	else if (!psym.constructor.has_value()){
		auto p = aliases.find(psym.getName());
		
		if (p != aliases.end()){
			sym.emplace<Reduction::SymbolCopy>(p->second);
			return true;
		}
		
	}
	
	// Symbol constructor (name is not alias)
	{
		Reduction::SymbolConstructor& ctr = sym.emplace<Reduction::SymbolConstructor>();
		
		ctr.symbol = symEnum->put(psym.getName());
		ctr.args.reserve(psym.constructor->size());
		
		for (const ParsedSymbol& _psym : *psym.constructor){
			Reduction::RightSymbol& _sym = ctr.args.emplace_back();
			
			if (!resolve_rightSymbol(_psym, _sym)){
				ctr.args.pop_back();
				warn("Failed to construct a symbol. Symbol dropped.");
			}
			
		}
		
		return true;
	}
	
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


// DEBUG
string str(const Reduction& r){
	string s = {};
	
	// Left
	for (int i = 0 ; i < r.left.size() ; i++){
		s += r.left[i]->name.c_str();
		s += ' ';
	}
	
	s += "->";
	
	// Recursive right symbol
	auto f = [&](auto& self, const Reduction::RightSymbol& sym) -> void {
		if (holds_alternative<Reduction::SymbolCopy>(sym)){
			s += to_string(get<Reduction::SymbolCopy>(sym) + 1);
		} else {
			const Reduction::SymbolConstructor& ctr = get<Reduction::SymbolConstructor>(sym);
			s += ctr.symbol->name;
			
			s += '(';
			for (int i = 0 ; i < ctr.args.size() ; i++){
				if (i > 0)
					s += ',';
				self(self, ctr.args[i]);
			}
			s += ')';
			
		}
	};
	
	// Right
	for (int i = 0 ; i < r.right.size() ; i++){
		s += ' ';
		f(f, r.right[i]);
	}
	
	return s;
}


// ------------------------------------------------------------------------------------------ //