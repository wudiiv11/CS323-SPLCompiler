%{
    #define YYSTYPE Node *
    #include "lex.yy.c"
    #include "parser.h"
    void yyerror (const char*);
    extern int flag;
    Node* root;
%}

%token TYPE LERROR
%token CHAR INT FLOAT
%token ID
%token STRUCT IF ELSE WHILE RETURN
%token DOT SEMI COMMA ASSIGN
%token LT LE GT GE NE EQ
%token PLUS MINUS MUL DIV
%token AND OR NOT
%token LP RP LB RB LC RC

%right ASSIGN
%left  OR
%left  AND
%left  LT LE GT GE EQ NE
%left  PLUS MINUS
%left  MUL DIV
%right NOT
%left  LP RP LB RB LC RC DOT

%%

Program:
      ExtDefList                   { root = $$ = new Node("Program", "", @$.first_line);         $$->insert(1, $1);}
   ;
ExtDefList:
      ExtDef ExtDefList                   { $$ = new Node("ExtDefList", "", @$.first_line);      $$->insert(2, $1, $2);}
   |  %empty                              { $$ = new Node();}
   ;
ExtDef:
      Specifier ExtDecList SEMI           { $$ = new Node("ExtDef", "", @$.first_line);          $$->insert(3, $1, $2, $3);}
   |  Specifier SEMI                      { $$ = new Node("ExtDef", "", @$.first_line);          $$->insert(2, $1, $2);}
   |  Specifier FunDec CompSt             { $$ = new Node("ExtDef", "", @$.first_line);          $$->insert(3, $1, $2, $3);}
   |  Specifier LERROR;
   |  Specifier ExtDecList error          { flag = 0; printf("Error type B at Line %d: Missing close brace\n", @$.first_line); }
   ;
ExtDecList:
      VarDec                              { $$ = new Node("ExtDecList", "", @$.first_line);      $$->insert(1, $1);}
   |  VarDec COMMA ExtDecList             { $$ = new Node("ExtDecList", "", @$.first_line);      $$->insert(3, $1, $2, $3);}
   ;
Specifier:
      TYPE                                { $$ = new Node("Specifier",  "", @$.first_line);      $$->insert(1, $1);}
   |  StructSpecifier                     { $$ = new Node("Specifier",  "", @$.first_line);      $$->insert(1, $1);}
   ;
StructSpecifier:
      STRUCT ID LC DefList RC             { $$ = new Node("StructSpecifier", "", @$.first_line); $$->insert(5, $1, $2, $3, $4, $5);}
   |  STRUCT ID                           { $$ = new Node("StructSpecifier", "", @$.first_line); $$->insert(2, $1, $2);}
   ;
VarDec:
      ID                                  { $$ = new Node("VarDec", "", @$.first_line);          $$->insert(1, $1);}
   |  VarDec LB INT RB                    { $$ = new Node("VarDec", "", @$.first_line);          $$->insert(4, $1, $2, $3, $4);}
   |  LERROR
   ;
FunDec:
      ID LP VarList RP                    { $$ = new Node("FunDec", "", @$.first_line);          $$->insert(4, $1, $2, $3, $4); }
   |  ID LP RP                            { $$ = new Node("FunDec", "", @$.first_line);          $$->insert(3, $1, $2, $3); }
   |  ID LERROR VarList RP
   |  ID LP VarList LERROR
   |  ID LERROR RP
   |  ID LP LERROR
   |  LERROR LP VarList RP
   |  LERROR LP RP
   |  ID LP error                         { flag = 0; printf("Error type B at Line %d: Missing closing parenthesis ')'\n", @$.first_line); }
   |  LERROR LP error                     { flag = 0; printf("Error type B at Line %d: Missing closing parenthesis ')'\n", @$.first_line); }
   ;
VarList:
      ParamDec COMMA VarList              { $$ = new Node("VarList", "", @$.first_line);         $$->insert(3, $1, $2, $3); }
   |  ParamDec                            { $$ = new Node("VarList", "", @$.first_line);         $$->insert(1, $1); }
   ;
ParamDec:
      Specifier VarDec                    { $$ = new Node("ParamDec", "", @$.first_line);        $$->insert(2, $1, $2); }
   ;
CompSt:
      LC DefList StmtList RC              { $$ = new Node("CompSt", "", @$.first_line);          $$->insert(4, $1, $2, $3, $4); }
   |  LC DefList error                    { flag = 0; printf("Error type B at line %d: Missing closing brace '}'\n", @$.first_line); }
   |  LC error                            { flag = 0; printf("Error type B at line %d: Missing closing brace '}'\n", @$.first_line); }
   ;
StmtList:
      Stmt StmtList                       { $$ = new Node("StmtList", "", @$.first_line);        $$->insert(2, $1, $2); }
   |  %empty                              { $$ = new Node("", "", @$.first_line); }
   ;
Stmt:
      Exp SEMI                            { $$ = new Node("Stmt", "", @$.first_line);            $$->insert(2, $1, $2); }
   |  CompSt                              { $$ = new Node("Stmt", "", @$.first_line);            $$->insert(1, $1); }
   |  RETURN Exp SEMI                     { $$ = new Node("Stmt", "", @$.first_line);            $$->insert(3, $1, $2, $3); }
   |  IF LP Exp RP Stmt                   { $$ = new Node("Stmt", "", @$.first_line);            $$->insert(5, $1, $2, $3, $4, $5); }
   |  IF LP Exp RP Stmt ELSE Stmt         { $$ = new Node("Stmt", "", @$.first_line);            $$->insert(7, $1, $2, $3, $4, $5, $6, $7); }
   |  WHILE LP Exp RP Stmt                { $$ = new Node("Stmt", "", @$.first_line);            $$->insert(5, $1, $2, $3, $4, $5); }
   |  RETURN LERROR SEMI 
   |  WHILE LP LERROR RP Stmt
   |  RETURN error                        { flag = 0; printf("Error type B at Line %d: Missing semicolon ';'\n", @$.first_line); }
   |  IF LP error                         { flag = 0; printf("Error type B at Line %d: Missing closing parenthesis ')'\n", @$.first_line); }
   |  WHILE LP error                      { flag = 0; printf("Error type B at Line %d: Missing closing parenthesis ')'\n", @$.first_line); }
   |  Exp error                           { flag = 0; printf("Error type B at Line %d: Missing semicolon ';'\n", @$.first_line); }
   ;
DefList:
      Def DefList                         { $$ = new Node("DefList", "", @$.first_line);         $$->insert(2, $1, $2); }
   |  %empty                              { $$ = new Node("", "", @$.first_line); }
   |  DefList error Def                   { flag = 0; printf("Error type B at Line %d: Missing specifier\n", @2.first_line); }
   ;
Def:
      Specifier DecList SEMI              { $$ = new Node("Def", "", @$.first_line);             $$->insert(3, $1, $2, $3); }
   |  Specifier LERROR SEMI
   |  Specifier DecList error             { flag = 0; printf("Error type B at Line %d: Missing semicolon ';'\n", @$.first_line); }
   |  Specifier error                     { flag = 0; printf("Error type B at Line %d: Specifier error\n", @$.first_line); }
   ;
DecList:
      Dec                                 { $$ = new Node("DecList", "", @$.first_line);         $$->insert(1, $1); }
   |  Dec COMMA DecList                   { $$ = new Node("DecList", "", @$.first_line);         $$->insert(3, $1, $2, $3); }
   ;
Dec:
      VarDec                              { $$ = new Node("Dec", "", @$.first_line);             $$->insert(1, $1); }
   |  VarDec ASSIGN Exp                   { $$ = new Node("Dec", "", @$.first_line);             $$->insert(3, $1, $2, $3); }
   |  VarDec ASSIGN LERROR
   ;
Exp: 
      Exp ASSIGN Exp                      { $$ = new Node("Exp", "", @$.first_line);             $$->insert(3, $1, $2, $3); }
   |  Exp AND Exp                         { $$ = new Node("Exp", "", @$.first_line);             $$->insert(3, $1, $2, $3); }
   |  Exp OR Exp                          { $$ = new Node("Exp", "", @$.first_line);             $$->insert(3, $1, $2, $3); }
   |  Exp LT Exp                          { $$ = new Node("Exp", "", @$.first_line);             $$->insert(3, $1, $2, $3); }
   |  Exp LE Exp                          { $$ = new Node("Exp", "", @$.first_line);             $$->insert(3, $1, $2, $3); }
   |  Exp GT Exp                          { $$ = new Node("Exp", "", @$.first_line);             $$->insert(3, $1, $2, $3); }
   |  Exp GE Exp                          { $$ = new Node("Exp", "", @$.first_line);             $$->insert(3, $1, $2, $3); }
   |  Exp NE Exp                          { $$ = new Node("Exp", "", @$.first_line);             $$->insert(3, $1, $2, $3); }
   |  Exp EQ Exp                          { $$ = new Node("Exp", "", @$.first_line);             $$->insert(3, $1, $2, $3); }
   |  Exp PLUS Exp                        { $$ = new Node("Exp", "", @$.first_line);             $$->insert(3, $1, $2, $3); }
   |  Exp MINUS Exp                       { $$ = new Node("Exp", "", @$.first_line);             $$->insert(3, $1, $2, $3); }
   |  Exp MUL Exp                         { $$ = new Node("Exp", "", @$.first_line);             $$->insert(3, $1, $2, $3); }
   |  Exp DIV Exp                         { $$ = new Node("Exp", "", @$.first_line);             $$->insert(3, $1, $2, $3); }
   |  LP Exp RP                           { $$ = new Node("Exp", "", @$.first_line);             $$->insert(3, $1, $2, $3); }
   |  MINUS Exp                           { $$ = new Node("Exp", "", @$.first_line);             $$->insert(2, $1, $2); }
   |  NOT Exp                             { $$ = new Node("Exp", "", @$.first_line);             $$->insert(2, $1, $2); }
   |  ID LP Args RP                       { $$ = new Node("Exp", "", @$.first_line);             $$->insert(4, $1, $2, $3, $4); }
   |  ID LP RP                            { $$ = new Node("Exp", "", @$.first_line);             $$->insert(3, $1, $2, $3); }
   |  Exp LB Exp RB                       { $$ = new Node("Exp", "", @$.first_line);             $$->insert(4, $1, $2, $3, $4); }
   |  Exp DOT ID                          { $$ = new Node("Exp", "", @$.first_line);             $$->insert(3, $1, $2, $3); }
   |  ID                                  { $$ = new Node("Exp", "", @$.first_line);             $$->insert(1, $1); }
   |  INT                                 { $$ = new Node("Exp", "", @$.first_line);             $$->insert(1, $1); }
   |  FLOAT                               { $$ = new Node("Exp", "", @$.first_line);             $$->insert(1, $1); }
   |  CHAR                                { $$ = new Node("Exp", "", @$.first_line);             $$->insert(1, $1); }
   |  Exp LERROR Exp
   |  LERROR
   |  ID LP error                         { flag = 0; printf("Error type B at Line %d: Missing closing parenthesis ')'\n", @$.first_line); }
   ;
Args:
      Exp COMMA Args                      { $$ = new Node("Args", "", @$.first_line);            $$->insert(3, $1, $2, $3); }
   |  Exp                                 { $$ = new Node("Args", "", @$.first_line);            $$->insert(1, $1); }
   ;

%%

void yyerror (const char* s) {}

int main (int argc, char** argv) {
   if (argc != 2) {
      fprintf(stderr, "Usage: %s <file_path>\n", argv[0]);
      exit(-1);
   } else if (!(yyin = fopen(argv[1], "r"))) {
      perror(argv[1]);
      exit(-1);
   }
   yydebug = 0;
   yyparse();
   if (!root || !flag) 
      return 0;
   root->pre_traverse(0); 
   Parser parser;
   parser.parse_tree(root);
   return 0;
}
