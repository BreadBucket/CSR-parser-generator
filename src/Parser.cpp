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
	printf("[%d,%d): ", src.start.i, src.end.i);
	printf("%d:%d-", src.start.row+1, src.start.col+1);
	printf("%d:%d  ", src.end.row+1, src.end.col+1);
	if (!src.empty())
		printf("\"%s\"", src.c_str());
	else
		printf("null");
	printf("\n");
}


// DEBUG
std::string locStr(const csg::Location& loc){
	char s[100];
	snprintf(s, 100, "[%d]: %d:%d", loc.i, loc.row+1, loc.col+1);
	return std::string(s);
}


//DEBUG
void Parser::printch(){
	printf(ANSI_CYAN);
	if (ch() == '\n')
		printf("%s -- '\\n'", locStr(getLoc()).c_str());
	else if (ch() == '\t')
		printf("%s -- '\\t'", locStr(getLoc()).c_str());
	else
		printf("%s -- '%c'", locStr(getLoc()).c_str(), ch());
	printf(ANSI_RESET "\n");
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


inline bool isSymbolNameFirstChar(char c){
	return ('A' <= c && c <= 'Z');
}


inline bool isIdChar(char c){
	return (
		('a' <= c && c <= 'z') ||
		('A' <= c && c <= 'Z') ||
		('0' <= c && c <= '9') ||
		(c == '_')
	);
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


void Parser::parse(istream& in){
	buffSize = 6;
	
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
	
	
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


void Parser::parseReduction(Reduction& r){
	// Parse left symbols
	if (isIdChar(ch())){
		while (isIdChar(ch())){
			Symbol& sym = r.left.emplace_back();
			parseSymbol(sym);
			skipSpace(true);
		}
	} else {
		throw ParserException(getLoc(), "Expected symbol name.");
	}
	
	if (r.left.size() <= 0){
		throw ParserException(getLoc(), "Missing left side of reduction.");
	}
	
	
	// Arrow
	if (!match("->")){
		if (ch() == '[')
			throw ParserException(getLoc(), "Symbol attributes missing symbol name.");
		else
			throw ParserException(getLoc(), "Expected left-right separator \"->\".");
	}
	
	skipSpace(true);
	
	
	// Parse right symbols
	while (isIdChar(ch())){
		Symbol& sym = r.right.emplace_back();
		parseSymbol(sym);
		skipSpace(true);
	}
	
	// Parse inline code
	if (ch() == '{'){
		parseReductionInlineCode(r.code);
		skipSpace(true);
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
	if (!isSymbolNameFirstChar(ch())){
		throw ParserException(getLoc(), "Symbols must start with a capital letter.");
	}
	
	// Parse symbol name
	sym.clear();
	parseId(sym.name);
	
	// Parse additional attributes
	if (ch() == '['){
		parseSymbolAttributes(sym);
	}
	
}


void Parser::parseSymbolAttributes(Symbol& sym){
	if (ch() != '['){
		throw ParserException(getLoc(), "Expected '[' when declaring symbol attributes.");
	}
	
	inc();
	skipSpace(true);
	
	if (isIdChar(ch())){
		parseId(sym.id);
	} else {
		throw ParserException(getLoc(), "Unexpected character.");
	}
	
	skipSpace(true);
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
	
	str.end = getLoc() - 1;
}


void Parser::parseReductionInlineCode(SourceString& code){
	if (ch() != '{'){
		throw ParserException(getLoc(), "Expected '{' at reduction inline code declaration.");
	}
	
	code.clear();
	code.start = getLoc();
	
	code.push_back('{');
	inc();
	
	int bracket = 1;
	int quote_1 = 0;
	int quote_2 = 0;
	while (bracket > 0){
		char c = ch();
		
		if (c == 0){
			break;
		} else if (c == '\n'){
			nl();
			inc(-1);
		} else if (c == '\t'){
			tab();
			inc(-1);
		}
		
		// String context
		else if (quote_1 > 0){
			if (c == '"')
				quote_1--;
			else if (c == '\\')
				inc();
		}
		
		// Literal context
		else if (quote_2 > 0){
			if (c == '\'')
				quote_2--;
			else if (c == '\\')
				inc();
		}
		
		// Default context
		else {
			if (c == '{')
				bracket++;
			else if (c == '}'){
				bracket--;
			}
			else if (c == '"')
				quote_1++;
			else if (c == '\'')
				quote_1++;
		}
		
		code.push_back(c);
		inc();
	}
	
	if (bracket > 0){
		throw ParserException(code.start, "Missing closing bracket '}'.");
	}
	
	code.end = getLoc() - 1;
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


void Parser::skipWhiteSpace(){
	while (true){
		char c = ch();
		if (c == '\n'){
			nl();
		} else if (c == '\t'){
			tab();
		} else if (isspace(c)){
			inc();
		} else {
			return;
		}
	}
}


void Parser::skipSpace(bool escapeable){
	while (true){
		char c = ch();
		
		// Regular whitespace
		if (c == '\t'){
			tab();
		} else if (c == '\n'){
			break;
		} else if (isspace(c)){
			inc();
		}
		
		// Escaped whitespace
		else if (c == '\\' && escapeable){
			lookAhead(2);
			
			if (buff[i+1] == '\n'){
				inc();
				nl();
			} else if (buff[i+1] == '\t'){
				inc();
				tab();
			} else if (isspace(buff[i+1])){
				inc(2);
			} else {
				break;
			}
			
		}
		
		// Solid character
		else {
			break;
		}
		
	}
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


bool Parser::match(const char* s, bool move){
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
	
	return true;
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


void Parser::reset(){
	eof = false;
	i = 0;
	bi = 0;
	ci = 0;
	ri = 0;
	n = 0;
	if (buff == nullptr)
		buff = new char[buffSize];
	buff[0] = 0;
}


bool Parser::fillBuffer(){
	i = 0;
	bi += n;
	
	in->read(buff, buffSize-1);
	n = in->gcount();
	
	buff[n] = 0;
	eof = (n == 0);
	return !eof;
}


int Parser::lookAhead(int count){
	int chars = n - i;
	
	// Fill buffer
	if (chars < count){
		int space = buffSize - n - 1;
		
		// Not enough space in buffer
		if ((count - chars) > space){
			
			// Allocate bigger buffer
			if (count >= buffSize){
				buffSize = count + 1;
				char* _buff = new char[buffSize];
				copy(buff + i, buff + n, _buff);
				swap(_buff, buff);
				delete _buff;
			}
			
			// Move leftover chars (right side) to beginning of buffer (left side)
			else if (i > 0){
				copy(buff + i, buff + n, buff);
			}
			
			bi += i;
			i = 0;
			n = chars;
			space = buffSize - n - 1;
		}
		
		// Fill empty space in buffer (right side) 
		in->read(buff + n, space);
		n += in->gcount();
		buff[n] = 0;
		eof = (in->gcount() == 0);
		chars = n - i;
	}
	
	return chars;
}


// ------------------------------------------------------------------------------------------ //