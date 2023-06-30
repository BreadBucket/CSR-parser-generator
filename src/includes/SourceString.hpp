#pragma once
#include <string>


namespace csg {
	struct Location;
	class SourceString;
}



struct csg::Location {
	int i;		// Global index of character
	int row;	// Row index of character
	int col;	// Column index of character
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
	inline bool sourceable(){
		return start.i >= 0;
	}
	
// ------------------------------------------------------------------------------------------ //
};