#pragma once
#include <string>
#include <ostream>

#include "ANSI.h"
#include "CLI.hpp"


// ----------------------------------- [ Functions ] ---------------------------------------- //


#ifdef CSR_LOCATION
inline void errLoc(const std::string& file, const csr::Location& loc){
	if (loc.row < 0)
		fprintf(stderr, "%s", file.c_str());
	else if (loc.col < 0)
		fprintf(stderr, "%s:%d", file.c_str(), loc.row+1);
	else
		fprintf(stderr, "%s:%d:%d", file.c_str(), loc.row+1, loc.col+1);
}
#endif


// ----------------------------------- [ Functions ] ---------------------------------------- //


template<typename ...T>
inline void err(const std::string& file, const char* format, T... args){
	if (CLI::ansi){
		fprintf(stderr, ANSI_BOLD "%s: " ANSI_RED "error" ANSI_RESET ": ", file.c_str());
		fprintf(stderr, format, args...);
	} else {
		fprintf(stderr, "%s: error: ", file.c_str());
		fprintf(stderr, format, args...);
	}
}


template<typename ...T>
inline void err(const std::string& file, const csr::Location& loc, const char* format, T... args){
	if (CLI::ansi){
		fprintf(stderr, ANSI_BOLD);
		errLoc(file, loc);
		fprintf(stderr, ": " ANSI_RED "error" ANSI_RESET ": ");
	} else {
		errLoc(file, loc);
		fprintf(stderr, ": error: ");
	}
	fprintf(stderr, format, args...);
}


template<typename ...T>
inline void err(const char* format, T... args){
	err(CLI::programName, format, args...);
}


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