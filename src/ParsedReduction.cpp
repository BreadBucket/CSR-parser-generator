#include "ParsedReduction.hpp"

#include <vector>
#include <set>


using namespace std;
using namespace csr;


// ----------------------------------- [ Functions ] ---------------------------------------- //


// vector<Symbol_> ParsedReduction::createEnum(const vector<ParsedReduction>& v){
// 	auto cmp = [](const string& a, const string& b) -> bool {
// 		if (a.size() < b.size())
// 			return true;
// 		else if (a.size() == b.size())
// 			return a < b;
// 		else
// 			return false;
// 	};
	
// 	set<string,decltype(cmp)> symbols;
// 	vector<Symbol_> symbolEnum;
	
// 	// Insert into set
// 	for (int i = 0 ; i < v.size() ; i++){
// 		for (int ii = 0 ; ii < v[i].left.size() ; ii++)
// 			symbols.insert(v[i].left[ii].name);
// 		for (int ii = 0 ; ii < v[i].right.size() ; ii++)
// 			symbols.insert(v[i].right[ii].name);
// 	}
	
// 	// Enumerate
// 	int id = 1;
// 	for (auto p = symbols.begin() ; p != symbols.end() ; p++){
// 		Symbol_& s = symbolEnum.emplace_back();
// 		s.name = move(const_cast<string&>(*p));
// 		s.id = id++;
// 	}
	
// 	return symbolEnum;
// }


// ----------------------------------- [ Functions ] ---------------------------------------- //


void ParsedReduction::resolveSymbolAliases(){
	// for (int i = 0 ; i < right.size() ; i++){
	// 	ParsedSymbol& sym = right[i];
		
		
	// 	// Resolve index alias
	// 	if (sym.name.name.empty()){
			
	// 		if (!sym.name.alias.has_value() || sym.name.alias->empty()){
	// 			throw ParserException(loc, "Empty reduction symbol.");
	// 		}
			
	// 		SourceString& s = *sym.name.alias;
			
	// 		long n = 0;
	// 		for (int i = 0 ; i < s.size() ; i++){
				
	// 		}
			
			
	// 		// // Is an index
	// 		// if (isdigit(*sym.name.alias->begin())){
				
	// 		// }
			
			
			
	// 	}
		
	// }
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


bool equals(const vector<ParsedSymbol>& a, const vector<ParsedSymbol>& b){
	if (a.size() != b.size())
		return false;
	
	for (int i = 0 ; i < a.size() ; i++){
		if (a[i].name.name != b[i].name.name)
			return false;
	}
	
	return true;
}


bool ParsedReduction::distinct(const vector<ParsedReduction>& v, int* a, int* b){
	for (int  i = 0   ;  i < v.size() ;  i++){
	for (int ii = i+1 ; ii < v.size() ; ii++){
		
		if (equals(v[i].left, v[ii].left)){
			if (a != nullptr)
				*a = i;
			if (b != nullptr)
				*b = ii;
			return false;
		}
		
	}}
	return true;
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


bool ParsedReduction::validateSize(const vector<ParsedReduction>& v, int* out_i){
	for (int i = 0 ; i < v.size() ; i++){
		if (v[i].left.size() < v[i].right.size()){
			if (out_i != nullptr)
				*out_i = i;
			return false;
		}
	}
	return true;
}


// ------------------------------------------------------------------------------------------ //