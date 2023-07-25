#pragma once
#include <string>


namespace csr {
	struct Location;
	class SourceString;
}



struct csr::Location {
// -------------------------------- //
public:
	int i;		// Index of character
	int row;	// Row index of character
	int col;	// Column index of character
	
// -------------------------------- //
public:
	inline bool valid() const {
		return i >= 0;
	}
	
// -------------------------------- //
public:
	Location operator+(int n) const {
		return {i + n, row, col + n};
	}
	
	Location& operator+=(int n){
		i += n;
		col += n;
		return *this;
	}
	
	Location operator-(int n) const {
		return {i - n, row, col - n};
	}
	
	Location& operator-=(int n){
		i -= n;
		col -= n;
		return *this;
	}
	
// -------------------------------- //
};



class csr::SourceString : public std::string {
// ------------------------------------[ Properties ] --------------------------------------- //
public:
	Location start;
	Location end;
	
// ---------------------------------- [ Constructors ] -------------------------------------- //
public:
	SourceString() : std::string(), start{-1}, end{-1} {};
	
	SourceString(const Location& start) : std::string(), start{start}, end{-1} {};
	SourceString(Location&& start)      : std::string(), start{start}, end{-1} {};
	
	SourceString(const std::string& s) : std::string(s), start{-1}, end{-1} {};
	SourceString(std::string&& s)      : std::string(s), start{-1}, end{-1} {};
	
// ----------------------------------- [ Functions ] ---------------------------------------- //
public:
	inline SourceString& clear(){
		start.i = -1;
		end.i = -1;
		std::string::clear();
		return *this;
	}
	
	inline bool valid(){
		return start.i >= 0;
	}
	
// ------------------------------------------------------------------------------------------ //
};