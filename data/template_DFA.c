#include "TokenEnum.h"

#include <stdbool.h>
#include <string.h>


// ---------------------------------- [ Definitions ] --------------------------------------- //


#ifndef TOKENSTACK_RESIZERULE
	#define TOKENSTACK_RESIZERULE(n)	((n)*2)
#endif


// ---------------------------------- [ Structures ] ---------------------------------------- //


typedef struct _Token {
	TokenID id;
	int childCount;				// Size of the children array (excluding null terminator).
	struct _Token** children;	// Null-terminated array of child pointers.
	void* data;					// Additional user data.
} Token;


typedef struct {
	int i;
	int count;
	int size;
	Token** v;
} TokenStack;


typedef int StateID;


// ------------------------------------[ Variables ] ---------------------------------------- //


TokenStack buffer;
TokenStack backlog;

StateID currentStateId;


// ----------------------------------- [ Prototypes ] --------------------------------------- //


Token* nextToken();


// ----------------------------------- [ Functions ] ---------------------------------------- //


void TokenStack_resize(TokenStack* buffer, int newSize){
	if (newSize > buffer->size){
		const size_t bytes = newSize * sizeof(*buffer->v);
		buffer->v = realloc(buffer->v, bytes);
	}
}


bool TokenStack_push(TokenStack* buffer, Token* token){
	const int count = buffer->count;
	
	if (count >= buffer->size){
		TokenStack_rezerve(buffer, TOKENSTACK_RESIZERULE(buffer->size));
		if (count >= buffer->size)
			return false;
	}
	
	buffer->v[count] = token;
	buffer->count++;
}


Token* TokenStack_pop(TokenStack* buffer){
	const int i = buffer->count--;
	
	if (i >= 0){
		return buffer->v[i];
	} else {
		buffer->count++;
		return NULL;
	}
	
}


// ----------------------------------- [ Functions ] ---------------------------------------- //





// ----------------------------------- [ Functions ] ---------------------------------------- //


bool step(Token* currentToken){
	if (currentToken == NULL)
		return false;
	
	TokenID currentTokenId = currentToken->id;
	StateID nextStateId = 0;
	
	
	// $MACRO$ switch //
	
	
	if (nextStateId > 0){
		currentStateId = nextStateId;
		return true;
	}
	
	return false;
}


// ------------------------------------------------------------------------------------------ //