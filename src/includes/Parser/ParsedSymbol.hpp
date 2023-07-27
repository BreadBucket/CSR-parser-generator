#pragma once
#include <vector>
#include <optional>

#include "SourceString.hpp"


namespace csr {
	struct SymbolName;
	class ParsedSymbol;
}


struct csr::SymbolName {
	SourceString name;
	SourceString alias;
};


class csr::ParsedSymbol {
// ---------------------------------- [ Structures ] ---------------------------------------- //
public:
	typedef std::vector<ParsedSymbol> Constructor;
	
// ------------------------------------[ Properties ] --------------------------------------- //
public:
	SymbolName name;
	std::optional<Constructor> constructor;
	
// ----------------------------------- [ Functions ] ---------------------------------------- //
public:
	const SourceString& getName() const {
		return name.name;
	}
	
	const SourceString& getAlias() const {
		return name.alias;
	}
	
// ----------------------------------- [ Functions ] ---------------------------------------- //
public:
	inline void clear(){
		name.name.clear();
		name.alias.clear();
		constructor.reset();
	}
	
// ------------------------------------------------------------------------------------------ //
};