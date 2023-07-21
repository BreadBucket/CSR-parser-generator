#include <vector>
#include <set>

#include "Symbol.hpp"
#include "Reduction.hpp"


using namespace std;
using namespace CSR;


// ----------------------------------- [ Functions ] ---------------------------------------- //


// vector<Symbol_> Reduction::createEnum(const vector<Reduction>& v){
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


bool equals(const vector<Symbol>& a, const vector<Symbol>& b){
	if (a.size() != b.size())
		return false;
	
	for (int i = 0 ; i < a.size() ; i++){
		if (a[i].name != b[i].name)
			return false;
	}
	
	return true;
}


bool Reduction::distinct(const vector<Reduction>& v, int* a, int* b){
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


bool Reduction::validateSize(const vector<Reduction>& v, int* out_i){
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