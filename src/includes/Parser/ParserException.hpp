#pragma once
#include "CSRException.hpp"
#include "SourceString.hpp"


namespace csr {
class ParserException : public CSRException {
// ------------------------------------[ Properties ] --------------------------------------- //
public:
	Location loc;
	
// ---------------------------------- [ Constructors ] -------------------------------------- //
public:
	ParserException(const char* msg) : CSRException(msg), loc{-1} {}
	ParserException(const Location& loc, const char* msg) : CSRException(msg), loc{loc} {}
	ParserException(Location&& loc, const char* msg) : CSRException(msg), loc{loc} {}
	
// ------------------------------------------------------------------------------------------ //
};
}
