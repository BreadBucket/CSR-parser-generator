#pragma once
#include <vector>

#include "SourceString.hpp"
#include "Reduction.hpp"


namespace CSR {
class Document {
// ------------------------------------[ Properties ] --------------------------------------- //
public:
	std::string name;
	std::vector<Reduction> reductions;
	std::vector<SourceString> code;
	
// ------------------------------------------------------------------------------------------ //	
};
}
