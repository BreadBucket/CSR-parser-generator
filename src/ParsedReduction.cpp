#include "ParsedReduction.hpp"

#include <vector>
#include <set>


using namespace std;
using namespace csr;


// ----------------------------------- [ Functions ] ---------------------------------------- //


bool equals(const vector<ParsedSymbol>& a, const vector<ParsedSymbol>& b){
	if (&a == &b)
		return true;
	else if (a.size() != b.size())
		return false;
	
	for (int i = 0 ; i < a.size() ; i++){
		if (a[i].name.name != b[i].name.name)
			return false;
	}
	
	return true;
}


bool ParsedReduction::distinct(const vector<shared_ptr<ParsedReduction>>& v, int* a, int* b){
	for (int  i = 0   ;  i < v.size() ;  i++){
	for (int ii = i+1 ; ii < v.size() ; ii++){
		
		if (v[i] == nullptr || v[ii] == nullptr)
			continue;
		else if (equals(v[i]->left, v[ii]->left)){
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


bool ParsedReduction::validateLength(const vector<shared_ptr<ParsedReduction>>& v, int* out_i){
	for (int i = 0 ; i < v.size() ; i++){
		if (v[i] == nullptr)
			continue;
		else if (v[i]->left.size() < v[i]->right.size()){
			if (out_i != nullptr)
				*out_i = i;
			return false;
		}
	}
	return true;
}


// ------------------------------------------------------------------------------------------ //