#pragma once
#include <vector>
#include <string>


namespace csg {
class Rule {
// ------------------------------------[ Properties ] --------------------------------------- //
public:
	std::vector<std::string> left;
	std::vector<std::string> right;
	
// ---------------------------------- [ Constructors ] -------------------------------------- //
public:
	Rule() = default;
	
// ------------------------------------------------------------------------------------------ //
};
}