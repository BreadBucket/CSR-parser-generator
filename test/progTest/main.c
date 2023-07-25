#include <stdio.h>
#include <stdlib.h>

#include "gen.c"
#include "Switch.c"


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


// ----------------------------------- [ Functions ] ---------------------------------------- //


Token* nextToken(){
	static int i = 0;
	Token* t = NULL;
	
	if (i < tokenInput_size){
		t = malloc(sizeof(Token));
		t->id = tokenInput[i++];
		t->childCount = 0;
		t->children = NULL;
		t->data = NULL;
	}
	
	return t;
}


// -------------------------------- [ Main Functions ] -------------------------------------- //


int main(int argc, char const* const* argv){
	printf("================================\n");
	
	
	printf("================================\n");
	return 0;
}


// ------------------------------------------------------------------------------------------ //