#pragma once
#include <string>
#include <vector>
#include <memory>

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
	static bool distinct(const std::vector<std::shared_ptr<ParsedReduction>>& v, int* out_first = nullptr, int* out_second = nullptr);
	static bool validateLength(const std::vector<std::shared_ptr<ParsedReduction>>& v, int* out_i);
	
// ------------------------------------------------------------------------------------------ //
};
}