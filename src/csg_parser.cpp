#include "csg_parser.hpp"

#include <ctype.h>
#include <iostream>


using namespace std;
using namespace csg;


// ----------------------------------- [ Functions ] ---------------------------------------- //


// DEBUG
void printSrc(const Source& src){
	printf("[%d,%d): ", src.loc.start.i, src.loc.end.i);
	printf("%d:%d-", src.loc.start.row, src.loc.end.row);
	printf("%d:%d  ", src.loc.start.col, src.loc.end.col);
	if (!src.str.empty())
		printf("\"%s\"", src.str.c_str());
	else
		printf("null");
	printf("\n");
}


inline bool isSymbolChar(char c){
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
}


void Parser::parseRule(){
	vector<Source> tokens = vector<Source>(8);
	
	// Parse left symbols
	while (true){
		skipWhiteSpace();
		
		Source& token = tokens.emplace_back();
		if (!parseLeftSymbol(token)){
			tokens.pop_back();
			break;
		}
		
	}
	
	if (!match("->", 2))
		printf("SADGE\n");
	
}


bool Parser::parseLeftSymbol(Source& src){
	char c = ch();
	
	// Starts with capital ASCII letter
	if (c < 'A' || 'Z' < c)
		return false;
	
	// Read token
	startSrc(src);
	while (isSymbolChar(c)){
		src.str.push_back(c);
		i++;
		c = ch();
	}
	endSrc(src);
	
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


void Parser::startSrc(Source& rec){
	rec.loc.start = getLoc();
	rec.loc.end = rec.loc.start;
	rec.str.clear();
}


void Parser::endSrc(Source& rec){
	rec.loc.end = getLoc();
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