#include "Parser.hpp"

#include <cstring>
#include <iostream>
#include "util/ANSI.h"
#include "util/utils.hpp"

using namespace std;
using namespace csg;


// ----------------------------------- [ Functions ] ---------------------------------------- //


// DEBUG
#define PATH "test/test.csg"


// DEBUG
void printSrc(const SourceString& src){
	if (!src.empty()){
		printf("[%d,%d): @", src.start.i, src.end.i);
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
		push();
		
		parseWhiteSpace(trash, true);
		trash.clear();
		char c = ch();
		
		if (c == '\n'){
			pop(-1, false);
			nl();
		} else if (isIdChar(c)){
			pop(-1, false);
			inc();
		} else if (c == '#'){
			pop(-1, false);
			parseSegment();
		} else if (c == 0){
			pop(-1, false);
			break;
		} else {
			pop(-1, false);
			throw ParserException(getLoc(), "Unexpected character.");
		}
		
	}
	
	
	
	// for (auto& s : *codeSegments){
	// 	printSrc(s);
	// }
	
	
	
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


void Parser::parseSegment(){
	if (ch() != '#'){
		throw ParserException(getLoc(), "Expected segment declaration.");
	}
	
	
	printch();
	
	inc(8);
	printch();
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


// void Parser::parseMacroSegment(){
// 	SourceString& s = codeSegments->emplace_back(getLoc());
	
// 	MacroType type = parseMacro(s);
	
// 	// Check if conditional macro
// 	int lvl = 0;
// 	switch (type){
// 		case MacroType::IF:
// 		case MacroType::IFDEF:
// 		case MacroType::IFNDEF:
// 		case MacroType::ELIF:
// 		case MacroType::ELIFDEF:
// 		case MacroType::ELIFNDEF:
// 		case MacroType::ELSE:
// 			lvl = 1;
// 			break;
// 	}
	
	
// 	printf("%d: ", type);
// 	// printSrc();
	
	
// 	// Parse conditional macro body
// 	while (lvl > 0){
// 		char c = ch();
		
// 		if (c == '\n'){
// 			s.push_back(c);
// 			nl();
// 		} else if (c == '\t'){
// 			s.push_back(c);
// 			tab();
// 		} else if (c == '"' || c == '\''){
// 			parseStringLiteral(s);
// 		} else if (c == '/' && match("//")){
// 			parseComment(s);
// 		} else if (c == '#'){
// 			MacroType type = parseMacro(s);
			
// 			switch (type){
// 				case MacroType::IF:
// 				case MacroType::IFDEF:
// 				case MacroType::IFNDEF:
// 					lvl++;
// 					break;
// 				case MacroType::ENDIF:
// 					lvl--;
// 					break;
// 				default:
// 					break;
// 			}
			
// 		} else if (c != 0){
// 			s.push_back(c);
// 			inc();
// 		} else {
// 			throw ParserException(s.start, "Unterminated conditional preprocessor directive.");
// 		}
		
// 	}
	
// 	s.end = getLoc();
// }


// bool Parser::parse_continueConditionalMacroBody(std::string& s){
// 	int lvl = 1;
	

	
// 	return true;
// }


// ----------------------------------- [ Functions ] ---------------------------------------- //


Parser::MacroType Parser::parseMacro(string& s){
	if (ch() != '#'){
		throw ParserException(getLoc(), "Preprocessor directive declaration expected.");
	}
	
	s.push_back('#');
	inc();
	
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
	
	return MacroType::UNKNOWN;
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


// void Parser::parseReduction(Reduction& r){
// 	// Parse left symbols
// 	if (isIdChar(ch())){
// 		while (isIdChar(ch())){
// 			Symbol& sym = r.left.emplace_back();
// 			parseSymbol(sym);
// 			skipSpace(true);
// 		}
// 	} else {
// 		throw ParserException(getLoc(), "Expected symbol name.");
// 	}
	
// 	if (r.left.size() <= 0){
// 		throw ParserException(getLoc(), "Missing left side of reduction.");
// 	}
	
	
// 	// Arrow
// 	if (!match("->", true)){
// 		if (ch() == '[')
// 			throw ParserException(getLoc(), "Symbol attributes missing symbol name.");
// 		else
// 			throw ParserException(getLoc(), "Expected left-right separator \"->\".");
// 	}
	
// 	skipSpace(true);
	
	
// 	// Parse right symbols
// 	while (isIdChar(ch())){
// 		Symbol& sym = r.right.emplace_back();
// 		parseSymbol(sym);
// 		skipSpace(true);
// 	}
	
// 	// Parse inline code
// 	if (ch() == '{'){
// 		parseReductionInlineCode(r.code);
// 		skipSpace(true);
// 	}
	
// 	if (ch() != '\n' && ch() != 0){
// 		throw ParserException(getLoc(), "Expected reduction termination using newline '\\n'.");
// 	} else if (r.right.size() <= 0){
// 		throw ParserException(getLoc(), "Missing right side of reduction.");
// 	}
	
	
// 	return;
// }


// void Parser::parseSymbol(Symbol& sym){
// 	// First letter is capital
// 	if (!isIdFirstChar(ch())){
// 		throw ParserException(getLoc(), "Symbols must start with a capital letter.");
// 	}
	
// 	// Parse symbol name
// 	sym.clear();
// 	parseId(sym.name);
	
// 	// Parse additional attributes
// 	if (ch() == '['){
// 		parseSymbolAttributes(sym);
// 	}
	
// }


// void Parser::parseSymbolAttributes(Symbol& sym){
// 	if (ch() != '['){
// 		throw ParserException(getLoc(), "Expected '[' when declaring symbol attributes.");
// 	}
	
// 	inc();
// 	skipSpace(true);
	
// 	if (isIdChar(ch())){
// 		parseId(sym.id);
// 	} else {
// 		throw ParserException(getLoc(), "Unexpected character.");
// 	}
	
// 	skipSpace(true);
// 	if (ch() != ']'){
// 		throw ParserException(getLoc(), "Expected ']' at the end of symbol attributes declaration.");
// 	}
	
// 	inc();
// }


// void Parser::parseId(SourceString& str){
// 	str.clear();
// 	str.start = getLoc();
	
// 	char c = ch();
// 	while (isIdChar(c)){
// 		str.push_back(c);
// 		inc();
// 		c = ch();
// 	}
	
// 	str.end = getLoc() - 1;
// }


// void Parser::parseReductionInlineCode(SourceString& code){
// 	if (ch() != '{'){
// 		throw ParserException(getLoc(), "Expected '{' at reduction inline code declaration.");
// 	}
	
// 	code.clear();
// 	code.start = getLoc();
	
// 	code.push_back('{');
// 	inc();
	
// 	int bracket = 1;
// 	int quote_1 = 0;
// 	int quote_2 = 0;
// 	while (bracket > 0){
// 		char c = ch();
		
// 		if (c == 0){
// 			break;
// 		} else if (c == '\n'){
// 			nl();
// 			inc(-1);
// 		} else if (c == '\t'){
// 			tab();
// 			inc(-1);
// 		}
		
// 		// String context
// 		else if (quote_1 > 0){
// 			if (c == '"')
// 				quote_1--;
// 			else if (c == '\\')
// 				inc();
// 		}
		
// 		// Literal context
// 		else if (quote_2 > 0){
// 			if (c == '\'')
// 				quote_2--;
// 			else if (c == '\\')
// 				inc();
// 		}
		
// 		// Default context
// 		else {
// 			if (c == '{')
// 				bracket++;
// 			else if (c == '}'){
// 				bracket--;
// 			}
// 			else if (c == '"')
// 				quote_1++;
// 			else if (c == '\'')
// 				quote_1++;
// 		}
		
// 		code.push_back(c);
// 		inc();
// 	}
	
// 	if (bracket > 0){
// 		throw ParserException(code.start, "Missing closing bracket '}'.");
// 	}
	
// 	code.end = getLoc() - 1;
// }


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
		fillBuffer(len);
		
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
		throw ParserException("Internal stack error.");
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
	
	printf("READ: %d\n", count);
	
	
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
	
	printf("  [%d/%d]: preserve:%d, p:%d, count:%d, space:%d, skip:%d, req:%d/%d \n", i, n, preserve, p, count, space, skip, requiredSize, _buffSize);
	
	
	// Not enough empty space at the end
	if (count > space){
		
		// Resize buffer
		if (requiredSize > _buffSize){
			printf("  RESIZE: %d", _buffSize);
			
			// (multiple of buffSize) + 1
			_buffSize = buffSize * ((requiredSize + buffSize)/buffSize) + 1;
			char* _buff = new char[_buffSize];
			
			printf(" -> %d\n", _buffSize);
			copy(&buff[p], &buff[n], &_buff[0]);
			
			swap(buff, _buff);
			delete[] _buff;
			
		}
		
		// Shift buffer
		else {
			printf("  SHIFT: [%d..%d] -> [0..%d]\n", i, n, preserve);
			copy(&buff[p], &buff[n], &buff[0]);
		}
		
		n = preserve;
		bi += p;
		i -= p;
	}
	
	// Skip characters
	if (skip > 0){
		printf("  SKIP: %d\n", skip);
		in->seekg(skip, ios::cur);
		n = 0;
		bi += i;
		i = 0;
	}
	
	// Use all remaining space
	if (fill && (n + count + 1) < _buffSize){
		count = _buffSize - n - 1;
		printf("  count:%d\n", count);
	}
	
	// Read data to empty space in buffer
	if (count > 0){
		in->read(&buff[n], count);
		count = in->gcount();
		printf("  IO: %d\n", count);
		n += count;
		buff[n] = 0;
	} else {
		count = 0;
	}
	
	eof = (count == 0);
	return count;
}

// ------------------------------------------------------------------------------------------ //