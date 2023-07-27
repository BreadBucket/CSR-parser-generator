#pragma once
#include <vector>
#include <memory>
#include <istream>
#include <stdexcept>

#include "ParsedReduction.hpp"
#include "Document.hpp"
#include "ParserException.hpp"


namespace csr {
	class Parser;
}


class csr::Parser {
public:												// DEBUG
	void printch(const char* color = "\e[96m");		// DEBUG
// ------------------------------------[ Properties ] --------------------------------------- //
public:
	int tabSize = 4;
	std::istream* in = nullptr;
	
private:
	SourceString trash;
	std::vector<Location> frames;
	std::unique_ptr<Document> doc;	// Temporary object for storing results
	
	
private:
	int buffSize  = 1024;	// Preffered buffer size
	int _buffSize = 0;		// Actual buffer size
	char* buff    = nullptr;
	
private:
	int n;		// Number of characters in buffer
	int i;		// Local index of current buffered character
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
	/**
	 * @throws ParserException for various parsing issues.
	 */
	Document* parse(std::istream& in);
	
// ----------------------------------- [ Functions ] ---------------------------------------- //
private:
	void parseSegment();
	void parseSegment_header(SourceString& out_directive, SourceString& out_type);
	void parseSegment_reductions(std::vector<ParsedReduction>& out_reductions);
	void parseSegment_code(std::string& out_s);
	
private:
	void parseReduction(ParsedReduction& out_reduction);
	void parseReduction_symbol(SymbolName& out);
	void parseReduction_symbol_alias(SourceString& out);
	void parseReduction_constructor(ParsedSymbol::Constructor& out);
	void parseReduction_inlineCode(SourceString& out_code);
	
private:
	/**
	 * @brief Parse C style preprocessor directives.
	 * @param s String buffer for appending parsed characters.
	 * @returns Parsed directive type or unknown.
	 * @throws ParserException when preprocessor directive does not start with '#'.
	 * @throws ParserException on unterminated string literals or comments.
	 */
	void parseMacro(std::string& s, SourceString& out_directive);
	
	/**
	 * @brief Parse identifier containing alphabetical letters, digits or underscore '_'.
	 * @param out_id Parsed ID replaces content within the string and sets its location.
	 * @returns Amount of characters parsed.
	 */
	int parseId(SourceString& out_id);
	
	/**
	 * @brief Parse string of digits.
	 * @param out_id Parsed integer replaces content within the string and sets its location.
	 * @returns Amount of characters parsed.
	 */
	int parseInt(SourceString& out_int);
	
// ----------------------------------- [ Functions ] ---------------------------------------- //
private:
	/**
	 * @brief Skip all white-space characters.
	 * @param s Where parsed white-space is appended.
	 * @param escapedNewline Newline must be escaped.
	 * @return Amount of characters parsed.
	 */
	int parseWhiteSpace(std::string& s, bool escapedNewline = false);
	
	/**
	 * @brief Parse C-style line comment (starts with "//" and extends to EOL) or block comment.
	 * @param s Where parsed comment is appended.
	 * @return Amount of parsed characters.
	 * @throws ParserException when comment does not start with "//" or ("/" + "*").
	 * @throws ParserException when comment does not end with "//" or ("*" + "/").
	 */
	int parseComment(std::string& s);
	
	/**
	 * @brief Skip all white-space characters.
	 * @param s Where parsed white-space is appended.
	 * @param escapedNewline Newline must be escaped.
	 * @return Amount of characters parsed.
	 */
	int parseWhiteSpaceAndComment(std::string& s, bool escapedNewline = false);
	
	/**
	 * @brief Skip all non-white-space characters excluding backslash '\'.
	 * @param s Where parsed characters are appended.
	 * @param includeEscaped Escaped white-space and '\' is considered as solid space.
	 * @return Amount of characters parsed.
	 */
	int parseSolidSpace(std::string& s, bool includeEscaped = false);
	
	/**
	 * @brief Parse C-style string literal (everything between two double quotes `"`)
	 *        or character literal (everything between two single quotes `'`).
	 * @param s Where parsed literal is appended.
	 * @return Amount of parsed characters.
	 * @throws ParserException when literal does not start with `"` or `'`.
	 * @throws ParserException when literal does not end with `"` or `'`.
	 */
	int parseStringLiteral(std::string& s);
	
// ----------------------------------- [ Functions ] ---------------------------------------- //
private:
	/**
	 * @return Current global index.
	 *         Global index maps to a position in a file.
	 */
	inline int gi(){
		return bi + i;
	}
	
	/**
	 * @brief Increment column index and local index by n.
	 * @param n Amount of characters to skip.
	 *          Negative values cause undefined behaviour.
	 */
	inline void inc(int n = 1){
		i += n;
		ci += n;
	}
	
	/**
	 * @brief Simulate newline:
	 *          increment row index,
	 *          increment local index,
	 *          reset column index.
	 */
	inline void nl(){
		ri++;
		i++;
		ci = 0;
	}
	
	/**
	 * @brief Simulate tab:
	 *          increment column index to next multiple of `tabSize`,
	 *          increment local index.
	 */
	inline void tab(){
		i++;
		ci += tabSize;
		ci -= ci % tabSize;
	}
	
	/**
	 * @brief  Get current character in buffer.
	 *         Index is not incremented.
	 *         Buffer is automatically filled with `buffSize` characters if needed.
	 * @return Current character, pointed to by local index `i`, or '\0' if EOF.
	 */
	inline char ch(){
		if (i >= n) [[unlikely]]
			fillBuffer(buffSize, true);
		return buff[i];
	}
	
	/**
	 * @return Current carret location using global index, row index and column index.
	 */
	inline csr::Location getLoc(){
		return {gi(), ri, ci};
	}
	
// ----------------------------------- [ Functions ] ---------------------------------------- //
private:
	/**
	 * @brief Push carret location to the stack.
	 *        Buffer resizing will not erase characters from this point on.
	 */
	const Location& push();
	
	/**
	 * @brief Return to previously pushed carret location.
	 * @param count Amount of location frames to pop. -1 to pop all frames.
	 * @param applyLocation Set carret location to the popped location.
	 */
	void pop(int count = 1, bool applyLocation = true);
	
	/**
	 * @brief Copy string from buffer, if start and end are within range of existing buffer: str += buff[start,end).
	 * @param start   Starting position.
	 * @param end     End position.
	 * @param out_str Where substring is appended.
	 * @return True if extraction was successful.
	 */
	bool extractString(const Location& start, const Location& end, std::string& out_str);
	
// ----------------------------------- [ Functions ] ---------------------------------------- //
private:
	/**
	 * @brief Try to match buff[i..] with s[0..].
	 * @param s String to match.
	 * @param move Increment i by |s| if successful.
	 * @param out_match Optionally append matched string as out_match += s.
	 * @return True if matched.
	 */
	bool match(const char* s, bool move = false, std::string* out_match = nullptr);
	
// ----------------------------------- [ Functions ] ---------------------------------------- //
private:
	/**
	 * @brief Performs cleanup: resets buffer and all indexes.
	 */
	void reset();
	
	/** 
	 * @brief Ensure buffer has at least `count` more characters after `buff[i]`.
	 *        Characters before `frame[0]` (using `push()`) or `buff[i]` are discarded.
	 *        Index `i`, char count `n`, and buffer pointer might change.
	 * @param count Min amount of characters to add to the buffer.
	 * @param fill  Fill buffer to the end if possible.
	 * @return Amount of new characters in buffer.
	 */
	int fillBuffer(int count, bool fill = true);
	
	/**
	 * @brief Wrapper for `fillBuffer`.
	 * @param count Amount of characters requested to be available after `buff[i]`.
	 * @return True if buffer has sufficient amount of characters.
	 */
	bool lookAhead(int count){
		fillBuffer(count);
		return count <= (n - i);
	}
	
// ----------------------------------- [ Operators ] ---------------------------------------- //
public:
	Parser& operator=(Parser&) = delete;
	Parser& operator=(Parser&&) = delete;
	
// ------------------------------------------------------------------------------------------ //
};
