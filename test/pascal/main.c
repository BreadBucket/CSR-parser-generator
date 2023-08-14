#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "../../obj/pascal/pascal.h"
#include "../../obj/pascal/pascal-tokens.h"

#include "../ANSI.h"


// ------------------------------------[ Variables ] ---------------------------------------- //


DFA dfa;


int tokenCounter = 0;
CSRToken* bufferedToken;


// ----------------------------------- [ Functions ] ---------------------------------------- //


char* nextLine(){
	static size_t buffSize = 0;
	static char* buff = NULL;
	
	int n = getline(&buff, &buffSize, stdin);
	if (n <= 0)
		return NULL;
	
	// Remove newline
	if (buff[n-1] == '\n')
		buff[--n] = '\0';
	
	return buff;
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


void enlistTokens(){
	Stack list;
	Stack_init(&list);
	
	char* name = nextLine();
	while (name != NULL){
		bool exists = false;
		
		for (int i = 0 ; i < list.count ; i++){
			if (strcmp((char*)list.v[i], name) == 0){
				exists = true;
				break;
			}
		}
		
		if (!exists){
			Stack_push(&list, strdup(name));
			printf("%s\n", name);
		}
		
	}
	
	// Delete strings
	for (int i = 0 ; i < list.count ; i++){
		free(list.v[i]);
	}
	
	Stack_deinit(&list);
}


CSRToken* peekToken(){
	if (bufferedToken == NULL)
		bufferedToken = dfa.getNextToken(&dfa);
	return bufferedToken;
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


CSRTokenID getId(const char* name){
	if (name == NULL) return -1;
	else if (strcmp(name, "BOOLEANCONST") == 0) return TOKEN_BOOLEANCONST;
	else if (strcmp(name, "STRINGCONST")  == 0) return TOKEN_STRINGCONST;
	else if (strcmp(name, "REALCONST")    == 0) return TOKEN_REALCONST;
	else if (strcmp(name, "INTEGERCONST") == 0) return TOKEN_INTEGERCONST;
	else if (strcmp(name, "ADD")          == 0) return TOKEN_ADD;
	else if (strcmp(name, "ASSIGN")       == 0) return TOKEN_ASSIGN;
	else if (strcmp(name, "COLON")        == 0) return TOKEN_COLON;
	else if (strcmp(name, "COMMA")        == 0) return TOKEN_COMMA;
	else if (strcmp(name, "DIV")          == 0) return TOKEN_DIV;
	else if (strcmp(name, "DOT")          == 0) return TOKEN_DOT;
	else if (strcmp(name, "EQU")          == 0) return TOKEN_EQU;
	else if (strcmp(name, "GEQ")          == 0) return TOKEN_GEQ;
	else if (strcmp(name, "GTH")          == 0) return TOKEN_GTH;
	else if (strcmp(name, "INTERVAL")     == 0) return TOKEN_INTERVAL;
	else if (strcmp(name, "LBRACKET")     == 0) return TOKEN_LBRACKET;
	else if (strcmp(name, "LPARENT")      == 0) return TOKEN_LPARENT;
	else if (strcmp(name, "LEQ")          == 0) return TOKEN_LEQ;
	else if (strcmp(name, "LTH")          == 0) return TOKEN_LTH;
	else if (strcmp(name, "MUL")          == 0) return TOKEN_MUL;
	else if (strcmp(name, "NEQ")          == 0) return TOKEN_NEQ;
	else if (strcmp(name, "PTR")          == 0) return TOKEN_PTR;
	else if (strcmp(name, "RBRACKET")     == 0) return TOKEN_RBRACKET;
	else if (strcmp(name, "RPARENT")      == 0) return TOKEN_RPARENT;
	else if (strcmp(name, "SEMIC")        == 0) return TOKEN_SEMIC;
	else if (strcmp(name, "SUB")          == 0) return TOKEN_SUB;
	else if (strcmp(name, "BOOLEAN")      == 0) return TOKEN_BOOLEAN;
	else if (strcmp(name, "CHAR")         == 0) return TOKEN_CHAR;
	else if (strcmp(name, "REAL")         == 0) return TOKEN_REAL;
	else if (strcmp(name, "INTEGER")      == 0) return TOKEN_INTEGER;
	else if (strcmp(name, "AND")          == 0) return TOKEN_AND;
	else if (strcmp(name, "ARRAY")        == 0) return TOKEN_ARRAY;
	else if (strcmp(name, "BEGIN")        == 0) return TOKEN_BEGIN;
	else if (strcmp(name, "CASE")         == 0) return TOKEN_CASE;
	else if (strcmp(name, "CONST")        == 0) return TOKEN_CONST;
	else if (strcmp(name, "DO")           == 0) return TOKEN_DO;
	else if (strcmp(name, "DOWNTO")       == 0) return TOKEN_DOWNTO;
	else if (strcmp(name, "ELSE")         == 0) return TOKEN_ELSE;
	else if (strcmp(name, "END")          == 0) return TOKEN_END;
	else if (strcmp(name, "FILE")         == 0) return TOKEN_FILE;
	else if (strcmp(name, "FOR")          == 0) return TOKEN_FOR;
	else if (strcmp(name, "FORWARD")      == 0) return TOKEN_FORWARD;
	else if (strcmp(name, "FUNCTION")     == 0) return TOKEN_FUNCTION;
	else if (strcmp(name, "GOTO")         == 0) return TOKEN_GOTO;
	else if (strcmp(name, "IDIV")         == 0) return TOKEN_IDIV;
	else if (strcmp(name, "IF")           == 0) return TOKEN_IF;
	else if (strcmp(name, "IMOD")         == 0) return TOKEN_IMOD;
	else if (strcmp(name, "IN")           == 0) return TOKEN_IN;
	else if (strcmp(name, "LABEL")        == 0) return TOKEN_LABEL;
	// else if (strcmp(name, "NIL")          == 0) return TOKEN_NIL;
	else if (strcmp(name, "NOT")          == 0) return TOKEN_NOT;
	else if (strcmp(name, "OF")           == 0) return TOKEN_OF;
	else if (strcmp(name, "OR")           == 0) return TOKEN_OR;
	else if (strcmp(name, "PACKED")       == 0) return TOKEN_PACKED;
	else if (strcmp(name, "PROCEDURE")    == 0) return TOKEN_PROCEDURE;
	else if (strcmp(name, "PROGRAM")      == 0) return TOKEN_PROGRAM;
	else if (strcmp(name, "RECORD")       == 0) return TOKEN_RECORD;
	else if (strcmp(name, "REPEAT")       == 0) return TOKEN_REPEAT;
	else if (strcmp(name, "SET")          == 0) return TOKEN_SET;
	else if (strcmp(name, "STEP")         == 0) return TOKEN_STEP;
	else if (strcmp(name, "THEN")         == 0) return TOKEN_THEN;
	else if (strcmp(name, "TO")           == 0) return TOKEN_TO;
	else if (strcmp(name, "TYPE")         == 0) return TOKEN_TYPE;
	else if (strcmp(name, "UNTIL")        == 0) return TOKEN_UNTIL;
	else if (strcmp(name, "VAR")          == 0) return TOKEN_VAR;
	else if (strcmp(name, "WHILE")        == 0) return TOKEN_WHILE;
	else if (strcmp(name, "WITH")         == 0) return TOKEN_WITH;
	else if (strcmp(name, "IDENTIFIER")   == 0) return TOKEN_IDENTIFIER;
	else return -1;
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


void loopPrint(const char* s, int n){
	while (n-- > 0)
		printf("%s", s);
}


void printToken(CSRToken* token){
	const char* name = NULL;
	if (token != NULL)
		name = CSRToken_getName(token->id);
	if (name != NULL)
		printf("%s", name);
	else
		printf("NULL");
}


void printTokenTree(CSRToken* token, int lvl){
	loopPrint(" ", lvl);
	printToken(token);
	
	if (token == NULL || token->childCount <= 0)
		return;
	
	lvl++;
	printf(":");
	for (int i = 0 ; i < token->childCount ; i++){
		printf("\n");
		printTokenTree(token->children[i], lvl);
	}
	
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


void printStack(){
	printf(ANSI_GREEN);
	
	for (int i = 0 ; i < dfa.tokenStack.count ; i++){
		printToken(((CSRToken*)dfa.tokenStack.v[i]));
		printf(" ");
	}
	
	printf(ANSI_RESET "| ");
	
	if (dfa.tokenBuffer.count > 0){
		printf(ANSI_YELLOW);
		for (int i = dfa.tokenBuffer.count - 1 ; i >= 0 ; i--){
			printToken(((CSRToken*)dfa.tokenBuffer.v[i]));
			printf(" ");
		}
	}
	
	if (peekToken() != NULL){
		printf(ANSI_CYAN);
		printf("%s[%d] ", CSRToken_getName(peekToken()->id), tokenCounter-1);
		printf(ANSI_RESET);
	}
	
	printf(ANSI_RESET);
}


void printState(){
	printf("[%3d]:  { ", dfa.currentStateId);
	printStack();
	printf("}\n");
}


void printDetailStack(){
	printf("TOKENS:\n");
	for (int i = 0 ; i < dfa.tokenStack.count ; i++){
		CSRToken* t = dfa.tokenStack.v[i];
		printf("  [%d]: %s (%p) ref:%d\n", i, CSRToken_getName(t->id), t, t->refCount);
	}
	
	if (dfa.tokenBuffer.count > 0){
		printf("--------\n");
		for (int i = 0 ; i < dfa.tokenBuffer.count ; i++){
			CSRToken* t = dfa.tokenBuffer.v[i];
			printf("  [%d]: %s (%p) ref:%d\n", i, CSRToken_getName(t->id), t, t->refCount);
		}
	}
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


CSRToken* nextToken(){
	if (bufferedToken != NULL){
		CSRToken* t = bufferedToken;
		bufferedToken = NULL;
		return t;
	}
	
	char* name = nextLine();
	if (name == NULL)
		return NULL;
	
	CSRTokenID id = getId(name);
	if (id < 0){
		fprintf(stderr, "Failed to parse token '%s'.\n", name);
		exit(1);
	}
	
	tokenCounter++;
	return CSRToken_create(id);
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


CSRToken* staticNext(){
	static int i = 0;
	static CSRTokenID v[] = {
		TOKEN_program_upto_consts,
		TOKEN_TYPE,
		TOKEN_IDENTIFIER,
		TOKEN_EQU,
		TOKEN_INTEGERCONST,
		TOKEN_INTERVAL,
		TOKEN_INTEGERCONST,
		TOKEN_SEMIC,
		TOKEN_IDENTIFIER,
		-1
	};
	
	
	if (bufferedToken != NULL){
		CSRToken* t = bufferedToken;
		bufferedToken = NULL;
		return t;
	}
	else if (v[i] == -1)
		return NULL;
	else
		return CSRToken_create(v[i++]);
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


void printTokens(int i, int n){
	for (int ii = 0 ; ii < i ; ii++)
		nextLine();
	while (n > 0){
		printf("[%3d]: %s\n", i, nextLine());
		i++;
		n--;
	}
}


// --------------------------------- [ Main Function ] -------------------------------------- //


void f0(){
	dfa.getNextToken = nextToken;
	while (DFA_step(&dfa));
	
	printf("STACK: " ANSI_YELLOW);
	for (int i = 0 ; i < dfa.tokenStack.count ; i++){
		CSRToken* t = (CSRToken*)dfa.tokenStack.v[i];
		printf("%s ", CSRToken_getName(t->id));
	}
	printf(ANSI_RESET "\n");
}


void f1(){
	dfa.getNextToken = nextToken;
	
	// const int max = 20702;
	const int max = 20800;
	for (int i = 0 ; i < max ; i++){
		if (!DFA_step(&dfa))
			break;
		
		if (i < (max-20)) continue;
		
		printf("%-3d ", i);
		printState();
	}
	
	printf("HALT at token[%d]\n", tokenCounter);
	printState();
}


void f2(){
	dfa.getNextToken = staticNext;
	int i = 0;
	
	printf("%-3d ", i++);
	printState();
	
	while (DFA_step(&dfa)){
		printf("%-3d ", i++);
		printState();
	}
	
	printf("HALT\n");
	printf("%-3d ", --i);
	printState();
}



int main(int argc, char const* const* argv){
	printf("======================================================\n");
	DFA_init(&dfa);
	
	
	// printTokens(0, 100);
	
	
	f0();
	// f1();
	// f2();
	
	
	DFA_deinit(&dfa);
	printf("======================================================\n");
	return 0;
}


// ------------------------------------------------------------------------------------------ //