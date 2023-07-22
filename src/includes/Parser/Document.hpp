#pragma once
#include <vector>

#include "SourceString.hpp"
#include "ParsedReduction.hpp"


namespace csr {
class Document {
// ------------------------------------[ Properties ] --------------------------------------- //
public:
	std::string name;
	std::vector<ParsedReduction> reductions;
	std::vector<SourceString> code;
	
// ------------------------------------------------------------------------------------------ //	
};
}