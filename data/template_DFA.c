#include <stdbool.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>

#include "template_DFA.h"		// $MACRO delete
// $MACRO include_header
// $MACRO include_tokenHeader-noMainHeader

// ---------------------------------- [ Definitions ] --------------------------------------- //


#ifndef STACK_RESIZERULE
	#define STACK_RESIZERULE(n)	((n)*2)
#endif


// $BEGIN _inline_header
// ----------------------------------- [ Structures ] --------------------------------------- //


// $MACRO inline_header


// $END
// ----------------------------------- [ Functions ] ---------------------------------------- //


const char* CSRToken_getName(CSRTokenID id){
	// $MACRO token_name_switch
	return NULL;
}


inline CSRToken* CSRToken_create(CSRTokenID id){
	CSRToken* t = malloc(sizeof(CSRToken));
	if (t != NULL){
		t->id = id;
		t->childCount = 0;
		t->children = NULL;
		t->data = NULL;
		t->refCount = 0;
	}
	return t;
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


bool Stack_init(Stack* stack){
	stack->count = 0;
	stack->size = 1;
	stack->v = malloc(sizeof(*stack->v) * stack->size);
	return (stack->v != NULL);
}


bool Stack_deinit(Stack* stack){
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


inline void* Stack_pop(Stack* stack){
	return stack->v[--stack->count];
}


inline void* Stack_peek(Stack* stack){
	return stack->v[stack->count - 1];
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


void DFA_init(DFA* dfa){
	Stack_init(&dfa->tokenStack);
	Stack_init(&dfa->tokenBuffer);
	Stack_init(&dfa->stateStack);
	dfa->onTokenCreate = NULL;
	dfa->onTokenDelete = NULL;
	dfa->getNextToken = NULL;
	
	// Initial state 0
	dfa->currentStateId = (StateID)0;
	Stack_push(&dfa->stateStack, (void*)(long)(StateID)0);
}


void DFA_unlinkTokens(DFA* dfa, CSRToken** v, int n){
	for (int i = 0 ; i < n ; i++){
		CSRToken* t = (CSRToken*)v[i];
		if (--(t->refCount) <= 0)
			DFA_destroyToken(dfa, t);
	}
}


void DFA_deinit(DFA* dfa){
	DFA_unlinkTokens(dfa, (CSRToken**)dfa->tokenStack.v, dfa->tokenStack.count);
	DFA_unlinkTokens(dfa, (CSRToken**)dfa->tokenBuffer.v, dfa->tokenBuffer.count);
	Stack_deinit(&dfa->tokenStack);
	Stack_deinit(&dfa->tokenBuffer);
	Stack_deinit(&dfa->stateStack);
}


void DFA_popTokens(DFA* dfa, int i){
	Stack* const tokenStack = &dfa->tokenStack;
	
	for ( ; i > 0 ; i--){
		CSRToken* t = (CSRToken*)Stack_pop(tokenStack);
		if (--(t->refCount) <= 0)
			DFA_destroyToken(dfa, t);
	}
	
}


StateID DFA_popStates(DFA* dfa, int i){
	const int count = dfa->stateStack.count - i;
	dfa->stateStack.count = count;
	return dfa->currentStateId = (StateID)(long)dfa->stateStack.v[count - 1];
}


bool DFA_unconsume(DFA* dfa, int n){
	Stack* tokens = &dfa->tokenStack;
	Stack* buffer = &dfa->tokenBuffer;
	
	// Resize buffer
	const int minBuffSize = buffer->count + n;
	if (buffer->size < minBuffSize){
		if (!Stack_rezerve(buffer, minBuffSize))
			return false;
	}
	
	// Move tokens
	void** p1 = &tokens->v[tokens->count - 1];
	void** p2 = &buffer->v[buffer->count];
	for (int i = 0 ; i < n ; i++){
		*p2 = *p1;
		p1--;
		p2++;
	}
	
	tokens->count -= n;
	buffer->count += n;
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


CSRToken* DFA_createToken(DFA* dfa, CSRTokenID id, int childCount, ...){
	CSRToken* t = CSRToken_create(id);
	if (t == NULL)
		return NULL;
	
	// Add children
	if (childCount > 0){
		t->childCount = childCount;
		t->children = malloc(sizeof(*t->children) * childCount + 1);
		t->children[childCount] = NULL;
		
		va_list l;
		va_start(l, childCount);
		
		for (int i = 0 ; i < childCount ; i++){
			CSRToken* child = va_arg(l, CSRToken*);
			child->refCount++;
			t->children[i] = child;
		}
		
		va_end(l);
	}
	
	
	if (dfa->onTokenCreate != NULL)
		t = dfa->onTokenCreate(dfa, t);
	
	return t;
}


void DFA_destroyToken(DFA* dfa, CSRToken* token){
	if (dfa->onTokenDelete != NULL && !dfa->onTokenDelete(dfa, token)){
		return;
	} else {
		CSRToken** children = token->children;
		
		for (int i = 0 ; i < token->childCount ; i++){
			if (--(children[i]->refCount) <= 0)
				DFA_destroyToken(dfa, children[i]);
		}
		
		free(children);
		free(token);
	}
}


static inline CSRToken* _incRef(CSRToken* t){
	t->refCount++;
	return t;
}


// $BEGIN _usercode
// ----------------------------------- [ USER CODE ] ---------------------------------------- //
// $MACRO usercode
// $END
// $BEGIN _reductions_functions
// ----------------------------------- [ Functions ] ---------------------------------------- //


// $MACRO reductions_functions


// $END
// ----------------------------------- [ Functions ] ---------------------------------------- //


bool DFA_consume(DFA* const _dfa, CSRToken* const _currentToken){
	if (_currentToken == NULL){
		return false;
	}
	
	Stack* const _tokenStack  = &_dfa->tokenStack;
	Stack* const _tokenBuffer = &_dfa->tokenBuffer;
	Stack* const _stateStack  = &_dfa->stateStack;
	StateID const _currentStateId = _dfa->currentStateId;	// inconsistency when popping extra symbols from reduction item
	CSRTokenID const _currentTokenId = _currentToken->id;
	bool _halt = false;
	
	// Push current token
	_currentToken->refCount++;
	Stack_push(_tokenStack, _currentToken);
	
	
	// DFA switch
	{
		StateID _nextStateId = 0;
		
		// State switch
		// $MACRO state_switch //
		return false; // Unknown state: Halt
	
		// Transition switch: select next state or a reduction
		goto __STATE_END;
		// $MACRO transition_switch //
		__STATE_END:
		
		// No reductions selected: Push state
		_dfa->currentStateId = _nextStateId;
		Stack_push(_stateStack, (void*)(long)_nextStateId);
		return true;
	}
	
	
	// Reduction items
	goto __REDUCTIONITEM_END;
	{
		// $MACRO reduction_items //
	}
	__REDUCTIONITEM_END:
	
	
	// Reductions
	goto __REDUCTION_END;
	{
		// $MACRO reductions //
	}
	__REDUCTION_END:
	
	
	return !_halt;
}


bool DFA_step(DFA* dfa){
	CSRToken* t;
	
	if (dfa->tokenBuffer.count > 0){
		t = Stack_pop(&dfa->tokenBuffer);
		t->refCount--;
	} else if (dfa->getNextToken != NULL){
		t = dfa->getNextToken(dfa);
	}
	
	return DFA_consume(dfa, t);
}


// ------------------------------------------------------------------------------------------ //