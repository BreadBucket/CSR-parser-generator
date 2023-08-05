#pragma once
#define CSR_HEADER
#include "template_TokenHeader.h"	// $MACRO delete
// $MACRO include_tokenHeader


// ----------------------------------- [ Prototypes ] --------------------------------------- //


typedef int StateID;
struct _DFA;


// $BEGIN _inline_tokenHeader
// ----------------------------------- [ Structures ] --------------------------------------- //


// $MACRO inline_tokenHeader


// $END
// ----------------------------------- [ Structures ] --------------------------------------- //


typedef struct {
	int count;
	int size;
	void** v;
} Stack;



typedef CSRToken* (*getNextToken_f)(struct _DFA*);
typedef CSRToken* (*onTokenCreate_f)(struct _DFA*, CSRToken*);
typedef bool (*onTokenDelete_f)(struct _DFA*, CSRToken*);



typedef struct _DFA {
	Stack tokenStack;
	Stack tokenBuffer;
	Stack stateStack;
	StateID currentStateId;
	
	getNextToken_f getNextToken;
	onTokenCreate_f onTokenCreate;
	
	/**
	 * @brief Callback function for when a token object is marked for deletion.
	 *        A token object is marked for deletion when its ref count reaches 0.
	 *        The user is responsible for managing the pointer when canceling deletion.
	 * @param DFA* Event source.
	 * @param CSRToken* Token marked for deletion.
	 * @return False to cancel token deletion.
	 * @return True to delete token object.
	 */
	onTokenDelete_f onTokenDelete;
} DFA;


// ----------------------------------- [ Structures ] --------------------------------------- //


/**
 * @brief Initialize a stack object: initial capacity is 1.
 *        Stack objects must be initialized before calling any stack functions.
 *        Initializing an already initialized object results in undefined behaviour.
 * @param stack
 * @return True if initialization was successful.
 */
bool Stack_init(Stack* stack);

/**
 * @brief Delete (deinitialize) a stack object.
 *        An object must be deinitialized before calling `free` or loosing scope. 
 *        Deleting non-initialized objects results in undefined behaviour.
 * @param stack 
 */
bool Stack_deinit(Stack* stack);

bool Stack_rezerve(Stack* stack, int newSize);
bool Stack_push(Stack* stack, void* value);
void* Stack_pop(Stack* stack);
void* Stack_peek(Stack* stack);


// ----------------------------------- [ Functions ] ---------------------------------------- //


CSRToken* createToken(struct _DFA* dfa, CSRTokenID id, int childCount, ...);


// ----------------------------------- [ Functions ] ---------------------------------------- //


/**
 * @brief Initialize a DFA object: internal stacks are intialized, initial state is set to 0.
 *        DFA objects must be initialized before calling any other DFA functions.
 *        Initializing an already initialized object results in undefined behaviour.
 * @param dfa 
 */
void DFA_init(struct _DFA* dfa);

/**
 * @brief Delete (deinitialize) a DFA object.
 *        DFA object must be deinitialized before calling `free` or loosing scope. 
 *        Deleting non-initialized objects results in undefined behaviour.
 * @param dfa 
 */
void DFA_deinit(struct _DFA* dfa);


void DFA_destroyToken(struct _DFA* dfa, struct _CSRToken* token);
void DFA_popTokens(struct _DFA* dfa, int i);
StateID DFA_popStates(struct _DFA* dfa, int i);
bool DFA_consume(struct _DFA* dfa, struct _CSRToken* const currentToken);
bool DFA_step(struct _DFA* dfa);


// ------------------------------------------------------------------------------------------ //