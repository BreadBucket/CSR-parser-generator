#include "Parser.hpp"

#include <cstring>
#include <iostream>
#include "util/ANSI.h"
#include "util/utils.hpp"

using namespace std;
using namespace CSR;


// ----------------------------------- [ Functions ] ---------------------------------------- //


// DEBUG
#define PATH 			"test/test.csg"
#define CSTR(color)		"{" color "%s" ANSI_RESET "}"
#define CSTRNL(color)	CSTR(color) "\n"


// #define PRINTF(...)	printf(__VA_ARGS__)
#define PRINTF(...)


// DEBUG
void printSrc(const SourceString& src){
	if (!src.empty()){
		printf("[%d,%d): " PATH ":", src.start.i, src.end.i);
		if (src.start.valid())
			printf("%d:%d", src.start.row+1, src.start.col+1);
		if (src.end.valid())
			printf("-%d:%d", src.end.row+1, src.end.col+1);
		printf("  ");
		
		printf("\"" ANSI_GREEN "%s" ANSI_RESET "\"", src.c_str());
	} else{
		printf("null");
	}
	printf("\n");
}


// DEBUG
char* locStr(const Location& loc){
	char* s = new char[100];
	snprintf(s, 100, "[%d]: " PATH ":%d:%d", loc.i, loc.row+1, loc.col+1);
	return s;
}


//DEBUG
void Parser::printch(const char* color){
	char c = ch();
	printf(color);
	printf("[%d/%d]", i, n);
	if (c == '\n')
		printf("%s -- '\\n'", locStr(getLoc()));
	else if (c == '\t')
		printf("%s -- '\\t'", locStr(getLoc()));
	else if (c == '\0')
		printf("%s -- '\\0'", locStr(getLoc()));
	else
		printf("%s -- '%c'", locStr(getLoc()), c);
	printf(ANSI_RESET "\n");
}


// DEBUG
void printreduction(const Reduction& r){
	if (r.left.size() > 0){
		for (int i = 0 ; i < r.left.size() ; i++){
			printSrc(r.left[i].name);
			if (!r.left[i].id.empty())
				printSrc(r.left[i].id);
		}
	} else {
		printf("null\n");
	}
	
	printf("->\n");
	
	if (r.right.size() > 0){
		for (int i = 0 ; i < r.right.size() ; i++){
			printSrc(r.right[i].name);
			if (!r.right[i].id.empty())
				printSrc(r.right[i].id);
		}
	} else {
		printf("null\n");
	}
	
	if (!r.code.empty())
		printSrc(r.code);
	
}


// DEBUG
char* strstr(const string& s){
	char* str = new char[256];
	snprintf(str, 256, "{" ANSI_GREEN "%s" ANSI_RESET "}", s.c_str());
	return str;
}


// ------------------------------------- [ Macros ] ----------------------------------------- //


#define DIR_REDUCT	"CSR"
#define DIR_CODE	"CSR_CODE"


#define LOOKAHEAD_IS_COMMENT(c)	(			\
	(c) == '/' && lookAhead(2) &&			\
	(buff[i+1] == '/' || buff[i+1] == '*')	\
)


// ----------------------------------- [ Functions ] ---------------------------------------- //


inline bool isIdFirstChar(char c){
	return BETWEEN(c, 'A', 'Z');
}


inline bool isIdChar(char c){
	return (
		BETWEEN(c, 'a', 'z') ||
		BETWEEN(c, 'A', 'Z') ||
		BETWEEN(c, '0', '9') ||
		(c == '_')
	);
}


inline bool isMacroFirstChar(char c){
	return (
		BETWEEN(c, 'a', 'z') ||
		BETWEEN(c, 'A', 'Z') ||
		(c == '_')
	);
}


inline bool isMacroChar(char c){
	return (
		BETWEEN(c, 'a', 'z') ||
		BETWEEN(c, 'A', 'Z') ||
		BETWEEN(c, '0', '9') ||
		(c == '_')
	);
}


inline bool isSegmentDirective(const string& s){
	return	s == "if"   || s == "ifdef"   || s == "ifndef"   ||
			s == "elif" || s == "elifdef" || s == "elifndef" ||
			s == "else";
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


void Parser::parse(istream& in){
	if (in.bad() || buffSize < 1){
		return;
	}
	
	// Reset
	this->in = &in;
	reset();
	
	
	bool macro = true;
	SourceString tmp;
	SourceString* line = &codeSegments->emplace_back();	// Temporary code line
	
	while (true){
		parseWhiteSpaceAndComment(*line, true);
		char c = ch();
		
		// EOL
		if (c == '\n'){
			line->clear();
			macro = true;
			nl();
		}
		
		// Macros
		else if (c == '#' && macro){
			macro = false;
			
			push();
			parseMacro(*line, tmp.clear());
			
			if (isSegmentDirective(tmp)){
				pop(1, true);
				parseSegment();
			} else {
				// pop(1, false);
				// line = &codeSegments->emplace_back();
				pop(1, true);
				throw ParserException(getLoc(), "Unexpected directive.");
			}
			
		}
		
		// Reductions
		else if (isIdChar(c)){
			macro = false;
			Reduction& r = reductions->emplace_back();
			parseReduction(r);
		}
		
		// EOF / error
		else if (c == 0){
			break;
		} else {
			throw ParserException(getLoc(), "Unexpected character.");
		}
		
	}
	
	
	// Delete temporary code line
	codeSegments->pop_back();
	return;
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


void Parser::parseSegment(){
	const Location start = getLoc();
	SourceString dir;
	SourceString typeName;
	
	enum class SegmentType {
		NONE,
		REDUCTIONS,
		CODE
	} elseType;
	
	// String to enum SegmentType
	auto getType = [](string& s) -> SegmentType {
		if (s == DIR_REDUCT)
			return SegmentType::REDUCTIONS;
		else if (s == DIR_CODE)
			return SegmentType::CODE;
		else
			return SegmentType::NONE;
	};
	
	/**
	 * @brief Invoke parsing for a segment type.
	 * @return Inverted segment type for 'else' statements.
	 */
	auto parseBody = [&](SegmentType type, const Location& errorLoc) -> SegmentType {
		switch (type){
			case SegmentType::REDUCTIONS: {
				parseSegment_reductions(*reductions);
				return SegmentType::CODE;
			}
			case SegmentType::CODE: {
				SourceString& s = codeSegments->emplace_back(getLoc());
				parseSegment_code(s);
				s.end = getLoc();
				return SegmentType::REDUCTIONS;
			}
			default: {
				throw ParserException(errorLoc, "Expected " DIR_REDUCT " or " DIR_CODE " segment.");
			}
		}
	};
	
	
	// Parse first segment
	{
		parseSegment_header(dir, typeName);
		
		if (dir != "if" && dir != "ifdef"){
			throw ParserException(dir.start, "Expected 'if' or 'ifdef' segment directive.");
		}
		
		elseType = parseBody(getType(typeName), typeName.start);
	}
	
	
	// Parse other segments
	bool endifExpected = false;
	while (true){
		
		// Newline
		if (ch() == '\n'){
			nl();
		} else if (ch() == 0){
			throw ParserException(start, "Unterminated segment. EOF reached.");
		}
		
		parseWhiteSpaceAndComment(trash.clear(), true);
		parseSegment_header(dir, typeName);
		SegmentType seg;
		
		// Terminatind directive
		if (dir == "endif"){
			break;
		} else if (endifExpected){
			throw ParserException(dir.start, "Expected 'endif' directive.");
		}
		
		// Get segment type
		else if (dir == "else"){
			seg = elseType;
			endifExpected = true;
			
			if (!typeName.empty()){
				throw ParserException(dir.start, "Unexpected segment type.");
			}
			
		} else if (dir == "elif" || dir == "elifdef"){
			seg = getType(typeName);
		}
		
		// Error
		else {
			throw ParserException(dir.start, "Expected 'else', 'elif' or 'elifdef' segment directive.");
		}
		
		elseType = parseBody(seg, typeName.start);
	}
	
	
	return;
}


void Parser::parseSegment_header(SourceString& directive, SourceString& type){
	if (ch() != '#')
		throw ParserException(getLoc(), "Expected segment declaration.");
	inc();
	
	// Parse directive
	directive.clear();
	directive.start = getLoc();
	parseSolidSpace(directive);
	directive.end = getLoc();
	
	// Parse segment type
	type.clear();
	if (parseWhiteSpaceAndComment(trash.clear(), true) >= 1){
		parseId(type);
	}
	
	// Parse remaining space and comments
	parseWhiteSpaceAndComment(trash.clear(), true);
	if (ch() == '\n'){
		nl();
	} else {
		throw ParserException(getLoc(), "Expected termination of segment declaration.");
	}
	
	return;
}


void Parser::parseSegment_code(string& s){
	SourceString dir;
	
	push();
	int acceptedSize = s.size();
	bool macro = true;
	int lvl = 0;
	
	// Parse lines
	while (true){
		char c = ch();
		
		// Macro
		if (c == '#' && macro){
			macro = false;
			
			dir.clear();
			parseMacro(s, dir);
			
			// Manage macro conditional depth
			if (dir == "if" || dir == "ifdef" || dir == "ifndef"){
				lvl++;
			} else if (dir == "else" || dir == "elif" || dir == "elifdef" || dir == "elifndef"){
				if (lvl == 0)
					lvl--;
			} else if (dir == "endif"){
				lvl--;
			}
			
			// New segment header detected, discard line
			if (lvl < 0){
				s.resize(acceptedSize);
				pop(1, true);
				break;
			}
			
		}
		
		// Check whitespace
		else if (c == '\n'){
			acceptedSize = s.size();
			macro = true;
			
			pop(1, false);
			push();
			
			nl();
			s.push_back('\n');
			
			// Quick skip whitepsace
			parseWhiteSpaceAndComment(s, true);
		} else if (c == '\t'){
			s.push_back('\t');
			tab();
		} else if (isspace(c)){
			s.push_back(c);
			inc();
		}
		
		// Scopes
		else if (LOOKAHEAD_IS_COMMENT(c)){
			parseComment(s);
		} else if (c == '"' || c == '\''){
			macro = false;
			parseStringLiteral(s);
		}
		
		// Check for regular characters
		 else if (c != 0){
			macro = false;
			s.push_back(c);
			inc();
		}
		
		// EOF
		else {
			acceptedSize = s.size();
			pop(1, false);
			break;
		}
		
	}
	
	return;
}


void Parser::parseSegment_reductions(vector<Reduction>& reductions){
	bool macro = true;
	push();
	
	while (true){
		char c = ch();
		
		// Macro
		if (c == '#' && macro){
			pop(1, true);
			break;
		}
		
		// Check whitespace
		else if (c == '\n'){
			macro = true;
			
			pop(1, false);
			push();
			nl();
			
			// Quick skip whitepsace
			parseWhiteSpace(trash.clear(), true);
		} else if (c == '\t'){
			tab();
		} else if (isspace(c)){
			inc();
		}
		
		// Reduction
		else if (c != 0){
			Reduction& r = reductions.emplace_back();
			parseReduction(r);
		}
		
		// EOF
		else {
			break;
		}
		
		
	}
	
	return;
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


void Parser::parseReduction(Reduction& r){
	if (!isIdChar(ch())){
		throw ParserException(getLoc(), "Expected symbol name.");
	}
	
	/**
	 * @brief Parse string of symbols and attributes until unknown character is reached.
	 * @param v Where parsed symbols are stored.
	 */
	auto parseSymbols = [&](vector<Symbol>& v){
		while (true){
			parseWhiteSpaceAndComment(trash.clear(), true);
			
			if (isIdChar(ch())){
				Symbol& sym = v.emplace_back();
				parseReduction_symbol(sym);
			} else {
				break;
			}
			
		}
	};
	
	
	// Parse left symbols
	parseSymbols(r.left);
	if (r.left.size() <= 0){
		throw ParserException(getLoc(), "Missing left side of reduction.");
	}
	
	
	// Arrow
	if (!match("->", true)){
		if (ch() == '[')
			throw ParserException(getLoc(), "Symbol attributes missing symbol name.");
		else
			throw ParserException(getLoc(), "Expected left-right separator \"->\".");
	}
	
	
	// Parse right symbols
	parseSymbols(r.right);
	if (r.right.size() <= 0){
		throw ParserException(getLoc(), "Missing right side of reduction.");
	}
	
	
	// Parse inline code
	if (ch() == '{'){
		parseReduction_inlineCode(r.code);
		parseWhiteSpace(trash.clear(), true);
	}
	
	
	// EOL
	if (ch() != '\n' && ch() != 0){
		throw ParserException(getLoc(), "Expected symbol name or termination using '\\n'.");
	}
	
	return;
}


void Parser::parseReduction_symbol(Symbol& sym){
	// First letter is capital
	if (!isIdFirstChar(ch())){
		throw ParserException(getLoc(), "Symbols must start with a capital letter.");
	}
	
	// Parse symbol name
	sym.clear();
	parseId(sym.name);
	
	// Lookahead and parse additional attributes
	push();
	parseWhiteSpaceAndComment(trash.clear(), true);
	
	if (ch() == '['){
		pop(1, false);
		parseReduction_symbol_attributes(sym);
	} else {
		pop(1, true);
	}
	
}


void Parser::parseReduction_symbol_attributes(Symbol& sym){
	if (ch() != '['){
		throw ParserException(getLoc(), "Expected '[' when declaring symbol attributes.");
	}
	
	inc();
	parseWhiteSpaceAndComment(trash.clear(), true);
	
	if (isIdChar(ch())){
		parseId(sym.id);
	} else {
		throw ParserException(getLoc(), "Expected symbol ID.");
	}
	
	parseWhiteSpaceAndComment(trash.clear(), true);
	if (ch() != ']'){
		throw ParserException(getLoc(), "Expected ']' at the end of symbol attributes declaration.");
	}
	
	inc();
}


void Parser::parseReduction_inlineCode(SourceString& code){
	if (ch() != '{'){
		throw ParserException(getLoc(), "Expected '{' at reduction inline code segment.");
	}
	
	code.clear();
	code.start = getLoc();
	
	code.push_back('{');
	inc();
	
	bool macro = true;
	int lvl = 1;
	
	while (lvl > 0){
		char c = ch();
		code.push_back(c);
		
		// Whitespace
		if (c == '\n'){
			macro = true;
			nl();
		} else if (c == '\t'){
			tab();
		} else if (isspace(c)){
			inc();
		}
		
		// Bracket context
		else if (c == '{'){
			lvl++;
			inc();
		} else if (c == '}'){
			lvl--;
			inc();
			if (lvl <= 0)
				break;
		}
		
		// Other contexts
		else if (LOOKAHEAD_IS_COMMENT(c)){
			code.pop_back();
			parseComment(code);
		} else if (c == '"' || c == '\''){
			macro = false;
			code.pop_back();
			parseStringLiteral(code);
		}
		
		// Macro
		else if (c == '#' && macro){
			macro = false;
			code.pop_back();
			parseMacro(code, trash.clear());
		}
		
		// Other
		else if (c != 0){
			inc();
		}
		
		// EOF
		else {
			throw ParserException(code.start, "Missing closing bracket '}'.");
		}
		
	}
	
	code.end = getLoc();
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


void Parser::parseMacro(string& s, SourceString& directive){
	if (ch() != '#'){
		throw ParserException(getLoc(), "Preprocessor directive declaration expected.");
	}
	
	s.push_back('#');
	inc();
	parseWhiteSpace(s, true);
	
	
	// Parse directive
	directive.clear();
	directive.start = getLoc();
	
	while (true){
		char c = ch();
		
		if (isspace(c) || (c == '\\' && match("\\\n")) || c == 0){
			break;
		} else {
			directive.push_back(c);
			s.push_back(c);
			inc();
		}
		
	}
	
	directive.end = getLoc();
	
	
	// Parse macro body
	while (true){
		char c = ch();
		s.push_back(c);
		
		if (c == '"' || c == '\''){
			s.pop_back();
			parseStringLiteral(s);
		} else if (c == '\t'){
			tab();
		} else if (c == '\n'){
			s.pop_back();
			break;
		} else if (c == '\\'){
			inc();
			if (ch() == '\n'){
				s.push_back('\n');
				nl();
			}
		} else if (c =='/' && (match("//") || match("/*"))){
			s.pop_back();
			parseComment(s);
		} else if (c != 0){
			inc();
		} else {
			s.pop_back();
			break;
		}
		
	}
	
	
	return;
}


int Parser::parseId(SourceString& s){
	s.clear();
	s.start = getLoc();
	
	char c = ch();
	while (isIdChar(c)){
		s.push_back(c);
		inc();
		c = ch();
	}
	
	s.end = getLoc();
	return s.size();
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


int Parser::parseWhiteSpace(string& s, bool escapedNewline){
	const int size = s.size();
	
	while (true){
		char c = ch();
		
		if (c == '\t'){
			tab();
		} else if (c == '\n'){
			if (!escapedNewline)
				nl();
			else
				break;
		} else if (c == '\\'){
			fillBuffer(2);
			if (buff[i+1] == '\n'){
				s.push_back('\\');
				s.push_back('\n');
				inc();
				nl();
				continue;
			} else {
				break;
			}
		} else if (isspace(c)){
			inc();
		} else {
			break;
		}
		
		s.push_back(c);
	}
	
	return s.size() - size;
}


int Parser::parseComment(string& s){
	if (ch() != '/'){
		throw ParserException(getLoc(), "Expected comment.");
	}
	
	const Location start = getLoc();
	const int size = s.size();
	
	s.push_back('/');
	inc();
	
	// Comment line
	if (ch() == '/'){
		s.push_back('/');
		inc();
		
		while (true){
			char c = ch();
			
			if (c == '\t'){
				tab();
			} else if (c == '\\' && match("\\\n")){
				s.push_back('\\');
				s.push_back('\n');
				inc();
				nl();
				continue;
			} else if (c == '\n' || c == 0){
				break;
			} else {
				inc();
			}
			
			s.push_back(c);
		}
		
	}
	
	// Comment Block
	else if (ch() == '*'){
		s.push_back('*');
		inc();
		
		while (true){
			char c = ch();
			
			if (c == '\t'){
				tab();
			} if (c == '\n'){
				nl();
			} else if (c == '*' && match("*/")){
				s.push_back('*');
				s.push_back('/');
				inc(2);
				break;
			} else if (c == 0){
				throw ParserException(start, "Unterminated comment block.");
			} else {
				inc();
			}
			
			s.push_back(c);
		}
		
	}
	
	else {
		throw ParserException(getLoc(), "Expected comment.");
	}
	
	return s.size() - size;
}


int Parser::parseWhiteSpaceAndComment(string& s, bool escapedNewline){
	const int len = s.size();
	
	while (true){
		parseWhiteSpace(s, escapedNewline);
		if (LOOKAHEAD_IS_COMMENT(ch()))
			parseComment(s);
		else
			break;
	}
	
	return s.size() - len;
}



int Parser::parseSolidSpace(std::string& s, bool includeEscaped){
	const int len = s.size();
	
	while (true){
		char c = ch();
		
		// Escaped
		if (c == '\\'){
			if (includeEscaped){
				s.push_back(c);
				inc();
				c = ch();
				
				if (c == '\n')
					nl();
				else if (c == '\t')
					tab();
				else
					inc();
				
				s.push_back(c);
			} else {
				break;
			}
		}
		
		else if (isgraph(c)){
			inc();
			s.push_back(c);
		}
		
		else {
			break;
		}
		
	}
	
	return s.size() - len;
}


int Parser::parseStringLiteral(string& s){
	const char terminator = ch();
	if (terminator != '\'' && terminator != '"'){
		throw ParserException(getLoc(), "Expected string or character literal.");
	}
	
	const Location start = getLoc();
	const int size = s.size();
	
	s.push_back(terminator);
	inc();
	
	int lvl = 1;
	while (lvl > 0){
		char c = ch();
		s.push_back(c);
		
		if (c == '\t'){
			tab();
		} else if (c == '\n'){
			nl();
		} else if (c == '\\'){
			fillBuffer(2);
			inc();
			if (ch() == terminator){
				s.push_back(terminator);
				inc();
			}
		} else if (c == terminator){
			inc();
			break;
		} else if (c != 0){
			inc();
		} else {
			throw ParserException(start, "Unterminated string or character literal.");
		}
		
	}
	
	return s.size() - size;
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


bool Parser::match(const char* s, bool move, string* out){
	int ii = 0;
	
	// Try to match with current buffer
	while (s[ii] != 0 && (i+ii) < n){
		if (buff[i+ii] != s[ii])
			return false;
		ii++;
	}
	
	// Buffer underflow
	if (s[ii] != 0 && (i+ii) >= n){
		int len = ii + strlen(&s[ii]);
		
		if (!lookAhead(len))
			return false;
		
		// Match remaining string
		while (ii < len){
			if (buff[i+ii] != s[ii])
				return false;
			ii++;
		}
		
	}
	
	if (move)
		inc(ii);
	if (out != nullptr)
		out->append(s);
	
	return true;
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


const Location& Parser::push(){
	return frames.emplace_back(getLoc());
}


void Parser::pop(int count, bool applyLocation){
	count = (count < 0) ? frames.size() : count;
	
	if (0 <= count && count <= frames.size()){
		if (applyLocation){
			const Location loc = frames[frames.size() - count];
			i = loc.i - bi;
			ri = loc.row;
			ci = loc.col;
		}
		frames.resize(frames.size() - count);
	} else {
		throw runtime_error("Internal stack error.");
	}
	
}


bool Parser::extractString(const Location& start, const Location& end, std::string& out_str){
	const int start_i = start.i - bi;
	const int end_i = end.i - bi;
	const int count = end_i - start_i;
	
	if (start_i >= 0 && end_i <= n && count > 0){
		out_str.append(&buff[start_i], count);
		return true;
	}
	
	return false;
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


void Parser::reset(){
	i = 0;
	bi = 0;
	ci = 0;
	ri = 0;
	n = 0;
	eof = false;
	
	if (buff == nullptr){
		_buffSize = buffSize + 1;
		buff = new char[_buffSize];
	}
	buff[0] = 0;
	
	if (codeSegments == nullptr)
		codeSegments = new vector<SourceString>();
	if (reductions == nullptr)
		reductions = new vector<Reduction>();
	
	codeSegments->clear();
	reductions->clear();
	codeSegments->reserve(32);
	reductions->reserve(32);
	
	trash.clear();
	frames.clear();
}


int Parser::fillBuffer(int count, bool fill){
	if (count <= (n - i) || count <= 0 || eof){
		return 0;
	}
	
	const int space = _buffSize - n - 1;
	int skip = 0;
	int p;
	int preserve;
	int requiredSize;
	
	if (frames.empty()){
		p = min(i, n);
	} else {
		p = min(frames[0].i - bi, n);
	}
	
	preserve = n - p;
	
	if (i > n){
		if (preserve == 0)
			skip = i - n;
		else
			count += i - n;
	} else {
		count -= n - i;
	}
	
	requiredSize = preserve + count + 1;
	
	
	// Not enough empty space at the end
	if (count > space){
		
		// Resize buffer
		if (requiredSize > _buffSize){
			// (multiple of buffSize) + 1
			_buffSize = buffSize * ((requiredSize + buffSize)/buffSize) + 1;
			char* _buff = new char[_buffSize];
			copy(&buff[p], &buff[n], &_buff[0]);
			swap(buff, _buff);
			delete[] _buff;
		}
		
		// Shift buffer
		else {
			copy(&buff[p], &buff[n], &buff[0]);
		}
		
		n = preserve;
		bi += p;
		i -= p;
	}
	
	// Skip characters
	if (skip > 0){
		in->seekg(skip, ios::cur);
		n = 0;
		bi += i;
		i = 0;
	}
	
	// Use all remaining space
	if (fill && (n + count + 1) < _buffSize){
		count = _buffSize - n - 1;
	}
	
	// Read data to empty space in buffer
	if (count > 0){
		in->read(&buff[n], count);
		count = in->gcount();
		n += count;
		buff[n] = 0;
	} else {
		count = 0;
	}
	
	eof = (count == 0);
	return count;
}


// ------------------------------------------------------------------------------------------ //