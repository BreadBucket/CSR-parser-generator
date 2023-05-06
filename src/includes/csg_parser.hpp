#pragma once
#include <cstdbool>
#include <vector>
#include <istream>

#include "csg_rule.hpp"
#include "interval.hpp"


namespace csg {
	class Parser;
	struct RuleSource;
}


class csg::Parser {
// ------------------------------------[ Properties ] --------------------------------------- //
private:
	std::istream* in;
	
	int buffSize;
	char* buff;
	
	int i;		// Local index of current buffer character
	int n;		// Number of characters in buffer
	int bi;		// Global index of first character in buffer
	int ri;		// Current global row index
	int ci;		// Global index of first character in current row
	bool eof;	// EOF reached
	
// ------------------------------------[ Properties ] --------------------------------------- //
private:
	std::vector<RuleSource> rules;
	
// ---------------------------------- [ Constructors ] -------------------------------------- //
public:
	Parser() : buffSize{1024} {}
	Parser(int buff) : buffSize{buff} {}
	
	Parser(Parser&) = delete;
	
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
	bool parseLeftToken(RuleSource& rec);
	bool parseRightToken(RuleSource& rec);
	
// ----------------------------------- [ Functions ] ---------------------------------------- //
private:
	void startSrc(RuleSource& rec);
	void endSrc(RuleSource& rec);
	void extractStr(RuleSource& rec, const std::string& str);
	
// ----------------------------------- [ Functions ] ---------------------------------------- //
private:
	/**
	 * @return Current global index.
	 */
	inline int gi(){
		return bi + i;
	}
	
	/**
	 * @brief Increment row index, set column index to global index of first character in the new row.
	 */
	inline void nl(){
		ri++;
		ci = gi() + 1;
	}
	
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
	 * @brief  Ensure buffer has at least n characters.
	 * @return New amount of characters in buffer. Returned value can be less than n if EOF is reached.
	 */
	int lookAhead(int n);
	
	/**
	 * @brief  Try to match buff[i..i+n] with s[0..n]. Increment i by n if successful.
	 * @return True if matched.
	 */
	bool match(const char* s, int n);
	
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
	
// ----------------------------------- [ Operators ] ---------------------------------------- //
public:
	Parser& operator=(Parser&) = delete;
	
// ------------------------------------------------------------------------------------------ //
};




struct csg::RuleSource {
	interval<int> pos;
	interval<int> row;
	interval<int> col;
	std::string str;
	Rule* rule;
};

