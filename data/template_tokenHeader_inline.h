typedef enum {TEMP} CSRTokenID; // $MACRO delete
// $MACRO enum


typedef struct _CSRToken {
	CSRTokenID id;
	int childCount;					// Size of the children array (excluding null terminator).
	struct _CSRToken** children;	// Null-terminated array of child pointers.
	void* data;						// Additional user data.
	int refCount;					// Amount of pointer refrences in the AST.
} CSRToken;


/**
 * @brief Get name of a token from its ID.
 * @param id 
 * @return Name of existing token, NULL otherwise.
 */
const char* CSRToken_getName(CSRTokenID id);


CSRToken* CSRToken_create(CSRTokenID id);