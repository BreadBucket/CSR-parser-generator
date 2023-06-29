#pragma once
#include <cstdbool>
#include <vector>
#include <istream>

#include "csg_rule.hpp"
#include "interval.hpp"


namespace csg {
	struct Location;
	struct Source;
	class Parser;
}



struct csg::Location {
	int i;
	int row;
	int col;
};



struct csg::Source {
	interval<csg::Location> loc;
	std::string str;
};




class csg::Parser {
// ------------------------------------[ Properties ] --------------------------------------- //
private:
	std::istream* in;
	int buffSize;
	char* buff;
	
private:
	int n;		// Number of characters in buffer
	int i;		// Local index of current buffer character
	int bi;		// Global index of first character in buffer
	int ri;		// Current global row index
	int ci0;	// Current global index of character in column 0 of current row
	bool eof;	// EOF reached
	
// ---------------------------------- [ Constructors ] -------------------------------------- //
public:
	Parser() : buffSize{1024} {}
	Parser(int buff) : buffSize{buff} {}
	
	Parser(Parser&) = delete;
	Parser(Parser&&) = delete;
	
	~Parser(){
		delete buff;
	}
	
// ----------------------------------- [ Functions ] ---------------------------------------- //
public:
	void parse(std::istream& in);
	
// ----------------------------------- [ Functions ] ---------------------------------------- //
private:
	void skipWhiteSpace();
	void parseRule();
	bool parseLeftSymbol(Source& rec);
	bool parseRightSymbol(Source& rec);
	
// ----------------------------------- [ Functions ] ---------------------------------------- //
private:
	void startSrc(Source& rec);
	void endSrc(Source& rec);
	
// ----------------------------------- [ Functions ] ---------------------------------------- //
private:
	/**
	 * @return Current global index.
	 */
	inline int gi(){
		return bi + i;
	}
	
	/**
	 * @return Current column index.
	 */
	inline int ci(){
		return gi() - ci0;
	}
	
	/**
	 * @brief  Get current character in buffer, index is not incremented.
	 *         Buffer is automatically refilled if needed.
	 * @return Current character or '\0' if EOF.
	 */ 
	inline char ch(){
		if (buff[i] == 0) [[unlikely]]
			fillBuffer();
		return buff[i];
	}
	
	/**
	 * @returns Current carret position.
	 */
	inline csg::Location getLoc(){
		return {gi(), ri, ci()};
	}
	
// ----------------------------------- [ Functions ] ---------------------------------------- //
private:
	/**
	 * @brief  Try to match buff[i..i+n] with s[0..n]. Increment i by n if successful.
	 * @return True if matched.
	 */
	bool match(const char* s, int n);
	
// ----------------------------------- [ Functions ] ---------------------------------------- //
private:
	/**
	 * @brief Resets buffer and all indexes.
	 */
	void reset();
	
	/** 
	 * @brief  Fills buffer with new data.
	 *         Resets index i, stores new char count to n, recalculates global index.
	 * @return True if refill was successful.
	 */
	bool fillBuffer();
	
	/**
	 * @brief Increment row index.
	 *        Set column index to global index of first character in the new row.
	 */
	void nl();
	
	/**
	 * @brief  Ensure buffer has at least n characters.
	 * @return New amount of characters in buffer. Returned value can be less than n if EOF is reached.
	 */
	int lookAhead(int n);
	
// ----------------------------------- [ Operators ] ---------------------------------------- //
public:
	Parser& operator=(Parser&) = delete;
	Parser& operator=(Parser&&) = delete;
	
// ------------------------------------------------------------------------------------------ //
};
