#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef __USE_POSIX199309
	#define __USE_POSIX199309
	#include <time.h>
	#undef __USE_POSIX199309
#else
	#include <time.h>
#endif

#include "../../obj/pascal/pascal.h"
#include "../../obj/pascal/pascal-tokens.h"

#include "../ANSI.h"


// ------------------------------------[ Variables ] ---------------------------------------- //


DFA dfa;
CSRTokenID* result;
int result_n;

CSRTokenID* inputBuffer;
int inputBuffer_n;
int inputBuffer_i;


struct {
	int tokenCounter;
	int symbolCounter;
	int reductionCounter;
	int stepCounter;
	double buffer;
	double dfa;
	double deinit;
	double total;
} sw = {};


struct {
	int fid;
	bool stats;
} settings = {};

// ------------------------------------- [ Macros ] ----------------------------------------- //


#define STOPWATCH_START(var) {			\
	struct timespec t1, t2;				\
	clock_gettime(CLOCK_REALTIME, &t1);	\

#define STOPWATCH_STOP(var)				\
	clock_gettime(CLOCK_REALTIME, &t2);	\
	var = dt_ms(&t1, &t2);				\
}										\


inline double dt_ms(struct timespec* t1, struct timespec* t2){
	#define S_TO_NS ((long)1e9)
	#define NS_TO_MS ((double)1.0/1e6)
	const long ns1 = (t1->tv_sec * S_TO_NS) + t1->tv_nsec;
	const long ns2 = (t2->tv_sec * S_TO_NS) + t2->tv_nsec;
	const long dt = ns2 - ns1;
	return (double)dt * NS_TO_MS;
}


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


CSRTokenID getId(const char* name){
	if (name == NULL) return -1;
	switch (name[0]){
		case 'A':
			if (strcmp(name, "ADD"          ) == 0) return TOKEN_ADD;
			if (strcmp(name, "AND"          ) == 0) return TOKEN_AND;
			if (strcmp(name, "ARRAY"        ) == 0) return TOKEN_ARRAY;
			if (strcmp(name, "ASSIGN"       ) == 0) return TOKEN_ASSIGN;
			break;
		case 'B':
			if (strcmp(name, "BEGIN"        ) == 0) return TOKEN_BEGIN;
			if (strcmp(name, "BOOLEAN"      ) == 0) return TOKEN_BOOLEAN;
			if (strcmp(name, "BOOLEANCONST" ) == 0) return TOKEN_BOOLEANCONST;
			break;
		case 'C':
			if (strcmp(name, "CASE"         ) == 0) return TOKEN_CASE;
			if (strcmp(name, "CHAR"         ) == 0) return TOKEN_CHAR;
			if (strcmp(name, "COLON"        ) == 0) return TOKEN_COLON;
			if (strcmp(name, "COMMA"        ) == 0) return TOKEN_COMMA;
			if (strcmp(name, "CONST"        ) == 0) return TOKEN_CONST;
			break;
		case 'D':
			if (strcmp(name, "DIV"          ) == 0) return TOKEN_DIV;
			if (strcmp(name, "DO"           ) == 0) return TOKEN_DO;
			if (strcmp(name, "DOT"          ) == 0) return TOKEN_DOT;
			if (strcmp(name, "DOWNTO"       ) == 0) return TOKEN_DOWNTO;
			break;
		case 'E':
			if (strcmp(name, "ELSE"         ) == 0) return TOKEN_ELSE;
			if (strcmp(name, "END"          ) == 0) return TOKEN_END;
			if (strcmp(name, "EQU"          ) == 0) return TOKEN_EQU;
			break;
		case 'F':
			if (strcmp(name, "FILE"         ) == 0) return TOKEN_FILE;
			if (strcmp(name, "FOR"          ) == 0) return TOKEN_FOR;
			if (strcmp(name, "FORWARD"      ) == 0) return TOKEN_FORWARD;
			if (strcmp(name, "FUNCTION"     ) == 0) return TOKEN_FUNCTION;
			break;
		case 'G':
			if (strcmp(name, "GEQ"          ) == 0) return TOKEN_GEQ;
			if (strcmp(name, "GOTO"         ) == 0) return TOKEN_GOTO;
			if (strcmp(name, "GTH"          ) == 0) return TOKEN_GTH;
			break;
		case 'I':
			if (name[1] == 'D'){
				if (strcmp(name, "IDENTIFIER"   ) == 0) return TOKEN_IDENTIFIER;
				if (strcmp(name, "IDIV"         ) == 0) return TOKEN_IDIV;
			} else if (name[1] == 'N'){
				if (strcmp(name, "IN"           ) == 0) return TOKEN_IN;
				if (strcmp(name, "INTEGER"      ) == 0) return TOKEN_INTEGER;
				if (strcmp(name, "INTEGERCONST" ) == 0) return TOKEN_INTEGERCONST;
				if (strcmp(name, "INTERVAL"     ) == 0) return TOKEN_INTERVAL;
			} else {
				if (strcmp(name, "IF"           ) == 0) return TOKEN_IF;
				if (strcmp(name, "IMOD"         ) == 0) return TOKEN_IMOD;
			}
			break;
		case 'L':
			if (strcmp(name, "LABEL"        ) == 0) return TOKEN_LABEL;
			if (strcmp(name, "LBRACKET"     ) == 0) return TOKEN_LBRACKET;
			if (strcmp(name, "LEQ"          ) == 0) return TOKEN_LEQ;
			if (strcmp(name, "LPARENT"      ) == 0) return TOKEN_LPARENT;
			if (strcmp(name, "LTH"          ) == 0) return TOKEN_LTH;
			break;
		case 'M':
			if (strcmp(name, "MUL"          ) == 0) return TOKEN_MUL;
			break;
		case 'N':
			if (strcmp(name, "NEQ"          ) == 0) return TOKEN_NEQ;
			if (strcmp(name, "NIL"          ) == 0) return -1;
			if (strcmp(name, "NOT"          ) == 0) return TOKEN_NOT;
			break;
		case 'O':
			if (strcmp(name, "OF"           ) == 0) return TOKEN_OF;
			if (strcmp(name, "OR"           ) == 0) return TOKEN_OR;
			break;
		case 'P':
			if (strcmp(name, "PACKED"       ) == 0) return TOKEN_PACKED;
			if (strcmp(name, "PROCEDURE"    ) == 0) return TOKEN_PROCEDURE;
			if (strcmp(name, "PROGRAM"      ) == 0) return TOKEN_PROGRAM;
			if (strcmp(name, "PTR"          ) == 0) return TOKEN_PTR;
			break;
		case 'R':
			if (strcmp(name, "RBRACKET"     ) == 0) return TOKEN_RBRACKET;
			if (strcmp(name, "REAL"         ) == 0) return TOKEN_REAL;
			if (strcmp(name, "REALCONST"    ) == 0) return TOKEN_REALCONST;
			if (strcmp(name, "RECORD"       ) == 0) return TOKEN_RECORD;
			if (strcmp(name, "REPEAT"       ) == 0) return TOKEN_REPEAT;
			if (strcmp(name, "RPARENT"      ) == 0) return TOKEN_RPARENT;
			break;
		case 'S':
			if (strcmp(name, "SEMIC"        ) == 0) return TOKEN_SEMIC;
			if (strcmp(name, "SET"          ) == 0) return TOKEN_SET;
			if (strcmp(name, "STEP"         ) == 0) return TOKEN_STEP;
			if (strcmp(name, "STRINGCONST"  ) == 0) return TOKEN_STRINGCONST;
			if (strcmp(name, "SUB"          ) == 0) return TOKEN_SUB;
			break;
		case 'T':
			if (strcmp(name, "THEN"         ) == 0) return TOKEN_THEN;
			if (strcmp(name, "TO"           ) == 0) return TOKEN_TO;
			if (strcmp(name, "TYPE"         ) == 0) return TOKEN_TYPE;
			break;
		case 'U':
			if (strcmp(name, "UNTIL"        ) == 0) return TOKEN_UNTIL;
			break;
		case 'V':
			if (strcmp(name, "VAR"          ) == 0) return TOKEN_VAR;
			break;
		case 'W':
			if (strcmp(name, "WHILE"        ) == 0) return TOKEN_WHILE;
			if (strcmp(name, "WITH"         ) == 0) return TOKEN_WITH;
			break;
	}
	return -1;
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


void fillInputBuffer(){
	int count = 0;
	size_t size = 32767;
	CSRTokenID* v = malloc(sizeof(*v) * size);
	
	while (true){
		const char* line = nextLine();
		if (line == NULL){
			break;
		}
		
		if (count >= size){
			// size *= 2;
			// v = realloc(v, size);
			CSRTokenID* _v = malloc(sizeof(*v) * size * 2);
			memcpy(_v, v, sizeof(*v) * size);
			free(v);
			v = _v;
			size *= 2;
		}
		
		CSRTokenID id = getId(line);
		if (id < 0){
			fprintf(stderr, "Failed to parse token '%s'.\n", line);
			exit(1);
		}
		
		v[count++] = id;
	}
	
	inputBuffer = v;
	inputBuffer_i = 0;
	inputBuffer_n = count;
	sw.tokenCounter = count;
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


CSRToken* nextToken(){
	char* name = nextLine();
	if (name == NULL)
		return NULL;
	
	CSRTokenID id = getId(name);
	if (id < 0){
		fprintf(stderr, "Failed to parse token '%s'.\n", name);
		exit(1);
	}
	
	sw.tokenCounter++;
	return CSRToken_create(id);
}


CSRToken* nextToken_buffered(){
	if (inputBuffer_i < inputBuffer_n)
		return CSRToken_create(inputBuffer[inputBuffer_i++]);
	else
		return NULL;
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


void getResult(){
	result_n = dfa.tokenStack.count;
	result = malloc(sizeof(*result) * dfa.tokenStack.count);
	
	for (int i = 0 ; i < result_n ; i++){
		if (dfa.tokenStack.v[i] != NULL)
			result[i] = ((CSRToken*)dfa.tokenStack.v[i])->id;
		else
			result[i] = -1;
	}
	
}


void printResult(){
	printf("Result:     ");
	for (int i = 0 ; i < result_n ; i++)
		printf("%s ", CSRToken_getName(result[i]));
	printf("\n");
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


bool _onTokenDestroy(DFA*, CSRToken*){
	sw.symbolCounter++;
	return true;
}


void runDFA(){
	STOPWATCH_START(sw.dfa);
	
	if (!settings.stats){
		while (DFA_step(&dfa));
	}
	
	else {
		dfa.onTokenDelete = _onTokenDestroy;
		
		int prevBuf = dfa.tokenBuffer.count;
		while (DFA_step(&dfa)){
			sw.stepCounter++;
			if (dfa.tokenBuffer.count > prevBuf || (prevBuf > 0 && dfa.tokenBuffer.count >= prevBuf))
				sw.reductionCounter++;
			prevBuf = dfa.tokenBuffer.count;
		}
		
	}
	
	STOPWATCH_STOP(sw.dfa);
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


void f0(){
	dfa.getNextToken = nextToken;
	runDFA();
}


void f1(){
	dfa.getNextToken = nextToken_buffered;
	
	// Read all tokens
	STOPWATCH_START(sw.buffer);
	fillInputBuffer();
	STOPWATCH_STOP(sw.buffer);
	
	runDFA();
}


// void fX(){
// 	dfa.getNextToken = nextToken;
	
// 	// const int max = 20702;
// 	const int max = 20800;
// 	for (int i = 0 ; i < max ; i++){
// 		if (!DFA_step(&dfa))
// 			break;
		
// 		if (i < (max-20)) continue;
		
// 		printf("%-3d ", i);
// 		printState();
// 	}
	
// 	printf("HALT at token[%d]\n", tokenCounter);
// 	printState();
// }


// --------------------------------- [ Main Function ] -------------------------------------- //


void f(){
	DFA_init(&dfa);
	
	if (settings.fid == 0)
		f0();
	else if (settings.fid == 1)
		f1();
	
	getResult();
	
	
	STOPWATCH_START(sw.deinit);
	DFA_deinit(&dfa);
	STOPWATCH_STOP(sw.deinit);
}


int main(int argc, char const* const* argv){
	printf("======================================================\n");
	settings.fid = 1;
	settings.stats = false;
	
	for (int i = 1 ; i < argc ; i++){
		if (strcmp(argv[i],"-f0") == 0)
			settings.fid = 0;
		else if (strcmp(argv[i],"-f1") == 0)
			settings.fid = 1;
		else if (strcmp(argv[i],"-stat") == 0)
			settings.stats = true;
	}
	
	
	// Run program
	STOPWATCH_START(sw.total);
	f();
	STOPWATCH_STOP(sw.total);
	
	
	if (settings.stats){
		printResult();
		printf("Tokens:     %d\n", sw.tokenCounter);
		printf("Symbols:    %d\n", sw.symbolCounter);
		printf("Reductions: %d\n", sw.reductionCounter);
		printf("Steps:      %d\n", sw.stepCounter);
	}
	printf("INPUT:      %.3fms (~%.1fs)\n", sw.buffer, sw.buffer/1000.0);
	printf("DFA:        %.3fms (~%.1fs)\n", sw.dfa, sw.dfa/1000.0);
	printf("DELETE:     %.3fms (~%.1fs)\n", sw.deinit, sw.deinit/1000.0);
	printf("TOTAL:      %.3fms (~%.1fs)\n", sw.total, sw.total/1000.0);
	printf("======================================================\n");
	return (result_n == 1 && result[0] == TOKEN_program) ? 0 : 1;
}


// ------------------------------------------------------------------------------------------ //