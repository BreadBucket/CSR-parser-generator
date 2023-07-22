#pragma once
#include <vector>

#include "Symbol.hpp"


namespace csr {
	class Reduction;
	typedef int ReductionID;
};


class csr::Reduction {
// ------------------------------------[ Properties ] --------------------------------------- //
public:
	ReductionID id;
	std::vector<SymbolID> left;
	std::vector<SymbolID> right;
	
// ------------------------------------------------------------------------------------------ //
};