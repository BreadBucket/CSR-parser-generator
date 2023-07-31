#include <stdbool.h>
#include <string.h>
#include <stdarg.h>


// ---------------------------------- [ Definitions ] --------------------------------------- //


#ifndef STACK_RESIZERULE
	#define STACK_RESIZERULE(n)	((n)*2)
#endif


// ---------------------------------- [ Structures ] ---------------------------------------- //


typedef enum {
	TOKEN_UNKNOWN,
	// $MACRO$ enum //
} TokenID;


// ---------------------------------- [ Structures ] ---------------------------------------- //


typedef int StateID;


typedef struct _Token {
	TokenID id;
	int childCount;				// Size of the children array (excluding null terminator).
	struct _Token** children;	// Null-terminated array of child pointers.
	void* data;					// Additional user data.
	int refCount;				// Amount of pointer refrences in the AST.
} Token;


typedef struct {
	int count;
	int size;
	void** v;
} Stack;


typedef struct {
	Stack tokenStack;
	Stack tokenBuffer;
	Stack stateStack;
	StateID currentStateId;
} DFA;


// ----------------------------------- [ Prototypes ] --------------------------------------- //


Token* getNextToken();
bool onTokenDelete(Token*);
Token* onTokenCreate(Token*);


// ----------------------------------- [ Functions ] ---------------------------------------- //


bool Stack_init(Stack* stack){
	stack->count = 0;
	stack->size = 1;
	stack->v = malloc(sizeof(*stack->v) * stack->size);
	return (stack->v != NULL);
}


bool Stack_delete(Stack* stack){
	stack->count = 0;
	stack->size = 0;
	free(stack->v);
}


bool Stack_rezerve(Stack* stack, int newSize){
	if (newSize > stack->size){
		
		const size_t size = sizeof(void*) * newSize;
		void* v = realloc(stack->v, size);
		
		if (v == NULL){
			return false;
		}
		
		stack->v = v;
		stack->size = newSize;
		return true;
	}
	return true;
}


bool Stack_push(Stack* stack, void* value){
	const int i = stack->count;
	
	if (i >= stack->size){
		if (!Stack_rezerve(stack, STACK_RESIZERULE(stack->size)))
			return false;
	}
	
	stack->v[i] = value;
	stack->count++;
	
	return true;
}


void* Stack_pop(Stack* stack){
	const int i = --stack->count;
	return stack->v[i];
}


void* Stack_peek(Stack* stack){
	return stack->v[stack->count - 1];
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


void DFA_init(DFA* dfa){
	Stack_init(&dfa->tokenStack);
	Stack_init(&dfa->tokenBuffer);
	Stack_init(&dfa->stateStack);
	dfa->currentStateId = (StateID)0;
	Stack_push(&dfa->stateStack, (void*)(long)(StateID)0);
}


StateID DFA_popState(DFA* dfa, int i){
	const int count = dfa->stateStack.count - i;
	dfa->stateStack.count = count;
	return dfa->currentStateId = (StateID)(long)dfa->stateStack.v[count - 1];
}


void DFA_popToken(DFA* dfa, int i){
	Stack* const tokenStack = &dfa->tokenStack;
	
	for ( ; i > 0 ; i--){
		Token* t = (Token*)Stack_pop(tokenStack);
		if (--(t->refCount) <= 0)
			free(t);
	}
	
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


Token* createToken(TokenID id, int childCount, ...){
	Token* t = malloc(sizeof(Token));
	if (t == NULL)
		return NULL;
	
	t->id = id;
	t->childCount = childCount;
	
	if (childCount > 0){
		t->children = malloc(sizeof(*t->children) * childCount + 1);
		
		va_list argp;
		va_start(argp, childCount);
		
		for (int i = 0 ; i < childCount ; i++){
			Token* child = va_arg(argp, Token*);
			child->refCount++;
			t->children[i] = child;
		}
		
		t->children[childCount] = NULL;
		
		va_end(argp);
	} else {
		t->children = NULL;
	}
	
	t->data = NULL;
	t->refCount = 0;
	
	return onTokenCreate(t);
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


bool DFA_consume(DFA* dfa, Token* const currentToken){
	if (currentToken == NULL){
		return false;
	}
	
	Stack*  const tokenStack     = &dfa->tokenStack;
	Stack*  const tokenBuffer    = &dfa->tokenBuffer;
	Stack*  const stateStack     = &dfa->stateStack;
	StateID const currentStateId = dfa->currentStateId;
	TokenID const currentTokenId = currentToken->id;
	
	StateID nextStateId = -1;
	bool pushNextState = true;
	bool halt = true;	// Halt if no next state set.
	
	// Push current token
	currentToken->refCount++;
	Stack_push(tokenStack, currentToken);
	
	
	// $MACRO$ switch //
	
	
	if (pushNextState && nextStateId >= 0){
		dfa->currentStateId = nextStateId;
		Stack_push(stateStack, (void*)(long)nextStateId);
		return true;
	}
	
	
	return !halt;
}


bool step(DFA* dfa){
	Token* t;
	
	if (dfa->tokenBuffer.count > 0)
		t = Stack_pop(&dfa->tokenBuffer);
	else
		t = getNextToken();
	
	return DFA_consume(dfa, t);
}


// ------------------------------------------------------------------------------------------ //