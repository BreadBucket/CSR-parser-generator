#include "Parser.hpp"

#include <cstring>
#include <iostream>
#include "util/ANSI.h"
#include "util/utils.hpp"

using namespace std;
using namespace csg;


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
char* locStr(const csg::Location& loc){
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


// ----------------------------------- [ Functions ] ---------------------------------------- //


void Parser::parse(istream& in){
	buffSize = 8;
	
	if (in.bad() || buffSize < 1){
		return;
	}
	
	// Reset
	this->in = &in;
	reset();
	
	
	
	while (true){
		parseWhiteSpace(trash.clear(), true);
		char c = ch();
		
		if (c == '\n'){
			nl();
		} else if (isIdChar(c)){
			printch();
			throw runtime_error("debug");
			inc();
		} else if (c == '#'){
			parseSegment();
		} else if (c == 0){
			break;
		} else {
			throw ParserException(getLoc(), "Unexpected character.");
		}
		
	}
	
	
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


void Parser::parseSegment(){
	const Location start = getLoc();
	SourceString dir;
	SourceString typeName;
	
	enum SegmentType {
		NONE,
		CSG,
		CSG_CODE
	} elseType;
	
	// String to enum SegmentType
	auto getType = [](string& s) -> SegmentType {
		if (s.compare("CSG") == 0)
			return SegmentType::CSG;
		else if (s.compare("CSG_CODE") == 0)
			return SegmentType::CSG_CODE;
		else
			return SegmentType::NONE;
	};
	
	/**
	 * @brief Invoke parsing for a segment type.
	 * @return Inverted segment type for 'else' statements.
	 */
	auto parseBody = [&](SegmentType type, const Location& errorLoc) -> SegmentType {
		switch (type){
			case SegmentType::CSG: {
				parseSegment_reductions(*reductions);
				return SegmentType::CSG_CODE;
			}
			case SegmentType::CSG_CODE: {
				SourceString& s = codeSegments->emplace_back(getLoc());
				parseSegment_code(s);
				s.end = getLoc();
				return SegmentType::CSG;
			}
			default: {
				throw ParserException(errorLoc, "Expected CSG or CSG_CODE segment.");
			}
		}
	};
	
	
	// Parse first segment
	{
		parseSegment_header(dir, typeName);
		
		if (dir.compare("if") != 0 && dir.compare("ifdef") != 0){
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
		
		parseWhiteSpace(trash.clear(), true);
		parseSegment_header(dir, typeName);
		SegmentType seg;
		
		// Terminatind directive
		if (dir.compare("endif") == 0){
			break;
		} else if (endifExpected){
			throw ParserException(dir.start, "Expected 'endif' directive.");
		}
		
		// Get segment type
		else if (dir.compare("else") == 0){
			seg = elseType;
			endifExpected = true;
			
			if (!typeName.empty()){
				throw ParserException(dir.start, "Unexpected segment type.");
			}
			
		} else if (dir.compare("elif") == 0 || dir.compare("elifdef") == 0){
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
	
	while (true){
		char c = ch();
		
		if (isspace(c) || (c == '\\' && match("\\\n")) || c == 0){
			break;
		} else {
			directive.push_back(c);
			inc();
		}
		
	}
	
	directive.end = getLoc();
	
	
	// Parse segment type
	type.clear();
	if (parseWhiteSpace(trash.clear(), true) >= 1){
		type.start = getLoc();
		
		while (true){
			char c = ch();
			
			if (isIdChar(c)){
				type.push_back(c);
				inc();
			} else {
				break;
			}
			
		}
		
		type.end = getLoc();
	}
	
	
	// Parse remaining space and comments
	while (true){
		parseWhiteSpace(trash.clear(), true);
		
		char c = ch();
		
		if (c == '/'){
			parseComment(trash.clear());
		} else if (c == '\n'){
			nl();
			break;
		} else if (c == 0){
			break;
		} else {
			throw ParserException(getLoc(), "Unexpected character in segment declaration.");
		}
		
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
			if (dir.compare("if") == 0 || dir.compare("ifdef") == 0 || dir.compare("ifndef") == 0){
				lvl++;
			} else if (dir.compare("else") == 0 || dir.compare("elif") == 0 || dir.compare("elifdef") == 0){
				if (lvl == 0)
					lvl--;
			} else if (dir.compare("endif") == 0){
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
			parseWhiteSpace(s, true);
		} else if (c == '\t'){
			s.push_back('\t');
			tab();
		} else if (isspace(c)){
			s.push_back(c);
			inc();
		}
		
		// Check for regular characters
		else if (c == '"' || c == '\''){
			macro = false;
			parseStringLiteral(s);
		} else if (c == '/'){
			macro = false;
			if (match("//") || match("/*")){
				parseComment(s);
			} else {
				s.push_back(c);
				inc();
			}
		} else if (c != 0){
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
			printreduction(r);
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
	
	// Parse left symbols
	if (isIdChar(ch())){
		while (isIdChar(ch())){
			Symbol& sym = r.left.emplace_back();
			parseSymbol(sym);
			parseWhiteSpace(trash.clear(), true);
		}
	} else {
		throw ParserException(getLoc(), "Expected symbol name.");
	}
	
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
	
	parseWhiteSpace(trash.clear(), true);
	
	
	// Parse right symbols
	while (isIdChar(ch())){
		Symbol& sym = r.right.emplace_back();
		parseSymbol(sym);
		parseWhiteSpace(trash.clear(), true);
	}
	
	// Parse inline code
	if (ch() == '{'){
		parseReductionInlineCode(r.code);
		parseWhiteSpace(trash.clear(), true);
	}
	
	if (ch() != '\n' && ch() != 0){
		throw ParserException(getLoc(), "Expected reduction termination using newline '\\n'.");
	} else if (r.right.size() <= 0){
		throw ParserException(getLoc(), "Missing right side of reduction.");
	}
	
	
	return;
}


void Parser::parseSymbol(Symbol& sym){
	// First letter is capital
	if (!isIdFirstChar(ch())){
		throw ParserException(getLoc(), "Symbols must start with a capital letter.");
	}
	
	// Parse symbol name
	sym.clear();
	parseId(sym.name);
	
	// Lookahead and parse additional attributes
	push();
	parseWhiteSpace(trash.clear(), true);
	
	if (ch() == '['){
		pop(1, false);
		parseSymbolAttributes(sym);
	} else {
		pop(1, true);
	}
	
}


void Parser::parseSymbolAttributes(Symbol& sym){
	if (ch() != '['){
		throw ParserException(getLoc(), "Expected '[' when declaring symbol attributes.");
	}
	
	inc();
	parseWhiteSpace(trash.clear(), true);
	
	if (isIdChar(ch())){
		parseId(sym.id);
	} else {
		throw ParserException(getLoc(), "Unexpected character.");
	}
	
	parseWhiteSpace(trash.clear(), true);
	if (ch() != ']'){
		throw ParserException(getLoc(), "Expected ']' at the end of symbol attributes declaration.");
	}
	
	inc();
}


void Parser::parseId(SourceString& str){
	str.clear();
	str.start = getLoc();
	
	char c = ch();
	while (isIdChar(c)){
		str.push_back(c);
		inc();
		c = ch();
	}
	
	str.end = getLoc();
}


void Parser::parseReductionInlineCode(SourceString& code){
	// TODO
	throw ParserException(getLoc(), "!!! inline code TODO !!!");
	// if (ch() != '{'){
	// 	throw ParserException(getLoc(), "Expected '{' at reduction inline code segment.");
	// }
	
	// code.clear();
	// code.start = getLoc();
	
	// code.push_back('{');
	// inc();
	
	// int bracket = 1;
	// int quote_1 = 0;
	// int quote_2 = 0;
	// while (bracket > 0){
	// 	char c = ch();
		
	// 	if (c == 0){
	// 		break;
	// 	} else if (c == '\n'){
	// 		nl();
	// 		inc(-1);
	// 	} else if (c == '\t'){
	// 		tab();
	// 		inc(-1);
	// 	}
		
	// 	// String context
	// 	else if (quote_1 > 0){
	// 		if (c == '"')
	// 			quote_1--;
	// 		else if (c == '\\')
	// 			inc();
	// 	}
		
	// 	// Literal context
	// 	else if (quote_2 > 0){
	// 		if (c == '\'')
	// 			quote_2--;
	// 		else if (c == '\\')
	// 			inc();
	// 	}
		
	// 	// Default context
	// 	else {
	// 		if (c == '{')
	// 			bracket++;
	// 		else if (c == '}'){
	// 			bracket--;
	// 		}
	// 		else if (c == '"')
	// 			quote_1++;
	// 		else if (c == '\'')
	// 			quote_1++;
	// 	}
		
	// 	code.push_back(c);
	// 	inc();
	// }
	
	// if (bracket > 0){
	// 	throw ParserException(code.start, "Missing closing bracket '}'.");
	// }
	
	// code.end = getLoc() - 1;
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


void Parser::pop(int count, bool apply){
	count = (count < 0) ? frames.size() : count;
	
	if (0 <= count && count <= frames.size()){
		if (apply){
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
	if (count <= 0 || count <= (n - i)){
		return 0;
	}
	
	PRINTF("READ: %d\n", count);
	
	
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
	
	PRINTF("  [%d/%d]: preserve:%d, p:%d, count:%d, space:%d, skip:%d, req:%d/%d \n", i, n, preserve, p, count, space, skip, requiredSize, _buffSize);
	
	
	// Not enough empty space at the end
	if (count > space){
		
		// Resize buffer
		if (requiredSize > _buffSize){
			PRINTF("  RESIZE: %d", _buffSize);
			
			// (multiple of buffSize) + 1
			_buffSize = buffSize * ((requiredSize + buffSize)/buffSize) + 1;
			char* _buff = new char[_buffSize];
			
			PRINTF(" -> %d\n", _buffSize);
			copy(&buff[p], &buff[n], &_buff[0]);
			
			swap(buff, _buff);
			delete[] _buff;
			
		}
		
		// Shift buffer
		else {
			PRINTF("  SHIFT: [%d..%d] -> [0..%d]\n", i, n, preserve);
			copy(&buff[p], &buff[n], &buff[0]);
		}
		
		n = preserve;
		bi += p;
		i -= p;
	}
	
	// Skip characters
	if (skip > 0){
		PRINTF("  SKIP: %d\n", skip);
		in->seekg(skip, ios::cur);
		n = 0;
		bi += i;
		i = 0;
	}
	
	// Use all remaining space
	if (fill && (n + count + 1) < _buffSize){
		count = _buffSize - n - 1;
		PRINTF("  count:%d\n", count);
	}
	
	// Read data to empty space in buffer
	if (count > 0){
		in->read(&buff[n], count);
		count = in->gcount();
		PRINTF("  IO: %d\n", count);
		n += count;
		buff[n] = 0;
	} else {
		count = 0;
	}
	
	eof = (count == 0);
	return count;
}

// ------------------------------------------------------------------------------------------ //