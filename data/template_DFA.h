#pragma once


// ----------------------------------- [ Prototypes ] --------------------------------------- //


struct _Stack;
enum _TokenID;
typedef int StateID;
struct _Token;
struct _DFA;


// ------------------------------------------------------------------------------------------ //


typedef struct _Stack {
	int count;
	int size;
	void** v;
} Stack;


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
bool Stack_delete(Stack* stack);

bool Stack_rezerve(Stack* stack, int newSize);
bool Stack_push(Stack* stack, void* value);
void* Stack_pop(Stack* stack);
void* Stack_peek(Stack* stack);


// ------------------------------------------------------------------------------------------ //


typedef struct _Token {
	TokenID id;
	int childCount;				// Size of the children array (excluding null terminator).
	struct _Token** children;	// Null-terminated array of child pointers.
	void* data;					// Additional user data.
	int refCount;				// Amount of pointer refrences in the AST.
} Token;


Token* createToken(struct _DFA* dfa, TokenID id, int childCount, ...);


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
void DFA_delete(struct _DFA* dfa);


void DFA_destroyToken(struct _DFA* dfa, struct _Token* token);
void DFA_popTokens(struct _DFA* dfa, int i);
StateID DFA_popStates(struct _DFA* dfa, int i);
bool DFA_consume(struct _DFA* dfa, struct _Token* const currentToken);
bool DFA_step(struct _DFA* dfa);


// ---------------------------------- [ Structures ] ---------------------------------------- //


typedef Token* (*onTokenCreate_f)(struct _DFA*, Token*);
typedef bool (*onTokenDelete_f)(struct _DFA*, Token*);
typedef Token* (*getNextToken_f)(struct _DFA*);


typedef struct _DFA {
	Stack tokenStack;
	Stack tokenBuffer;
	Stack stateStack;
	StateID currentStateId;
	
	onTokenCreate_f onTokenCreate;
	onTokenDelete_f onTokenDelete;
	getNextToken_f getNextToken;
} DFA;


// ------------------------------------------------------------------------------------------ //