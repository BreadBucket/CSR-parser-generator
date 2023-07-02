#include "Parser.hpp"

#include <iostream>


using namespace std;
using namespace csg;

using ErrorCode = csg::Parser::ErrorCode;


// ----------------------------------- [ Functions ] ---------------------------------------- //


// // DEBUG
// void printSrc(const Source& src){
// 	printf("[%d,%d): ", src.loc.start.i, src.loc.end.i);
// 	printf("%d:%d-", src.loc.start.row, src.loc.end.row);
// 	printf("%d:%d  ", src.loc.start.col, src.loc.end.col);
// 	if (!src.str.empty())
// 		printf("\"%s\"", src.str.c_str());
// 	else
// 		printf("null");
// 	printf("\n");
// }


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
	
	
	skipWhiteSpace();
	
	
	// Is a reduction
	if (isIdChar(ch())){
		parseReduction();
	}
	
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


void Parser::parseReduction(){
	Reduction r = Reduction();
	
	
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
	if (!match("->", 2)){
		if (ch() == '[')
			throw ParserException(getLoc(), "Symbol attributes missing symbol name.");
		else
			throw ParserException(getLoc(), "Expected left-right separator \"->\".");
	}
	
	skipSpace(true);
	
	
	// Parse right symbols
	if (isIdChar(ch())){
		while (isIdChar(ch())){
			Symbol& sym = r.right.emplace_back();
			parseSymbol(sym);
			skipSpace(true);
		}
	} else {
		throw ParserException(getLoc(), "Expected symbol name.");
	}
	
	if (r.right.size() <= 0){
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
	
	i++;
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
	
	i++;
}


void Parser::parseId(SourceString& str){
	str.clear();
	str.start = getLoc();
	
	char c = ch();
	while (isIdChar(c)){
		str.push_back(c);
		i++;
		c = ch();
	}
	
	str.end = getLoc();
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


void Parser::skipWhiteSpace(){
	while (true){
		if (isspace(ch())){
			if (buff[i] == '\n')
				nl();
			i++;
		} else {
			return;
		}
	}
}


void Parser::skipSpace(bool escapeable){
	while (true){
		char c = ch();
		
		if (isspace(c) && c != '\n'){
			i++;
		}
		
		else if (c == '\\' && escapeable){
			lookAhead(2);
			if (isspace(buff[i+1]))
				i += 2;
			else
				break;
		}
		
		else {
			break;
		}
		
	}
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


bool Parser::match(const char* s, int n){
	if (lookAhead(n) < n){
		return false;
	}
	
	for (int ii = 0 ; ii < n ; ii++){
		if (buff[i+ii] != s[ii])
			return false;
	}
	
	i += n;
	return true;
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


void Parser::reset(){
	i = 0;
	bi = 0;
	ci0 = 0;
	ri = 0;
	eof = false;
	if (buff == nullptr)
		buff = new char[buffSize];
	n = 0;
	buff[0] = 0;
}


bool Parser::fillBuffer(){
	in->read(buff, buffSize-1);
	
	i = 0;
	bi += n;
	n = in->gcount();
	buff[n] = 0;
	eof = (n == 0);
	
	return !eof;
}


void Parser::nl(){
	ri++;
	ci0 = gi() + 1;
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
				char* _buff = buff;
				buff = new char[buffSize];
				copy(_buff + i, _buff + n, buff);
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