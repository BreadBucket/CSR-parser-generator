#pragma once
#define CSR_TOKEN_HEADER


// ----------------------------------- [ Structures ] --------------------------------------- //


typedef enum {TEMP} CSRTokenID; // $MACRO delete
// $MACRO enum


// ----------------------------------- [ Structures ] --------------------------------------- //


typedef struct _CSRToken {
	CSRTokenID id;
	int childCount;					// Size of the children array (excluding null terminator).
	struct _CSRToken** children;	// Null-terminated array of child pointers.
	void* data;						// Additional user data.
	int refCount;					// Amount of pointer refrences in the AST.
} CSRToken;


// ------------------------------------------------------------------------------------------ //