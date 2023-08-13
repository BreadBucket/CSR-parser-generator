#define CSR_HEADER


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


typedef CSRToken* (*onTokenCreate_f)(struct _DFA*, CSRToken*);
typedef bool (*onTokenDelete_f)(struct _DFA*, CSRToken*);
typedef CSRToken* (*getNextToken_f)(struct _DFA*);


typedef struct _DFA {
	Stack tokenStack;
	Stack tokenBuffer;
	Stack stateStack;
	StateID currentStateId;
	
	onTokenCreate_f onTokenCreate;
	onTokenDelete_f onTokenDelete;
	getNextToken_f getNextToken;
} DFA;


// ----------------------------------- [ Functions ] ---------------------------------------- //


bool Stack_init(Stack* stack);
bool Stack_deinit(Stack* stack);
bool Stack_rezerve(Stack* stack, int newSize);
bool Stack_push(Stack* stack, void* value);
void* Stack_pop(Stack* stack);
void* Stack_peek(Stack* stack);


void DFA_init(DFA* dfa);
void DFA_deinit(DFA* dfa);
CSRToken* DFA_createToken(DFA*, CSRTokenID id, int childCount, ...);
void DFA_destroyToken(DFA*, CSRToken*);
void DFA_popTokens(DFA*, int i);
StateID DFA_popStates(DFA*, int i);
bool DFA_consume(struct _DFA* const dfa, struct _CSRToken* const token);
bool DFA_step(DFA*);
bool DFA_unconsume(DFA*, int n);