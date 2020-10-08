%{
    #include "project1_node.c"
    #include "syntax.tab.h"
    int yycolno = 1;
    #define YY_USER_ACTION \
        yylloc.first_line = yylineno; \
        yylloc.first_column = yycolno; \
        yylloc.last_line = yylineno; \
        yylloc.last_column = yycolno + yyleng; \
        yycolno += yyleng;

%}
%option yylineno

int                  0|[1-9][0-9]*
frac                 \.[0-9]+
int_dec              -?{int}
int_hex              0[xX]{int}
int_all              {int_dec}|{int_hex}
float                -?{int}{frac}
id                   [_a-zA-Z][_a-zA-Z0-9]*
unic                 u[0-9a-fA-F]{4}
esc                  \\([\"\\/bfnrt]|{unic})
scp                  [^"\\\x00-\x1f]
char                 '[[:print:]]'
empty                [ \t\b\r\n]

%%

"struct"             { yylval = make_node("STRUCT", NULL,   0); return STRUCT; }
"char"|"int"|"float" { yylval = make_node("TYPE",   yytext, 0); return TYPE;   }
"if"                 { yylval = make_node("IF",     NULL,   0); return IF;     }
"else"               { yylval = make_node("ELSE",   NULL,   0); return ELSE;   }
"while"              { yylval = make_node("WHILE",  NULL,   0); return WHILE;  }
"return"             { yylval = make_node("RETURN", NULL,   0); return RETURN; }
"."                  { yylval = make_node("DOT",    NULL,   0); return DOT;    }
";"                  { yylval = make_node("SEMI",   NULL,   0); return SEMI;   }
","                  { yylval = make_node("COMMA",  NULL,   0); return COMMA;  }
"="                  { yylval = make_node("ASSIGN", NULL,   0); return ASSIGN; }
"<"                  { yylval = make_node("LT",     NULL,   0); return LT;     }
"<="                 { yylval = make_node("LE",     NULL,   0); return LE;     }
">"                  { yylval = make_node("GT",     NULL,   0); return GT;     }
">="                 { yylval = make_node("GE",     NULL,   0); return GE;     }
"!="                 { yylval = make_node("NE",     NULL,   0); return NE;     }
"=="                 { yylval = make_node("EQ",     NULL,   0); return EQ;     }
"+"                  { yylval = make_node("PLUS",   NULL,   0); return PLUS;   }
"-"                  { yylval = make_node("MINUS",  NULL,   0); return MINUS;  }
"*"                  { yylval = make_node("MUL",    NULL,   0); return MUL;    }
"/"                  { yylval = make_node("DIV",    NULL,   0); return DIV;    }
"&&"                 { yylval = make_node("AND",    NULL,   0); return AND;    }
"||"                 { yylval = make_node("OR",     NULL,   0); return OR;     }
"!"                  { yylval = make_node("NOT",    NULL,   0); return NOT;    }
"("                  { yylval = make_node("LP",     NULL,   0); return LP;     }
")"                  { yylval = make_node("RP",     NULL,   0); return RP;     }
"["                  { yylval = make_node("LB",     NULL,   0); return LB;     }
"]"                  { yylval = make_node("RB",     NULL,   0); return RB;     }
"{"                  { yylval = make_node("LC",     NULL,   0); return LC;     }
"}"                  { yylval = make_node("RC",     NULL,   0); return RC;     }
{int_all}            { yylval = make_node("INT",    yytext, 0); return INT;    }
{float}              { yylval = make_node("FLOAT",  yytext, 0); return FLOAT;  }
{char}               { yylval = make_node("CHAR",   yytext, 0); return CHAR;   }
{id}                 { yylval = make_node("ID",     yytext, 0); return ID;     }
{empty}              {                                                         }

%%
