#include "Parser.hpp"

#include <cstring>
#include <iostream>
#include "util/ANSI.h"
#include "util/utils.hpp"

using namespace std;
using namespace csg;


// ----------------------------------- [ Functions ] ---------------------------------------- //


// DEBUG
void printSrc(const SourceString& src){
	printf("[%d,%d): @", src.start.i, src.end.i);
	if (src.start.valid())
		printf("%d:%d", src.start.row+1, src.start.col+1);
	if (src.end.valid())
		printf("-%d:%d", src.end.row+1, src.end.col+1);
	printf("  ");
	if (!src.empty())
		printf("\"" ANSI_GREEN "%s" ANSI_RESET "\"", src.c_str());
	else
		printf("null");
	printf("\n");
}


// DEBUG
char* locStr(const csg::Location& loc){
	char* s = new char[100];
	snprintf(s, 100, "[%d]: %d:%d", loc.i, loc.row+1, loc.col+1);
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
	buffSize = 4;
	
	if (in.bad() || buffSize < 1){
		return;
	}
	
	// Reset
	this->in = &in;
	reset();
	
	
	vector<Reduction> reductions;
	vector<SourceString> code;
	reductions.reserve(32);
	code.reserve(32);
	
	
	SourceString tmp;
	while (true){
		tmp.start = getLoc();
		parseWhiteSpace(tmp, true);
		
		char c = ch();
		
		if (c == '\n'){
			nl();
		} else if (c == '#'){
			
			SourceString cond;
			MacroType m = parseMacro(tmp, &cond);
			
			printSrc(tmp);
			printf("%d: {" ANSI_RED "%s" ANSI_RESET "}\n", m, cond.c_str());
			printf("\n");
			
			code.push_back(move(tmp));
		} else if (isIdChar(c)){
			Reduction r;
			inc(); //
			// parseReduction(r);
		} else if (c == 0){
			break;
		} else {
			throw ParserException(getLoc(), "Unexpected character.");
		}
		
		tmp.clear();
	}
	
	
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


Parser::MacroType Parser::parseMacro(SourceString& s, SourceString* condition){
	if (s.size() == 0 && !s.start.valid()){
		s.start = getLoc();
	}
	
	parseWhiteSpace(s);
	
	// Parse initial # symbol
	if (ch() == '#'){
		s.push_back('#');
		inc();
	} else {
		throw ParserException(getLoc(), "Preprocessor directive declaration expected.");
	}
	
	parseWhiteSpace(s);
	
	
	// Parse macro type
	MacroType type = MacroType::UNKNOWN;
	if (match("ifndef", true, &s)){
		type = MacroType::IFNDEF;
	} else if (match("ifdef", true, &s)){
		type = MacroType::IFDEF;
	} else if (match("if", true, &s)){
		type = MacroType::IF;
	} else if (match("elifndef", true, &s)){
		type = MacroType::ELIFNDEF;
	} else if (match("elifdef", true, &s)){
		type = MacroType::ELIFDEF;
	} else if (match("elif", true, &s)){
		type = MacroType::ELIF;
	} else if (match("else", true, &s)){
		type = MacroType::ELSE;
	} else if (match("endif", true, &s)){
		type = MacroType::ENDIF;
	}
	
	// Space after macro type
	if (type != MacroType::UNKNOWN && parseWhiteSpace(s) < 1){
		type = MacroType::UNKNOWN;
	}
	
	
	// Conditional
	if (condition != nullptr && type != MacroType::UNKNOWN){
		while (true){
			char c = ch();
			
			if (BETWEEN(c, 'a', 'z') || BETWEEN(c, 'A', 'Z') || BETWEEN(c, '0', '9') || c == '_'){
				s.push_back(c);
				condition->push_back(c);
				inc();
			} else {
				break;
			}
			
		}
	}
	
	
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
	
	
	s.end = getLoc();
	return type;
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
			lookAhead(2);
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
			lookAhead(2);
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
		lookAhead(len);
		
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
		_buffSize = buffSize;
		buff = new char[_buffSize];
	}
	buff[0] = 0;
	
	trash.clear();
	frames.clear();
}


int Parser::fillBuffer(int count, bool fill){
	if (count <= 0){
		return 0;
	}
	
	// Frame global index must be converted to local index
	const int p = (!frames.empty()) ? (frames[0].i - bi) : i;
	const int preserve = n - p;
	
	int space = _buffSize - n - 1;
	count = (fill & count < space) ? space : count;
	
	const int requiredSize = preserve + count + 1;
	
	
	// Reorganize buffer
	if (count > space){
		
		// Create larger buffer
		if (requiredSize > _buffSize){
			_buffSize = buffSize * ((requiredSize + buffSize)/buffSize) + 1;	// (multiple of buffSize) + 1
			char* _buff = new char[_buffSize];
			
			if (preserve > 0){
				copy(&buff[p], &buff[n], &_buff[0]);
			}
			
			swap(buff, _buff);
			delete[] _buff;
		}
		
		// Shift data in current buffer to the left
		else if (preserve > 0){
			copy(&buff[p], &buff[n], &buff[0]);
		}
		
		// Adjust indexes
		i -= p;
		n = max(0, preserve);
		bi += p;
		
		// Recalculate available space
		space = _buffSize - n - 1;
		if (fill & count < space){
			count = space;
		}
		
	}
	
	
	// Read data to empty space in buffer
	in->read(&buff[n], count);
	count = in->gcount();
	n += count;
	buff[n] = 0;
	
	eof = (count == 0);
	return count;
}


int Parser::lookAhead(int count){
	int chars = n - i;
	if (chars < count)
		chars += fillBuffer(count - chars);
	return chars;
}

// ------------------------------------------------------------------------------------------ //