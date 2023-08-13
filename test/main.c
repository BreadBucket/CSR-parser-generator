#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "ANSI.h"
#include "../obj/test/test.h"


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


CSRToken* _nextToken(struct _DFA*){
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


// ----------------------------------- [ Functions ] ---------------------------------------- //


void printStack(){
	printf(ANSI_GREEN);
	
	for (int i = 0 ; i < dfa.tokenStack.count ; i++){
		printToken(((CSRToken*)dfa.tokenStack.v[i]));
		printf(" ");
	}
	
	printf(ANSI_RESET "| ");
	
	if (dfa.tokenBuffer.count > 0){
		printf(ANSI_YELLOW);
		for (int i = dfa.tokenBuffer.count - 1 ; i >= 0 ; i--){
			printToken(((CSRToken*)dfa.tokenBuffer.v[i]));
			printf(" ");
		}
	}
	
	if (input_i < input_n){
		printf(ANSI_CYAN);
		for (int i = input_i ; i < input_n ; i++)
			printf("%s ", CSRToken_getName(input[i]));
		printf(ANSI_RESET);
	}
	
	printf(ANSI_RESET);
}


void printState(){
	printf("[%d]:  ", dfa.currentStateId);
	printf("STACK: %2d/%-2d  ", dfa.tokenStack.count, dfa.tokenStack.size);
	printf("BUFF: %2d/%-2d    {", dfa.tokenBuffer.count, dfa.tokenBuffer.size);
	printStack();
	printf("}\n");
}


void printDetailStack(){
	printf("TOKENS:\n");
	for (int i = 0 ; i < dfa.tokenStack.count ; i++){
		CSRToken* t = dfa.tokenStack.v[i];
		printf("  [%d]: %s (%p) ref:%d\n", i, CSRToken_getName(t->id), t, t->refCount);
	}
	
	if (dfa.tokenBuffer.count > 0){
		printf("--------\n");
		for (int i = 0 ; i < dfa.tokenBuffer.count ; i++){
			CSRToken* t = dfa.tokenBuffer.v[i];
			printf("  [%d]: %s (%p) ref:%d\n", i, CSRToken_getName(t->id), t, t->refCount);
		}
	}
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


CSRToken* _onTokenCreate(DFA* dfa, CSRToken* t){
	// printf(ANSI_GREEN "CREATE: " ANSI_RESET);
	// printToken(t);
	// printf(" (%p)\n", t);
	return t;
}


bool _onTokenDestroy(DFA* dfa, CSRToken* t){
	// printf(ANSI_RED "DESTROY: " ANSI_RESET);
	// printToken(t);
	// printf(" (%p)\n", t);
	return true;
}


// -------------------------------- [ Main Functions ] -------------------------------------- //


int main(int argc, char const* const* argv){
	printf("================================\n");
	
	DFA_init(&dfa);
	dfa.getNextToken = _nextToken;
	dfa.onTokenCreate = _onTokenCreate;
	dfa.onTokenDelete = _onTokenDestroy;
	
	
	for (int i = 0 ; i < 20 ; i++){
		printState();
		if (!DFA_step(&dfa)){
			printf("HALT\n");
			break;
		}
	}
	
	
	printf("\n");
	printf("AST:\n");
	for (int i = 0 ; i < dfa.tokenStack.count ; i++){
		printTokenTree((CSRToken*)dfa.tokenStack.v[i], 1);
		printf("\n");
	}
	printf("\n");
	
	
	DFA_deinit(&dfa);
	printf("================================\n");
	return 0;
}


// ------------------------------------------------------------------------------------------ //