#pragma once
#include "SourceString.hpp"


namespace CSR {
class Symbol {
// ------------------------------------[ Properties ] --------------------------------------- //
public:
	SourceString name;	// The name of the symbol
	SourceString id;	// Optional identifier
	
// ---------------------------------- [ Constructors ] -------------------------------------- //
public:
	Symbol() = default;
	
// ----------------------------------- [ Functions ] ---------------------------------------- //
public:
	inline void clear(){
		name.clear();
		id.clear();
	}
	
// ------------------------------------------------------------------------------------------ //
};
}
