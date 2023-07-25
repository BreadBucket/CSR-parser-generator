#include <ostream>

#include "ANSI.h"
#include "CLI.hpp"


// ----------------------------------- [ Functions ] ---------------------------------------- //


template <typename ...T>
inline void warn(const char* format, T... args){
	if (CLI::ansi){
		printf(ANSI_BOLD "%s: " ANSI_YELLOW "warning: " ANSI_RESET, CLI::programName.c_str());
	} else {
		printf("%s: warning: ", CLI::programName.c_str());
	}
	printf(format, args...);
}


// ------------------------------------------------------------------------------------------ //