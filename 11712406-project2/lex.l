%{
    #include "node.h"
    #include "symbol_table.h"
    #include "syntax.tab.h"
    #define YY_USER_ACTION \
        yylloc.first_line = yylineno; \
        yylloc.first_column = yycolno; \
        yylloc.last_line = yylineno; \
        yylloc.last_column = yycolno + yyleng; \
        yycolno += yyleng;
        
    int yycolno = 1;
    int flag = 1;
    void multiple_comment();
    extern "C" int yylex();

%}
%option yylineno

int                  0|[1-9][0-9]*
frac                 \.[0-9]+
int_hex              0[xX]((0|[1-9][0-9]*)|([a-fA-F]))*
int_all              {int}|{int_hex}
float                -?{int}{frac}
id                   [_a-zA-Z][_a-zA-Z0-9]*
char                 '([[:print:]]|(\\[xX][0-9a-fA-F]{2}))'
empty                [ \t\b\r\n]
wrong_id             [0-9]+{id}
wrong_int            (0[xX][0-9]*[g-zG-Z]+)|(0[0-9]+)
wrong_char           '[[:print:]]*'
wrong_float          0[0-9]+{frac}

%%

"struct"             { yylval = new Node("STRUCT");             return STRUCT; }
"char"|"int"|"float" { yylval = new Node("TYPE", yytext);       return TYPE;   }
"if"                 { yylval = new Node("IF"    );             return IF;     }
"else"               { yylval = new Node("ELSE"  );             return ELSE;   }
"while"              { yylval = new Node("WHILE" );             return WHILE;  }
"return"             { yylval = new Node("RETURN");             return RETURN; }
"."                  { yylval = new Node("DOT"   );             return DOT;    }
";"                  { yylval = new Node("SEMI"  );             return SEMI;   }
","                  { yylval = new Node("COMMA" );             return COMMA;  }
"="                  { yylval = new Node("ASSIGN");             return ASSIGN; }
"<"                  { yylval = new Node("LT"    );             return LT;     }
"<="                 { yylval = new Node("LE"    );             return LE;     }
">"                  { yylval = new Node("GT"    );             return GT;     }
">="                 { yylval = new Node("GE"    );             return GE;     }
"!="                 { yylval = new Node("NE"    );             return NE;     }
"=="                 { yylval = new Node("EQ"    );             return EQ;     }
"+"                  { yylval = new Node("PLUS"  );             return PLUS;   }
"-"                  { yylval = new Node("MINUS" );             return MINUS;  }
"*"                  { yylval = new Node("MUL"   );             return MUL;    }
"/"                  { yylval = new Node("DIV"   );             return DIV;    }
"&&"                 { yylval = new Node("AND"   );             return AND;    }
"||"                 { yylval = new Node("OR"    );             return OR;     }
"!"                  { yylval = new Node("NOT"   );             return NOT;    }
"("                  { yylval = new Node("LP"    );             return LP;     }
")"                  { yylval = new Node("RP"    );             return RP;     }
"["                  { yylval = new Node("LB"    );             return LB;     }
"]"                  { yylval = new Node("RB"    );             return RB;     }
"{"                  { yylval = new Node("LC"    );             return LC;     }
"}"                  { yylval = new Node("RC"    );             return RC;     }
"//"                 { char c;  while((c=yyinput() != '\n'));                  }
"/*"                 { multiple_comment();}
{int_all}            { yylval = new Node("INT",  yytext, yylineno);     return INT;    }
{float}              { yylval = new Node("FLOAT",yytext, yylineno);     return FLOAT;  }
{char}               { yylval = new Node("CHAR", yytext, yylineno);     return CHAR;   }
{id}                 { yylval = new Node("ID",   yytext, yylineno);     return ID;     }
{empty}              {                                                         }

.|{wrong_int}|{wrong_char}|{wrong_float}|{wrong_id}              { flag = 0; printf("Error type A at Line %d: unknown lexeme %s\n", yylineno, yytext); return LERROR; }
%%

void multiple_comment() {
    int c;
    while (1) {
        switch(yyinput()) {
            case 0:
                fprintf(stderr, "Unclosed comment\n");
                exit(-1);
            case '*':
                if ((c = yyinput()) == '/') return;
                unput(c);
                break;
        }
    }
}