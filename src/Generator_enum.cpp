#include "Generator.hpp"

#include <vector>
#include <memory>
#include <ostream>

#include "Tab.hpp"
#include "Symbol.hpp"
#include "CSRException.hpp"


using namespace std;
using namespace csr;


// ----------------------------------- [ Functions ] ---------------------------------------- //


void Generator::generateTokenEnum(ostream& out, const Tab& tab, const vector<shared_ptr<Symbol>>& symbols){
	const Tab tab1 = tab + 1;
	out << tab << "typedef enum {";
	
	for (int i = 0 ; i < symbols.size() ; i++){
		if (i > 0)
			out << ",";
		
		if (symbols[i]->cname.empty()){
			throw CSRException("Internal error: Empty symbol c-name.");
		}
		
		out << '\n' << tab1 << symbols[i]->cname;
	}
	
	out << '\n' << tab << "} CSRTokenID;";
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


void Generator::generateTokenEnumNameSwitch(ostream& out, const Tab& tab, const vector<shared_ptr<Symbol>>& symbols){
	const Tab tab1 = tab + 1;
	
	out << tab << "switch (id){\n";
	
	for (int i = 0 ; i < symbols.size() ; i++){
		if (symbols[i]->cname.empty()){
			throw CSRException("Internal error: Empty symbol c-name.");
		}
		
		out << tab1 << "case " << symbols[i]->cname << ":";
		out << '\t' << "return \"" << symbols[i]->name << "\";\n";
	}
	
	out << tab << "}";
}


// ------------------------------------------------------------------------------------------ //