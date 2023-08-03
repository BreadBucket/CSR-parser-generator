#include <stdio.h>
#include <stdlib.h>

#include "../../obj/switch.c"


// ------------------------------------[ Variables ] ---------------------------------------- //


CSRTokenID tokenInput[] = {
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


CSRToken* token_a(){
	return createToken(&dfa, TOKEN_A, 0);
}

CSRToken* token_b(){
	return createToken(&dfa, TOKEN_B, 0);
}

CSRToken* token_c(){
	return createToken(&dfa, TOKEN_C, 0);
}


CSRToken* _generateToken(){
	static int i = 0;
	CSRToken* t = NULL;
	
	if (i < tokenInput_size){
		t = createToken(&dfa, tokenInput[i++], 0);
	}
	
	return t;
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


CSRToken* _onTokenCreate(DFA* dfa, CSRToken* t){
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
		if (((CSRToken*)dfa.tokenStack.v[i])->data == NULL)
			printf("%d ", ((CSRToken*)dfa.tokenStack.v[i])->id);
		else
			printf("%s ", ((CSRToken*)dfa.tokenStack.v[i])->data);
	}
	
	printf("| ");
	
	for (int i = dfa.tokenBuffer.count - 1 ; i >= 0 ; i--){
		if (((CSRToken*)dfa.tokenBuffer.v[i])->data == NULL)
			printf("%d ", ((CSRToken*)dfa.tokenBuffer.v[i])->id);
		else
			printf("%s ", ((CSRToken*)dfa.tokenBuffer.v[i])->data);
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