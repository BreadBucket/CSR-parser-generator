#pragma once
#include <string>
#include <vector>

#include "SourceString.hpp"
#include "ParsedSymbol.hpp"


namespace csr {
class ParsedReduction {
// ------------------------------------[ Properties ] --------------------------------------- //
public:
	Location loc = {-1};
	std::vector<ParsedSymbol> left;
	std::vector<ParsedSymbol> right;
	SourceString code;
	
// ----------------------------------- [ Functions ] ---------------------------------------- //
public:
	static bool distinct(const std::vector<ParsedReduction>& v, int* out_first = nullptr, int* out_second = nullptr);
	static bool validateSize(const std::vector<ParsedReduction>& v, int* out_i);
	
// ------------------------------------------------------------------------------------------ //
};
}