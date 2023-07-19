#pragma once
#include <vector>
#include <string>

#include "Symbol.hpp"
#include "SourceString.hpp"


namespace CSR {
class Reduction {
// ------------------------------------[ Properties ] --------------------------------------- //
public:
	std::vector<Symbol> left;
	std::vector<Symbol> right;
	SourceString code;
	
// ---------------------------------- [ Constructors ] -------------------------------------- //
public:
	Reduction() = default;
	
// ------------------------------------------------------------------------------------------ //
};
}