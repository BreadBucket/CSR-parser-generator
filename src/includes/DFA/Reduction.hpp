#pragma once
#include <vector>
#include <memory>

#include "Symbol.hpp"


namespace csr {
	class Reduction;
	typedef int ReductionID;
};


class csr::Reduction {
// ------------------------------------[ Properties ] --------------------------------------- //
public:
	ReductionID id;
	std::vector<std::shared_ptr<Symbol>> left;
	std::vector<std::shared_ptr<Symbol>> right;
	
// ------------------------------------------------------------------------------------------ //
};