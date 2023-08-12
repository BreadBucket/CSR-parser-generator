#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "ANSI.h"
#include "../obj/test/test.h"


// ------------------------------------[ Variables ] ---------------------------------------- //


DFA dfa;


CSRToken** input;
int input_i;
int input_n;


// ----------------------------------- [ Functions ] ---------------------------------------- //


void generateInput(){
	input_n = 6;
	input = malloc(sizeof(*input) * (input_n + 1));
	
	input_i = 0;
	input[input_i++] = DFA_createToken(&dfa, TOKEN_A, 0);
	input[input_i++] = DFA_createToken(&dfa, TOKEN_A, 0);
	input[input_i++] = DFA_createToken(&dfa, TOKEN_B, 0);
	input[input_i++] = DFA_createToken(&dfa, TOKEN_B, 0);
	input[input_i++] = DFA_createToken(&dfa, TOKEN_C, 0);
	input[input_i++] = DFA_createToken(&dfa, TOKEN_C, 0);
	input[input_i++] = NULL;
	input_i = 0;
	
}


CSRToken* _nextToken(){
	if (input_i < input_n)
		return input[input_i++];
	return NULL;
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


void loopPrint(const char* s, int n){
	while (n > 0){
		printf("%s", s);
		n--;
	}
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
	loopPrint("  ", lvl);
	printToken(token);
	
	lvl++;
	if (token == NULL || token->childCount <= 0)
		return;
	
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
	} else if (input_i < input_n){
		printf(ANSI_CYAN);
		printToken(input[input_i]);
		printf(" ");
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


// ----------------------------------- [ Functions ] ---------------------------------------- //


CSRToken* _onTokenCreate(DFA* dfa, CSRToken* t){
	printf(ANSI_GREEN "CREATE: " ANSI_RESET); printToken(t); printf("\n");
	return t;
}


bool _onTokenDestroy(DFA* dfa, CSRToken* t){
	printf(ANSI_RED "DESTROY: " ANSI_RESET); printToken(t); printf("\n");
	return true;
}


// -------------------------------- [ Main Functions ] -------------------------------------- //


int main(int argc, char const* const* argv){
	printf("================================\n");
	
	DFA_init(&dfa);
	dfa.getNextToken = _nextToken;
	dfa.onTokenCreate = _onTokenCreate;
	dfa.onTokenDelete = _onTokenDestroy;
	
	generateInput();
	
	
	for (int i = 0 ; i < 20 ; i++){
		printState();
		if (!DFA_step(&dfa)){
			printf("HALT\n");
			break;
		}
	}
	
	
	printf("\n");
	printf("TREE:\n");
	printTokenTree(Stack_peek(&dfa.tokenStack), 1);
	printf("\n\n");
	
	
	DFA_deinit(&dfa);
	printf("================================\n");
	return 0;
}


// ------------------------------------------------------------------------------------------ //