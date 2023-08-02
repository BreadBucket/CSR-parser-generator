#include <stdio.h>
#include <stdlib.h>

#include "../../obj/switch.c"


// ------------------------------------[ Variables ] ---------------------------------------- //


TokenID tokenInput[] = {
	TOKEN_A,
	TOKEN_A,
	TOKEN_B,
	TOKEN_B,
	TOKEN_C,
	TOKEN_C
};


const int tokenInput_size = sizeof(tokenInput)/sizeof(*tokenInput);


DFA dfa;


// ----------------------------------- [ Functions ] ---------------------------------------- //


Token* token_a(){
	return createToken(&dfa, TOKEN_A, 0);
}

Token* token_b(){
	return createToken(&dfa, TOKEN_B, 0);
}

Token* token_c(){
	return createToken(&dfa, TOKEN_C, 0);
}


Token* _generateToken(){
	static int i = 0;
	Token* t = NULL;
	
	if (i < tokenInput_size){
		t = createToken(&dfa, tokenInput[i++], 0);
	}
	
	return t;
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


Token* _onTokenCreate(DFA* dfa, Token* t){
	switch (t->id){
		case TOKEN_A:	t->data = "A"; break;
		case TOKEN_B:	t->data = "B"; break;
		case TOKEN_C:	t->data = "C"; break;
		case TOKEN_S:	t->data = "S"; break;
		case TOKEN_Q:	t->data = "Q"; break;
		default:
			char* s = malloc(sizeof(*s) * 32);
			snprintf(s, 32, "%d", t->id);
			t->data = s;
			break;
	}
	return t;
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


void printStack(){
	
	for (int i = 0 ; i < dfa.tokenStack.count ; i++){
		if (((Token*)dfa.tokenStack.v[i])->data == NULL)
			printf("%d ", ((Token*)dfa.tokenStack.v[i])->id);
		else
			printf("%s ", ((Token*)dfa.tokenStack.v[i])->data);
	}
	
	printf("| ");
	
	for (int i = dfa.tokenBuffer.count - 1 ; i >= 0 ; i--){
		if (((Token*)dfa.tokenBuffer.v[i])->data == NULL)
			printf("%d ", ((Token*)dfa.tokenBuffer.v[i])->id);
		else
			printf("%s ", ((Token*)dfa.tokenBuffer.v[i])->data);
	}
	
}


void f(){
	printf("[%d]:  ", dfa.currentStateId);
	printf("STACK: %d/%d,  ", dfa.tokenStack.count, dfa.tokenStack.size);
	printf("BUFFER: %d/%d,    {", dfa.tokenBuffer.count, dfa.tokenBuffer.size);
	printStack();
	printf("}\n");
}


// -------------------------------- [ Main Functions ] -------------------------------------- //


int main(int argc, char const* const* argv){
	printf("================================\n");
	DFA_init(&dfa);
	dfa.getNextToken = _generateToken;
	dfa.onTokenCreate = _onTokenCreate;
	
	
	for (int i = 0 ; i < 20 ; i++){
		f();
		if (!DFA_step(&dfa)){
			printf("HALT\n");
			break;
		}
	}
	f();
	
	
	printf("================================\n");
	return 0;
}


// ------------------------------------------------------------------------------------------ //