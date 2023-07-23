#pragma once
#include <vector>
#include <optional>

#include "SourceString.hpp"


namespace csr {
	struct SymbolName;
	class ParsedSymbol;
	typedef std::vector<SymbolName> SymbolConstructor;
}


struct csr::SymbolName {
	SourceString name;
	SourceString alias;
};


class csr::ParsedSymbol {
// ------------------------------------[ Properties ] --------------------------------------- //
public:
	SymbolName name;
	std::optional<SymbolConstructor> constructor;
	
// ----------------------------------- [ Functions ] ---------------------------------------- //
public:
	const SourceString& getName() const {
		return name.name;
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