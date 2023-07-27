#pragma once
#include <vector>
#include <variant>
#include <memory>

#include "Symbol.hpp"


namespace csr {
	class Reduction;
	typedef int ReductionID;
};


class csr::Reduction {
// ---------------------------------- [ Structures ] ---------------------------------------- //
public:
	typedef int SymbolCopy;		// Index of symbol on the left side.
	struct SymbolConstructor;	// List of symbols as arguments of a new symbol object.
	typedef std::variant<SymbolCopy,SymbolConstructor> RightSymbol;
	
public:
	struct SymbolConstructor {
		std::shared_ptr<Symbol> symbol;
		std::vector<RightSymbol> args;
	};
	
// ------------------------------------[ Properties ] --------------------------------------- //
public:
	ReductionID id;
	std::vector<std::shared_ptr<Symbol>> left;
	std::vector<RightSymbol> right;
	
// ------------------------------------------------------------------------------------------ //
};