#include "csg_parser.hpp"

#include <ctype.h>
#include <iostream>


using namespace std;
using namespace csg;


// ----------------------------------- [ Functions ] ---------------------------------------- //


void Parser::parse(istream& in){
	if (in.bad() || buffSize < 1){
		return;
	}
	
	// Reset
	this->in = &in;
	reset();
	
	
	skipWhiteSpace();
	
	if (next() != 0){
		cout << buff[i];
	}
	
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


void Parser::skipWhiteSpace(){
	while (true){
		if (next() != 0 && isspace(buff[i])){
			if (buff[i] == '\n')
				li++;
			i++;
		} else {
			return;
		}
	}
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


void Parser::reset(){
	i = 0;
	n = 0;
	li = 0;
	gi = 0;
	eof = false;
	if (buff == nullptr)
		buff = new char[buffSize];
	buff[0] = 0;
}


bool Parser::fillBuffer(){
	in->read(buff, buffSize);
	
	i = 0;
	n = in->gcount();
	gi += n;
	buff[n] = 0;
	eof = (n != 0);
	
	return eof;
}


// ------------------------------------------------------------------------------------------ //