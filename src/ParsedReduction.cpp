#include "ParsedReduction.hpp"

#include <vector>
#include <set>


using namespace std;
using namespace csr;


// ----------------------------------- [ Functions ] ---------------------------------------- //


int ParsedReduction::findAlias(const string& alias) const {
	for (int i = 0 ; i < left.size() ; i++){
		if (left[i].name.alias == alias)
			return i;
	}
	return -1;
}


void ParsedReduction::resolveSymbolAliases(){
	
	// Create implicit aliases
	for (int i = 0 ; i < left.size() ; i++){
		if (left[i].name.alias.empty())
			left[i].name.alias = to_string(i);
		
		// Check for duplicate
		for (int ii = 0 ; ii < i ; ii++){
			if (left[ii].name.alias.c_str() == left[i].name.alias.c_str())
				throw ParserException(left[i].name.alias.start, "Duplicate alias.");
		}
		
	}
	
	for (int i = 0 ; i < right.size() ; i++){
		SymbolName& sym = right[i].name;
		
		// Resolve index alias
		if (sym.name.empty() && sym.alias.empty()){
			throw ParserException(loc, "Empty reduction symbol.");
		}
		
		// Name could be an alias
		else if (sym.alias.empty()){
			int p = findAlias(sym.name);
			
			if (p >= 0){
				sym.alias = move(sym.name);
				sym.name = left[p].name.name;
			}
			
		}
		
		// Find alias
		else {
			int p = findAlias(sym.alias);
			
			if (p < 0){
				throw ParserException(sym.alias.start, "Missing alias reference.");
			} else if (sym.name != left[p].name.name){
				throw ParserException(sym.name.start, "Aliased symbol name missmatch.");
			}
			
		}
		
	}
	
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