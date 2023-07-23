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
	
// ---------------------------------- [ Constructors ] -------------------------------------- //
public:
	Symbol() = default;
	
	explicit Symbol(SymbolID id) : id{id} {}
	
	explicit Symbol(SymbolID id, const std::string& name) : id{id}, name{name} {}
	explicit Symbol(SymbolID id, std::string&& name) : id{id}, name{move(name)} {}
	
// ------------------------------------------------------------------------------------------ //
};
