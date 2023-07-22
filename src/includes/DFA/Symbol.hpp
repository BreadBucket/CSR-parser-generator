#pragma once
#include <string>


namespace csr {
	class Symbol;
	typedef int SymbolID;
};


class csr::Symbol {
// ------------------------------------[ Properties ] --------------------------------------- //
public:
	SymbolID id;
	std::string name;
	
// ------------------------------------------------------------------------------------------ //
};