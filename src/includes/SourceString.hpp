#pragma once
#include <string>


namespace csg {
	struct Location;
	class SourceString;
}



struct csg::Location {
// -------------------------------- //
public:
	int i;		// Index of character
	int row;	// Row index of character
	int col;	// Column index of character
	
// -------------------------------- //
public:
	inline bool valid(){
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



class csg::SourceString : public std::string {
// ------------------------------------[ Properties ] --------------------------------------- //
public:
	Location start;
	Location end;
	
// ---------------------------------- [ Constructors ] -------------------------------------- //
public:
	// LocatableString 
	
// ----------------------------------- [ Functions ] ---------------------------------------- //
public:
	inline void clear(){
		start.i = -1;
		std::string::clear();
	}
	
	inline bool sourceable(){
		return start.i >= 0;
	}
	
// ------------------------------------------------------------------------------------------ //
};