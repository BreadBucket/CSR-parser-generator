#pragma once
#include <vector>
#include <string>

#include "Symbol.hpp"


namespace csg {
class Rule {
// ------------------------------------[ Properties ] --------------------------------------- //
public:
	std::vector<Symbol> left;
	std::vector<Symbol> right;
	
// ---------------------------------- [ Constructors ] -------------------------------------- //
public:
	Rule() = default;
	
// ------------------------------------------------------------------------------------------ //
};
}