#pragma once
#include <ostream>


namespace csr {
struct Tab {
// ------------------------------------[ Variables ] ---------------------------------------- //
public:
	int n = 0;		// Amount of tab characters
	char c = '\t';	// Tab character
	int w = 1;		// Character width (amount of character for one tab)
	
// ---------------------------------- [ Constructors ] -------------------------------------- //
public:
	Tab() = default;
	Tab(int n) : n{n} {}
	Tab(char c, int w) : c{c}, w{w} {}
	Tab(int n, char c, int w) : n{n}, c{c}, w{w} {}
	
// ----------------------------------- [ Functions ] ---------------------------------------- //
public:
	inline void convert(char c, int w){
		this->c = c;
		this->w = w;
	}
	
// ----------------------------------- [ Operators ] ---------------------------------------- //
public:
	inline Tab operator +(int n) const {
		return {this->n + n, this->c, this->w};
	}
	
	inline Tab operator -(int n) const {
		return {this->n - n, this->c, this->w};
	}
	
	inline Tab& operator +=(int n){
		this->n += n;
		return *this;
	}
	
	inline Tab& operator -=(int n){
		this->n -= n;
		return *this;
	}
	
	inline Tab& operator ++(){
		this->n++;
		return *this;
	}
	
	inline Tab& operator --(){
		this->n--;
		return *this;
	}
	
// ------------------------------------------------------------------------------------------ //	
};
}


inline std::ostream& operator <<(std::ostream& stream, const csr::Tab& tab){
	for (int i = tab.n * tab.w ; i > 0 ; i--)
		stream << tab.c;
	return stream;
}

