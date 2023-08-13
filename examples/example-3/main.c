#include <string.h>
#include <stdio.h>

#include "../../obj/examples/example-3.h"

// Input: AABBCC
// Expected output:
/*
AST:
 S:
  A:
   A
   A
   A
  B:
   B
   B
   B
  C:
   C
   C
   C
*/


// ------------------------------------[ Variables ] ---------------------------------------- //


DFA dfa;


CSRTokenID input[] = {
	TOKEN_A,
	TOKEN_A,
	TOKEN_A,
	TOKEN_B,
	TOKEN_B,
	TOKEN_B,
	TOKEN_C,
	TOKEN_C,
	TOKEN_C
};


const int input_n = sizeof(input)/sizeof(*input);
int input_i = 0;


// ----------------------------------- [ Functions ] ---------------------------------------- //


CSRToken* _nextToken(DFA*){
	if (input_i < input_n)
		return CSRToken_create(input[input_i++]);
	return NULL;
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


void loopPrint(const char* s, int n){
	while (n-- > 0)
		printf("%s", s);
}


void printToken(CSRToken* token){
	const char* name = NULL;
	if (token != NULL)
		name = CSRToken_getName(token->id);
	if (name != NULL)
		printf("%s", name);
	else
		printf("NULL");
}


void printTokenTree(CSRToken* token, int lvl){
	loopPrint(" ", lvl);
	printToken(token);
	
	if (token == NULL || token->childCount <= 0)
		return;
	
	lvl++;
	printf(":");
	for (int i = 0 ; i < token->childCount ; i++){
		printf("\n");
		printTokenTree(token->children[i], lvl);
	}
	
}


// --------------------------------- [ Main Function ] -------------------------------------- //


int main(int argc, char const* const* argv){
	// Initialize DFA
	DFA_init(&dfa);
	dfa.getNextToken = _nextToken;
	
	// Execute DFA
	while (DFA_step(&dfa));
	
	// Verify parse
	if (dfa.tokenStack.count != 1 || ((CSRToken*)dfa.tokenStack.v[0])->id != TOKEN_S){
		fprintf(stderr, "Syntax error.\n");
		DFA_deinit(&dfa);
		return 1;
	}
	
	// Print token stack/AST
	printf("AST:\n");
	for (int i = 0 ; i < dfa.tokenStack.count ; i++){
		CSRToken* token = (CSRToken*)dfa.tokenStack.v[i];
		printTokenTree(token, 1);
		printf("\n");
	}
	
	// Cleanup
	DFA_deinit(&dfa);
	return 0;
}


// ------------------------------------------------------------------------------------------ //