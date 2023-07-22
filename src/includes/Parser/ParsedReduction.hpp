#pragma once
#include <string>
#include <vector>

#include "SourceString.hpp"
#include "CSRException.hpp"


namespace csr {
	class ParsedSymbol;
	class ParsedReduction;
}




class csr::ParsedSymbol {
// ------------------------------------[ Properties ] --------------------------------------- //
public:
	SourceString name;	// The name of the symbol
	SourceString atr;	// Optional identifier
	
// ----------------------------------- [ Functions ] ---------------------------------------- //
public:
	inline void clear(){
		name.clear();
		atr.clear();
	}
	
// ------------------------------------------------------------------------------------------ //
};




class csr::ParsedReduction {
// ------------------------------------[ Properties ] --------------------------------------- //
public:
	Location loc;
	std::vector<ParsedSymbol> left;
	std::vector<ParsedSymbol> right;
	SourceString code;
	
// ---------------------------------- [ Constructors ] -------------------------------------- //
public:
	ParsedReduction() : loc{-1} {};
	
// ----------------------------------- [ Functions ] ---------------------------------------- //
public:
	static bool distinct(const std::vector<ParsedReduction>& v, int* out_first = nullptr, int* out_second = nullptr);
	static bool validateSize(const std::vector<ParsedReduction>& v, int* out_i);
	static std::vector<ParsedSymbol> createEnum(const std::vector<ParsedReduction>& reductions);
	
// ------------------------------------------------------------------------------------------ //
};
