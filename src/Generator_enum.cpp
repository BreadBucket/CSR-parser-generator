#include <vector>
#include <memory>
#include <ostream>

#include "Tab.hpp"
#include "Symbol.hpp"
#include "CSRException.hpp"


using namespace std;
using namespace csr;


// ----------------------------------- [ Prototypes ] --------------------------------------- //


namespace csr::GeneratorTemplate {
	void generateTokenEnum(ostream& out, const Tab& tab, const vector<shared_ptr<Symbol>>& symbols);
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


void GeneratorTemplate::generateTokenEnum(ostream& out, const Tab& tab, const vector<shared_ptr<Symbol>>& symbols){
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


// ------------------------------------------------------------------------------------------ //