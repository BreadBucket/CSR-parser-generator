#pragma once

#include <vector>
#include <iostream>
#include <fstream>

#include "Reduction.hpp"
#include "SourceString.hpp"



namespace CSR {
	class Generator;
}



class CSR::Generator {
// ------------------------------------[ Properties ] --------------------------------------- //
public:
	const std::vector<SourceString>* code;
	const std::vector<Reduction>* reductions;
	
	std::ostream* cFile;
	std::ostream* headerFile;
	
// ------------------------------------[ Variables ] ---------------------------------------- //
private:
	std::ofstream nullStream;
	
// ----------------------------------- [ Functions ] ---------------------------------------- //
public:
	void generate();
	
// ------------------------------------------------------------------------------------------ //
};
