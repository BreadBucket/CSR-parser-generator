#include "csg_parser.hpp"

#include <ctype.h>
#include <iostream>


using namespace std;
using namespace csg;


// ----------------------------------- [ Functions ] ---------------------------------------- //


void printSrc(const RuleSource& src){
	printf("[%d,%d): ", src.pos.start, src.pos.end);
	printf("%d:%d-", src.row.start, src.row.end);
	printf("%d:%d  ", src.col.start, src.col.end);
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


// ----------------------------------- [ Functions ] ---------------------------------------- //


void Parser::parse(istream& in){
	buffSize = 6;
	
	if (in.bad() || buffSize < 1){
		return;
	}
	
	// Reset
	this->in = &in;
	reset();
	
	parseRule();
}


void Parser::parseRule(){
	vector<RuleSource> tokens = vector<RuleSource>(8);
	
	while (true){
		skipWhiteSpace();
		
		RuleSource& token = tokens.emplace_back();
		if (!parseLeftToken(token)){
			tokens.pop_back();
			break;
		}
		
	}
	
	if (!match("->", 2))
		printf("SADGE\n");
	
}


bool Parser::parseLeftToken(RuleSource& src){
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


void Parser::startSrc(RuleSource& rec){
	rec.pos.start = gi();
	rec.row.start = ri;
	rec.col.start = gi() - ci;
	rec.str.clear();
}


void Parser::endSrc(RuleSource& rec){
	rec.pos.end = gi();
	rec.row.end = ri;
	rec.col.end = gi() - ci;
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


void Parser::reset(){
	i = 0;
	bi = 0;
	ci = 0;
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
			
			// Move to beginning of buffer
			else if (i > 0){
				copy(buff + i, buff + n, buff);
			}
			
			bi += i;
			i = 0;
			n = chars;
			space = buffSize - n - 1;
		}
		
		// Fill buffer
		in->read(buff + n, space);
		n += in->gcount();
		buff[n] = 0;
		eof = (in->gcount() == 0);
		chars = n - i;
	}
	
	return chars;
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


// ------------------------------------------------------------------------------------------ //