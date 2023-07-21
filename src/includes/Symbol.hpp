#pragma once
#include "SourceString.hpp"


namespace CSR {
class Symbol {
// ------------------------------------[ Properties ] --------------------------------------- //
public:
	SourceString name;	// The name of the symbol
	SourceString atr;	// Optional identifier
	
// ----------------------------------- [ Functions ] ---------------------------------------- //
public:
	inline void clear(){
		name.clear();
		atr.clear();
	}
	
// ------------------------------------------------------------------------------------------ //
};
}
