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


inline bool isFirstIdChar(char c){
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


// ---------------------------------- [ Structures ] ---------------------------------------- //





// ----------------------------------- [ Functions ] ---------------------------------------- //


void Parser::parse(istream& in){
	buffSize = 6;
	
	if (in.bad() || buffSize < 1){
		return;
	}
	
	// Reset
	this->in = &in;
	reset();
	
	// TEMP
	parseRule();
	parseRule();
	parseRule();
}


void Parser::parseRule(){
	Rule rule = Rule();
	
	// Parse left symbols
	while (true){
		skipWhiteSpace();
		
		Symbol& sym = rule.left.emplace_back();
		if (!parseId(sym.name)){
			rule.left.pop_back();
			break;
		}
		
	}
	
	// Arrow
	if (!match("->", 2)){
		if (isIdChar(ch()))
			throw ParserException(getLoc(), "Symbols must start with a capital letter.");
		else
			throw ParserException(getLoc(), "Unexpected character. Expected side separator \"->\".");
	}
	
	// Parse right symbols
	while (true){
		skipWhiteSpace();
		
		Symbol& sym = rule.right.emplace_back();
		if (!parseId(sym.name)){
			rule.right.pop_back();
			break;
		}
		
	}
	
	
}


bool Parser::parseId(SourceString& str){
	char c = ch();
	if (!isFirstIdChar(c))
		return false;
	
	str.clear();
	str.start = getLoc();
	
	// Read name
	while (isIdChar(c)){
		str.push_back(c);
		i++;
		c = ch();
	}
	
	str.end = getLoc();
	return true;
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