#pragma once
#include <vector>
#include <string>

#include "Symbol.hpp"


namespace csg {
class Reduction {
// ------------------------------------[ Properties ] --------------------------------------- //
public:
	std::vector<Symbol> left;
	std::vector<Symbol> right;
	
// ---------------------------------- [ Constructors ] -------------------------------------- //
public:
	Reduction() = default;
	
// ------------------------------------------------------------------------------------------ //
};
}