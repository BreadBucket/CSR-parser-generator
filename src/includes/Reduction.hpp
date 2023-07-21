#pragma once
#include <string>
#include <vector>

#include "Symbol.hpp"
#include "SourceString.hpp"
#include "CSRException.hpp"


namespace CSR {
	class Reduction;
	class DuplicateReduction;
}




class CSR::Reduction {
// ------------------------------------[ Properties ] --------------------------------------- //
public:
	Location loc;
	std::vector<Symbol> left;
	std::vector<Symbol> right;
	SourceString code;
	
// ---------------------------------- [ Constructors ] -------------------------------------- //
public:
	Reduction() : loc{-1} {};
	
// ----------------------------------- [ Functions ] ---------------------------------------- //
public:
	static bool distinct(const std::vector<Reduction>& v, int* out_first = nullptr, int* out_second = nullptr);
	static bool validateSize(const std::vector<Reduction>& v, int* out_i);
	static std::vector<Symbol> createEnum(const std::vector<Reduction>& reductions);
	
// ------------------------------------------------------------------------------------------ //
};




class CSR::DuplicateReduction : public CSR::CSRException {
// ------------------------------------[ Properties ] --------------------------------------- //
public:
	int i = -1;
	int prev_i = -1;
	
// ---------------------------------- [ Constructors ] -------------------------------------- //
public:
	using CSRException::CSRException;
	
	DuplicateReduction(int i, int prev_i, const char* msg) : CSRException(msg), i{i}, prev_i{prev_i} {}
	
// ------------------------------------------------------------------------------------------ //
};
