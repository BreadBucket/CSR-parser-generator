#pragma once
#include <string>
#include <vector>
#include <optional>

#include "SourceString.hpp"
#include "ParserException.hpp"


namespace csr {
	struct SymbolName;
	class ParsedSymbol;
	class ParsedReduction;
	typedef std::vector<SymbolName> SymbolConstructor;
}




struct csr::SymbolName {
	SourceString name;
	std::optional<SourceString> alias;
};




class csr::ParsedSymbol {
// ------------------------------------[ Properties ] --------------------------------------- //
public:
	SymbolName name;
	std::optional<SymbolConstructor> constructor;
	
// ----------------------------------- [ Functions ] ---------------------------------------- //
public:
	inline void clear(){
		name.name.clear();
		name.alias.reset();
		constructor.reset();
	}
	
// ------------------------------------------------------------------------------------------ //
};




class csr::ParsedReduction {
// ------------------------------------[ Properties ] --------------------------------------- //
public:
	Location loc;
	std::vector<ParsedSymbol> left;
	std::vector<ParsedSymbol> right;
	SourceString code;
	
// ---------------------------------- [ Constructors ] -------------------------------------- //
public:
	ParsedReduction() : loc{-1} {};
	
// ----------------------------------- [ Functions ] ---------------------------------------- //
public:
	/**
	 * @brief Find apropriate names for indexed symbol aliases.
	 *        Symbols without a name must have an alias consisting of an integer
	 *         that can be parsed into an index. The name of the corresponding symbol from
	 *         the left side is copied into the indexing symbol on the right.
	 *        Named symbols with an alias are also checked for consistency between the left and the right side.
	 *        Example: A -> 1 | A -> A[1] | A[first] -> A[first].
	 * @throws ParserException on any alias inconsistency.
	 */
	void resolveSymbolAliases();
	
// ----------------------------------- [ Functions ] ---------------------------------------- //
public:
	static bool distinct(const std::vector<ParsedReduction>& v, int* out_first = nullptr, int* out_second = nullptr);
	static bool validateSize(const std::vector<ParsedReduction>& v, int* out_i);
	static std::vector<ParsedSymbol> createEnum(const std::vector<ParsedReduction>& reductions);
	
// ------------------------------------------------------------------------------------------ //
};
