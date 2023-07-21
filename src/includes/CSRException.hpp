#pragma once
#include <stdexcept>


namespace CSR {
class CSRException : public std::runtime_error {
// ---------------------------------- [ Constructors ] -------------------------------------- //
public:
	using runtime_error::runtime_error;
	
// ------------------------------------------------------------------------------------------ //
};
}