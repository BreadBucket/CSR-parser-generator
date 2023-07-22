#pragma once

#include <vector>
#include <iostream>
#include <fstream>

#include "ParsedReduction.hpp"
#include "Document.hpp"



namespace csr {
	class Generator;
}



class csr::Generator {
// ------------------------------------[ Properties ] --------------------------------------- //
public:
	const std::vector<SourceString>* code;
	const std::vector<ParsedReduction>* reductions;
	
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
