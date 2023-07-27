#include <ostream>

#include "ANSI.h"
#include "CLI.hpp"


// ----------------------------------- [ Functions ] ---------------------------------------- //


template <typename ...T>
inline void warn(const char* format, T... args){
	if (CLI::ansi){
		fprintf(stderr, ANSI_BOLD "%s: " ANSI_YELLOW "warning: " ANSI_RESET, CLI::programName.c_str());
	} else {
		fprintf(stderr, "%s: warning: ", CLI::programName.c_str());
	}
	fprintf(stderr, format, args...);
}


// ------------------------------------------------------------------------------------------ //