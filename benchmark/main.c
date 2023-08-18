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

#include "obj/parser.h"
#include "obj/tokens.h"

// #include "../ANSI.h"


// ----------------------------------- [ Prototypes ] --------------------------------------- //


extern int yylex();


// ------------------------------------[ Variables ] ---------------------------------------- //


DFA dfa;
CSRTokenID* result;	// Array of leftover symbols on the stack
int result_n;


CSRTokenID* buffer;
size_t buffer_size;
size_t buffer_count;


struct {
	int tokenCounter;
	int symbolCounter;
	int reductionCounter;
	int stepCounter;
	double lexer;
	double parser;
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


void bufferLexer(){
	buffer_size = 124000;
	buffer = malloc(sizeof(*buffer) * buffer_size);
	buffer_count = 0;
	
	while (true){
		int id = yylex();
		if (id == 0)
			break;
		
		if (buffer_count >= buffer_size){
			buffer_size *= 2;
			buffer = realloc(buffer, sizeof(*buffer) * buffer_size);
		}
		
		buffer[buffer_count++] = (CSRTokenID)id;
	}
	
}


void saveResult(){
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
	for (int i = 0 ; i < result_n ; i++)
		printf("%s ", CSRToken_getName(result[i]));
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


CSRToken* _nextToken_buff(DFA*){
	static int i = 0;
	if (i < buffer_count)
		return CSRToken_create(buffer[i++]);
	else
		return NULL;
}


CSRToken* _nextToken_lex(DFA*){
	int t = yylex();
	if (t != 0){
		return CSRToken_create((CSRTokenID)t);
	} else {
		return NULL;
	}
}


bool _onTokenCreate(DFA*, CSRToken*){
	return true;
}


bool _onTokenDestroy(DFA*, CSRToken*){
	sw.symbolCounter++;
	return true;
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


void f(){
	if (settings.fid == 0 || settings.fid == 2){
		DFA_init(&dfa);
		dfa.getNextToken = _nextToken_buff;
		
		STOPWATCH_START(sw.lexer);
		bufferLexer();
		STOPWATCH_STOP(sw.lexer);
		
		STOPWATCH_START(sw.parser);
		while (DFA_step(&dfa));
		saveResult();
		STOPWATCH_STOP(sw.parser);
		
		STOPWATCH_START(sw.deinit);
		DFA_deinit(&dfa);
		STOPWATCH_STOP(sw.deinit);
	}
	
	else if (settings.fid == 1){
		STOPWATCH_START(sw.lexer);
		while (yylex() != 0);
		STOPWATCH_STOP(sw.lexer);
	}
	
	else if (settings.fid == 3){
		DFA_init(&dfa);
		dfa.getNextToken = _nextToken_lex;
		
		STOPWATCH_START(sw.parser);
		while (DFA_step(&dfa));
		saveResult();
		STOPWATCH_STOP(sw.parser);
		
		STOPWATCH_START(sw.deinit);
		DFA_deinit(&dfa);
		STOPWATCH_STOP(sw.deinit);
	}
	
	else if (settings.fid == 4){
		DFA_init(&dfa);
		dfa.getNextToken = _nextToken_buff;
		// dfa.getNextToken = _nextToken_lex;
		dfa.onTokenDelete = _onTokenDestroy;
		
		bufferLexer();
		sw.tokenCounter = buffer_count;
		
		int prevBuf = dfa.tokenBuffer.count;
		while (DFA_step(&dfa)){
			sw.stepCounter++;
			if (dfa.tokenBuffer.count > prevBuf || (prevBuf > 0 && dfa.tokenBuffer.count >= prevBuf))
				sw.reductionCounter++;
			prevBuf = dfa.tokenBuffer.count;
		}
		
		saveResult();
		DFA_deinit(&dfa);
	}
	
}


// --------------------------------- [ Main Function ] -------------------------------------- //


int main(int argc, char const* const* argv){
	printf("======================================================\n");
	settings.fid = 0;
	settings.stats = true;
	
	// Options
	for (int i = 1 ; i < argc ; i++){
		if (strcmp(argv[i],"-lexer-parser") == 0)
			settings.fid = 0;
		else if (strcmp(argv[i],"-lexer") == 0)		// lexer only
			settings.fid = 1;
		else if (strcmp(argv[i],"-parser") == 0)	// parser only (lexer buffered)
			settings.fid = 2;
		else if (strcmp(argv[i],"-analysis") == 0)	// no buffer
			settings.fid = 3;
		else if (strcmp(argv[i],"-stats") == 0)		// dont count objects
			settings.fid = 4;
	}
	
	
	// Run program
	STOPWATCH_START(sw.total);
	f();
	STOPWATCH_STOP(sw.total);
	
	
	if (settings.fid == 0){
		printf("LEXER:      %.3fms (~%.1fs)\n", sw.lexer, sw.lexer/1000.0);
		printf("PARSER:     %.3fms (~%.1fs)\n", sw.parser, sw.parser/1000.0);
		printf("DELETE:     %.3fms (~%.1fs)\n", sw.deinit, sw.deinit/1000.0);
	}
	else if (settings.fid == 1){
		printf("LEXER:      %.3fms (~%.1fs)\n", sw.lexer, sw.lexer/1000.0);
	}
	else if (settings.fid == 2){
		printf("PARSER:     %.3fms (~%.1fs)\n", sw.parser, sw.parser/1000.0);
		printf("DELETE:     %.3fms (~%.1fs)\n", sw.deinit, sw.deinit/1000.0);
	}
	else if (settings.fid == 3){
		printf("ANALYSIS:   %.3fms (~%.1fs)\n", sw.parser, sw.parser/1000.0);
	}
	else if (settings.fid == 4){
		printf("RESULT:     "); printResult(); printf("\n");
		printf("TOKENS:     %d\n", sw.tokenCounter);
		printf("SYMBOLS:    %d\n", sw.symbolCounter);
		printf("REDUCTIONS: %d\n", sw.reductionCounter);
		printf("STEPS:      %d\n", sw.stepCounter);
	}
	
	printf("TOTAL:      %.3fms (~%.1fs)\n", sw.total, sw.total/1000.0);
	printf("======================================================\n");
	return 0;
}


// ------------------------------------------------------------------------------------------ //