#pragma once
#include <cstdbool>
#include <vector>
#include <istream>
#include <stdexcept>

#include "Symbol.hpp"
#include "Reduction.hpp"


namespace csg {
	class Parser;
	class ParserException;
}



// DEBUG
std::string locStr(const csg::Location& loc);


class csg::Parser {
public:					// DEBUG
	void printch();		// DEBUG
// ------------------------------------[ Properties ] --------------------------------------- //
public:
	int tabSize = 4;
	
private:
	std::istream* in	= nullptr;
	int buffSize		= 1024;		// Preffered buffer size
	int _buffSize		= 0;		// Actual buffer size
	char* buff			= nullptr;
	std::vector<Location> frames;
	
private:
	int n;		// Number of characters in buffer
	int i;		// Local index of current buffer character
	int bi;		// Global index of character at buff[0]
	int ri;		// Current global row index
	int ci;		// Current global column index
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
	
	void parseReductionSegment(SourceString& out_s);
	void parseCodeSegment(SourceString& out_s);
	void skipMacro(SourceString& out_s);
	
// ----------------------------------- [ Functions ] ---------------------------------------- //
private:
	void parseMacro();
	
private:
	/**
	 * @throws csg::ParserException on syntax error.
	 */
	void parseReduction(Reduction& out_reduction);
	void parseReductionInlineCode(SourceString& out_code);
	
	void parseSymbol(Symbol& out_symbol);
	void parseSymbolAttributes(Symbol& out_symbol);
	void parseId(SourceString& out_symbol);
	
// ----------------------------------- [ Functions ] ---------------------------------------- //
private:
	void skipWhiteSpace();
	
	/**
	 * @brief Skip all white-space characters except newline.
	 * @param escapeable Determines if escaped newlines are permisable.
	 */
	void skipSpace(bool escapeable = false);
	
// ----------------------------------- [ Functions ] ---------------------------------------- //
private:
	/**
	 * @return Current global index.
	 */
	inline int gi(){
		return bi + i;
	}
	
	/**
	 * @brief Increment column index and local index by n.
	 */
	inline void inc(int n = 1){
		i += n;
		ci += n;
	}
	
	/**
	 * @brief Increment row index.
	 *        Increment local index by 1.
	 *        Set column index to 0.
	 */
	inline void nl(){
		ri++;
		i++;
		ci = 0;
	}
	
	/**
	 * @brief Increment column index by tabSize.
	 *        Increment local index by 1.
	 */
	inline void tab(){
		i++;
		ci += tabSize;
		ci -= ci % tabSize;
	}
	
	/**
	 * @brief  Get current character in buffer, index is not incremented.
	 *         Buffer is automatically refilled if needed.
	 * @return Current character or '\0' if EOF.
	 */
	inline char ch(){
		if (i >= n) [[unlikely]]
			fillBuffer(i - n + 1, true);
		return buff[i];
	}
	
	/**
	 * @return Current carret position.
	 */
	inline csg::Location getLoc(){
		return {gi(), ri, ci};
	}
	
// ----------------------------------- [ Functions ] ---------------------------------------- //
private:
	/**
	 * @brief  Try to match buff[i..i+n] with s[0..n].
	 *         Increment i by n if successful.
	 * @return True if matched.
	 */
	bool match(const char* s, bool move = true);
	
// ----------------------------------- [ Functions ] ---------------------------------------- //
private:
	/**
	 * @brief Push carret location to the stack.
	 *        Buffer resizing will not erase characters from this point on.
	 */
	void push();
	
	/**
	 * @brief Return to previously pushed carret location.
	 * @param applyLocation Set carret location to the popped location.
	 * @param count Amount of location frames to pop. -1 to pop all frames.
	 */
	void pop(bool applyLocation = true, int count = 1);
	
// ----------------------------------- [ Functions ] ---------------------------------------- //
private:
	/**
	 * @brief Resets buffer and all indexes.
	 */
	void reset();
	
	/** 
	 * @brief Fills buffer with new data.
	 *        Characters before frame[0] are discarded if it exists, otherwise all characters before index i.
	 *        Resets index i, sets n to new char count, recalculates global index.
	 * @param count Min amount of characters to add to the buffer.
	 * @param fill  Add more characters if there is space available.
	 * @return Amount of new characters in buffer.
	 */
	int fillBuffer(int count, bool fill = true);
	
	/**
	 * @brief  Ensure buffer has at least n more characters i.e. buff[i..i+n].
	 * @return Available characters in buffer. Returned value can be less than n if EOF is reached.
	 */
	int lookAhead(int n);
	
// ----------------------------------- [ Operators ] ---------------------------------------- //
public:
	Parser& operator=(Parser&) = delete;
	Parser& operator=(Parser&&) = delete;
	
// ------------------------------------------------------------------------------------------ //
};




class csg::ParserException : public std::runtime_error {
// ------------------------------------[ Properties ] --------------------------------------- //
public:
	Location loc;
	
// ---------------------------------- [ Constructors ] -------------------------------------- //
public:
	ParserException(const char* msg) : runtime_error(msg), loc{-1} {}
	ParserException(Location& loc, const char* msg) : runtime_error(msg), loc{loc} {}
	ParserException(Location&& loc, const char* msg) : runtime_error(msg), loc{loc} {}
	
// ------------------------------------------------------------------------------------------ //
};