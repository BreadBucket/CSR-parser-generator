#include "Generator.hpp"

using namespace std;
using namespace csr;


// ----------------------------------- [ Functions ] ---------------------------------------- //


const char* const header_1 =
	"#pragma once\n"
	"#include <stdint>\n"
	"\n"
	"typedef int32_t TokenID;\n"
	"\n"
	"TokenID csr_next();\n"
	"\n"
;


// ----------------------------------- [ Functions ] ---------------------------------------- //


void Generator::generate(){
	if (reductions == nullptr || code == nullptr){
		return;
	}
	
	nullStream.setstate(ios_base::badbit);
	if (this->cFile == nullptr)
		this->cFile = &nullStream;
	if (this->headerFile == nullptr)
		this->headerFile = &nullStream;
	
	
	
	*headerFile << header_1;
	
	
	
	if (this->cFile == &this->nullStream)
		this->cFile = nullptr;
	if (this->headerFile == &this->nullStream)
		this->headerFile = nullptr;
	
}


// ------------------------------------------------------------------------------------------ //