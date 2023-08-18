%{
#include <stdio.h>
#include <stdlib.h>

#include "tokens.h"

int comment_depth = 0;
int line = 1;


#ifdef LEXER
	#define PRINTF(...) printf(__VA_ARGS__)
#else
	#define PRINTF(...)
#endif


%}

%x COMMENT

STRINGCONST   \'([^']|\'\')*\'
REALCONST     [0-9]+((\.[0-9]+)([Ee][+-][0-9]+)?|(\.[0-9]+)?([Ee][+-][0-9]+))
INTEGERCONST  [0-9]+

%%

<INITIAL,COMMENT>[ \t]+ ;
<INITIAL,COMMENT>[\n]   { line++; }

"true"|"false"       { PRINTF ("BOOLEANCONST:%d\n",line); return TOKEN_BOOLEANCONST; }
{STRINGCONST}        { PRINTF ("STRINGCONST:%d\n",line);  return TOKEN_STRINGCONST;  }
{REALCONST}          { PRINTF ("REALCONST:%d\n",line);    return TOKEN_REALCONST;    }
{INTEGERCONST}       { PRINTF ("INTEGERCONST:%d\n",line); return TOKEN_INTEGERCONST; }

"+"                  { PRINTF ("ADD:%d\n",line);          return TOKEN_ADD;          }
":="                 { PRINTF ("ASSIGN:%d\n",line);       return TOKEN_ASSIGN;       }
":"                  { PRINTF ("COLON:%d\n",line);        return TOKEN_COLON;        }
","                  { PRINTF ("COMMA:%d\n",line);        return TOKEN_COMMA;        }
"/"                  { PRINTF ("DIV:%d\n",line);          return TOKEN_DIV;          }
"."                  { PRINTF ("DOT:%d\n",line);          return TOKEN_DOT;          }
"="                  { PRINTF ("EQU:%d\n",line);          return TOKEN_EQU;          }
">="                 { PRINTF ("GEQ:%d\n",line);          return TOKEN_GEQ;          }
">"                  { PRINTF ("GTH:%d\n",line);          return TOKEN_GTH;          }
".."                 { PRINTF ("INTERVAL:%d\n",line);     return TOKEN_INTERVAL;     }
"["                  { PRINTF ("LBRACKET:%d\n",line);     return TOKEN_LBRACKET;     }
"("                  { PRINTF ("LPARENT:%d\n",line);      return TOKEN_LPARENT;      }
"<="                 { PRINTF ("LEQ:%d\n",line);          return TOKEN_LEQ;          }
"<"                  { PRINTF ("LTH:%d\n",line);          return TOKEN_LTH;          }
"*"                  { PRINTF ("MUL:%d\n",line);          return TOKEN_MUL;          }
"<>"                 { PRINTF ("NEQ:%d\n",line);          return TOKEN_NEQ;          }
"^"                  { PRINTF ("PTR:%d\n",line);          return TOKEN_PTR;          }
"]"                  { PRINTF ("RBRACKET:%d\n",line);     return TOKEN_RBRACKET;     }
")"                  { PRINTF ("RPARENT:%d\n",line);      return TOKEN_RPARENT;      }
";"                  { PRINTF ("SEMIC:%d\n",line);        return TOKEN_SEMIC;        }
"-"                  { PRINTF ("SUB:%d\n",line);          return TOKEN_SUB;          }

"boolean"            { PRINTF ("BOOLEAN:%d\n",line);      return TOKEN_BOOLEAN;      }
"char"               { PRINTF ("CHAR:%d\n",line);         return TOKEN_CHAR;         }
"real"               { PRINTF ("REAL:%d\n",line);         return TOKEN_REAL;         }
"integer"            { PRINTF ("INTEGER:%d\n",line);      return TOKEN_INTEGER;      }

"and"                { PRINTF ("AND:%d\n",line);          return TOKEN_AND;          }
"array"              { PRINTF ("ARRAY:%d\n",line);        return TOKEN_ARRAY;        }
"begin"              { PRINTF ("BEGIN:%d\n",line);        return TOKEN_BEGIN;        }
"case"               { PRINTF ("CASE:%d\n",line);         return TOKEN_CASE;         }
"const"              { PRINTF ("CONST:%d\n",line);        return TOKEN_CONST;        }
"do"                 { PRINTF ("DO:%d\n",line);           return TOKEN_DO;           }
"downto"             { PRINTF ("DOWNTO:%d\n",line);       return TOKEN_DOWNTO;       }
"else"               { PRINTF ("ELSE:%d\n",line);         return TOKEN_ELSE;         }
"end"                { PRINTF ("END:%d\n",line);          return TOKEN_END;          }
"file"               { PRINTF ("FILE:%d\n",line);         return TOKEN_FILE;         }
"for"                { PRINTF ("FOR:%d\n",line);          return TOKEN_FOR;          }
"forward"            { PRINTF ("FORWARD:%d\n",line);      return TOKEN_FORWARD;      }
"function"           { PRINTF ("FUNCTION:%d\n",line);     return TOKEN_FUNCTION;     }
"goto"               { PRINTF ("GOTO:%d\n",line);         return TOKEN_GOTO;         }
"div"                { PRINTF ("IDIV:%d\n",line);         return TOKEN_IDIV;         }
"if"                 { PRINTF ("IF:%d\n",line);           return TOKEN_IF;           }
"mod"                { PRINTF ("IMOD:%d\n",line);         return TOKEN_IMOD;         }
"in"                 { PRINTF ("IN:%d\n",line);           return TOKEN_IN;           }
"label"              { PRINTF ("LABEL:%d\n",line);        return TOKEN_LABEL;        }
"nil"                { PRINTF ("NIL:%d\n",line);          return -1; /*TOKEN_NIL*/   }
"not"                { PRINTF ("NOT:%d\n",line);          return TOKEN_NOT;          }
"of"                 { PRINTF ("OF:%d\n",line);           return TOKEN_OF;           }
"or"                 { PRINTF ("OR:%d\n",line);           return TOKEN_OR;           }
"packed"             { PRINTF ("PACKED:%d\n",line);       return TOKEN_PACKED;       }
"procedure"          { PRINTF ("PROCEDURE:%d\n",line);    return TOKEN_PROCEDURE;    }
"program"            { PRINTF ("PROGRAM:%d\n",line);      return TOKEN_PROGRAM;      }
"record"             { PRINTF ("RECORD:%d\n",line);       return TOKEN_RECORD;       }
"repeat"             { PRINTF ("REPEAT:%d\n",line);       return TOKEN_REPEAT;       }
"set"                { PRINTF ("SET:%d\n",line);          return TOKEN_SET;          }
"step"               { PRINTF ("STEP:%d\n",line);         return TOKEN_STEP;         }
"then"               { PRINTF ("THEN:%d\n",line);         return TOKEN_THEN;         }
"to"                 { PRINTF ("TO:%d\n",line);           return TOKEN_TO;           }
"type"               { PRINTF ("TYPE:%d\n",line);         return TOKEN_TYPE;         }
"until"              { PRINTF ("UNTIL:%d\n",line);        return TOKEN_UNTIL;        }
"var"                { PRINTF ("VAR:%d\n",line);          return TOKEN_VAR;          }
"while"              { PRINTF ("WHILE:%d\n",line);        return TOKEN_WHILE;        }
"with"               { PRINTF ("WITH:%d\n",line);         return TOKEN_WITH;         }

[A-Za-z][A-Za-z0-9]* { PRINTF ("IDENTIFIER:%d\n",line);   return TOKEN_IDENTIFIER;   }

<INITIAL>\{ { comment_depth++; BEGIN(COMMENT); }
<COMMENT>\{ { comment_depth++; }
<COMMENT>\} { comment_depth--; if (comment_depth == 0) BEGIN(INITIAL); }
<COMMENT>.  ;

. {
    printf ("LEX: Unexpected character '%c' [%d].\n", yytext[0], yytext[0]);
    exit (1);
  }

%%

#ifdef LEXER
int main(){
	while (yylex () != 0){}
	return 0;
}
#endif
