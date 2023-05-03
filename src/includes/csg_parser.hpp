#pragma once
#include <cstdbool>
#include <istream>


namespace csg {
class Parser {
// ------------------------------------[ Properties ] --------------------------------------- //
private:
	std::istream* in;
	
	int buffSize;
	char* buff;
	char* prevBuff;
	
	int i;
	int n;
	int gi;
	int li;
	bool eof;
	
// ---------------------------------- [ Constructors ] -------------------------------------- //
public:
	Parser(int buff = 1024) : buffSize{buff} {}
	
	Parser(Parser&) = delete;
	
	~Parser(){
		delete buff;
		delete prevBuff;
	}
	
// ----------------------------------- [ Functions ] ---------------------------------------- //
public:
	void parse(std::istream& in);
	
// ----------------------------------- [ Functions ] ---------------------------------------- //
private:
	void skipWhiteSpace();
	
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
	 * @brief  Get next character in buffer, index is not incremented.
	 *         Buffer is automatically refilled if needed.
	 * @return Next character or '\0' if EOF.
	 */ 
	inline char next();
	
// ----------------------------------- [ Operators ] ---------------------------------------- //
public:
	Parser& operator=(Parser&) = delete;
	
// ------------------------------------------------------------------------------------------ //
};
}




// ----------------------------------- [ Functions ] ---------------------------------------- //


char csg::Parser::next(){
	if (buff[i] == 0)
		fillBuffer();
	return buff[i];
}


// ------------------------------------------------------------------------------------------ //