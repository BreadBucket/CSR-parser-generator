#include <stdio.h>

#include "../../obj/examples/example-1.h"


// Input: AABBCC
// Expected output: 
/*
Stack: [S]
*/


// ------------------------------------[ Variables ] ---------------------------------------- //


DFA dfa;


CSRTokenID input[] = {
	TOKEN_A,
	TOKEN_A,
	TOKEN_B,
	TOKEN_B,
	TOKEN_C,
	TOKEN_C
};


const int input_n = sizeof(input)/sizeof(*input);
int input_i = 0;


// ----------------------------------- [ Functions ] ---------------------------------------- //


CSRToken* _nextToken(){
	if (input_i < input_n){
		CSRTokenID id = input[input_i++];
		return CSRToken_create(id);
	}
	return NULL;
}


// --------------------------------- [ Main Function ] -------------------------------------- //


int main(int argc, char const* const* argv){
	int result;
	
	// Initialize DFA
	DFA_init(&dfa);
	dfa.getNextToken = _nextToken;
	
	// Execute DFA
	while (DFA_step(&dfa));
	
	if (dfa.tokenStack.count == 1 && ((CSRToken*)dfa.tokenStack.v[0])->id == TOKEN_S){
		printf("Parsing completed.\n");
		result = 0;
	} else {
		fprintf(stderr, "Syntax error.\n");
		result = 1;
	}
	
	// Cleanup
	DFA_deinit(&dfa);
	return result;
}


// ------------------------------------------------------------------------------------------ //