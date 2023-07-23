#pragma once
#include <string>
#include <vector>

#include "SourceString.hpp"
#include "ParsedSymbol.hpp"
#include "ParserException.hpp"


namespace csr {
class ParsedReduction {
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
	 * @brief Find symbol index of an alias.
	 * @param alias Alias to search.
	 * @return int Index of the symbol with the alias. -1 if the symbol was not found.
	 */
	int findAlias(const std::string& alias) const;
	
	/**
	 * @brief Create implicit aliases. Copy names from left to right according to aliases.
	 *        Verify alias correctness (check for duplicates etc.).
	 * @throws ParserException on any alias inconsistency.
	 */
	void resolveSymbolAliases();
	
// ----------------------------------- [ Functions ] ---------------------------------------- //
public:
	static bool distinct(const std::vector<ParsedReduction>& v, int* out_first = nullptr, int* out_second = nullptr);
	static bool validateSize(const std::vector<ParsedReduction>& v, int* out_i);
	
// ------------------------------------------------------------------------------------------ //
};
}