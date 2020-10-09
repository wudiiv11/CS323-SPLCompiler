%{
    #define YYSTYPE Node *
    #include "lex.yy.c"
    void yyerror (const char*);
    extern int flag;
    Node* root;
    int null_cnt = 0;
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
      ExtDefList                   { root = $$ = make_node("Program",         NULL, @$.first_line); insert(2, $$, $1);}
   ;
ExtDefList:
      ExtDef ExtDefList                   { $$ = make_node("ExtDefList",      NULL, @$.first_line); insert(3, $$, $1, $2); }
   |  %empty                              { $$ = make_node("NULL",            NULL, 0);}
   ;
ExtDef:
      Specifier ExtDecList SEMI           { $$ = make_node("ExtDef",          NULL, @$.first_line); insert(4, $$, $1, $2, $3); }
   |  Specifier SEMI                      { $$ = make_node("ExtDef",          NULL, @$.first_line); insert(3, $$, $1, $2); }
   |  Specifier FunDec CompSt             { $$ = make_node("ExtDef",          NULL, @$.first_line); insert(4, $$, $1, $2, $3); }
   ;
ExtDecList:
      VarDec                              { $$ = make_node("ExtDecList",      NULL, @$.first_line); insert(2, $$, $1); }
   |  VarDec COMMA ExtDecList             { $$ = make_node("ExtDecList",      NULL, @$.first_line); insert(4, $$, $1, $2, $3); }
   ;
Specifier:
      TYPE                                { $$ = make_node("Specifier",       NULL, @$.first_line); insert(2, $$, $1);}
   |  StructSpecifier                     { $$ = make_node("Specifier",       NULL, @$.first_line); insert(2, $$, $1); }
   ;
StructSpecifier:
      STRUCT ID LC DefList RC             { $$ = make_node("StructSpecifier", NULL, @$.first_line); insert(6, $$, $1, $2, $3, $4, $5); }
   |  STRUCT ID                           { $$ = make_node("StructSpecifier", NULL, @$.first_line); insert(3, $$, $1, $2); }
   ;
VarDec:
      ID                                  { $$ = make_node("VarDec",          NULL, @$.first_line); insert(2, $$, $1); }
   |  VarDec LB INT RB                    { $$ = make_node("VarDec",          NULL, @$.first_line); insert(5, $$, $1, $2, $3, $4); }
   ;
FunDec:
      ID LP VarList RP                    { $$ = make_node("FunDec",          NULL, @$.first_line); insert(5, $$, $1, $2, $3, $4); }
   |  ID LP RP                            { $$ = make_node("FunDec",          NULL, @$.first_line); insert(4, $$, $1, $2, $3); }
   |  ID LP error                         { flag = 0; printf("Error type B at Line %d: Missing closing parenthesis ')'\n", @$.first_line); }
   ;
VarList:
      ParamDec COMMA VarList              { $$ = make_node("VarList",         NULL, @$.first_line); insert(4, $$, $1, $2, $3); }
   |  ParamDec                            { $$ = make_node("VarList",         NULL, @$.first_line); insert(2, $$, $1); }
   ;
ParamDec:
      Specifier VarDec                    { $$ = make_node("ParamDec",        NULL, @$.first_line); insert(3, $$, $1, $2); }
   ;
CompSt:
      LC DefList StmtList RC              { $$ = make_node("CompSt",          NULL, @$.first_line); insert(5, $$, $1, $2, $3, $4); }
   ;
StmtList:
      Stmt StmtList                       { $$ = make_node("StmtList",        NULL, @$.first_line); insert(3, $$, $1, $2); }
   |  %empty                              { $$ = make_node("NULL",            NULL, @$.first_line); }
   ;
Stmt:
      Exp SEMI                            { $$ = make_node("Stmt",            NULL, @$.first_line); insert(3, $$, $1, $2); }
   |  CompSt                              { $$ = make_node("Stmt",            NULL, @$.first_line); insert(2, $$, $1); }
   |  RETURN Exp SEMI                     { $$ = make_node("Stmt",            NULL, @$.first_line); insert(4, $$, $1, $2, $3); }
   |  IF LP Exp RP Stmt                   { $$ = make_node("Stmt",            NULL, @$.first_line); insert(6, $$, $1, $2, $3, $4, $5); }
   |  IF LP Exp RP Stmt ELSE Stmt         { $$ = make_node("Stmt",            NULL, @$.first_line); insert(8, $$, $1, $2, $3, $4, $5, $6, $7); }
   |  WHILE LP Exp RP Stmt                { $$ = make_node("Stmt",            NULL, @$.first_line); insert(6, $$, $1, $2, $3, $4, $5); }
   |  RETURN error                        { flag = 0; printf("Error type B at Line %d: Missing semicolon ';'\n", @$.first_line); }
   |  RETURN LERROR SEMI 
   ;
DefList:
      Def DefList                         { $$ = make_node("DefList",         NULL, @$.first_line); insert(3, $$, $1, $2); }
   |  %empty                              { $$ = make_node("NULL",            NULL, @$.first_line); }
   ;
Def:
      Specifier DecList SEMI              { $$ = make_node("Def",             NULL, @$.first_line); insert(4, $$, $1, $2, $3); }
   |  Specifier LERROR SEMI
   ;
DecList:
      Dec                                 { $$ = make_node("DecList",         NULL, @$.first_line); insert(2, $$, $1); }
   |  Dec COMMA DecList                   { $$ = make_node("DecList",         NULL, @$.first_line); insert(4, $$, $1, $2, $3); }
   ;
Dec:
      VarDec                              { $$ = make_node("Dec",             NULL, @$.first_line); insert(2, $$, $1); }
   |  VarDec ASSIGN Exp                   { $$ = make_node("Dec",             NULL, @$.first_line); insert(4, $$, $1, $2, $3); }
   |  VarDec ASSIGN LERROR
   ;
Exp: 
      Exp ASSIGN Exp                      { $$ = make_node("Exp",             NULL, @$.first_line); insert(4, $$, $1, $2, $3); }
   |  Exp AND Exp                         { $$ = make_node("Exp",             NULL, @$.first_line); insert(4, $$, $1, $2, $3); }
   |  Exp OR Exp                          { $$ = make_node("Exp",             NULL, @$.first_line); insert(4, $$, $1, $2, $3); }
   |  Exp LT Exp                          { $$ = make_node("Exp",             NULL, @$.first_line); insert(4, $$, $1, $2, $3); }
   |  Exp LE Exp                          { $$ = make_node("Exp",             NULL, @$.first_line); insert(4, $$, $1, $2, $3); }
   |  Exp GT Exp                          { $$ = make_node("Exp",             NULL, @$.first_line); insert(4, $$, $1, $2, $3); }
   |  Exp GE Exp                          { $$ = make_node("Exp",             NULL, @$.first_line); insert(4, $$, $1, $2, $3); }
   |  Exp NE Exp                          { $$ = make_node("Exp",             NULL, @$.first_line); insert(4, $$, $1, $2, $3); }
   |  Exp EQ Exp                          { $$ = make_node("Exp",             NULL, @$.first_line); insert(4, $$, $1, $2, $3); }
   |  Exp PLUS Exp                        { $$ = make_node("Exp",             NULL, @$.first_line); insert(4, $$, $1, $2, $3); }
   |  Exp MINUS Exp                       { $$ = make_node("Exp",             NULL, @$.first_line); insert(4, $$, $1, $2, $3); }
   |  Exp MUL Exp                         { $$ = make_node("Exp",             NULL, @$.first_line); insert(4, $$, $1, $2, $3); }
   |  Exp DIV Exp                         { $$ = make_node("Exp",             NULL, @$.first_line); insert(4, $$, $1, $2, $3); }
   |  LP Exp RP                           { $$ = make_node("Exp",             NULL, @$.first_line); insert(4, $$, $1, $2, $3); }
   |  MINUS Exp                           { $$ = make_node("Exp",             NULL, @$.first_line); insert(3, $$, $1, $2); }
   |  NOT Exp                             { $$ = make_node("Exp",             NULL, @$.first_line); insert(3, $$, $1, $2); }
   |  ID LP Args RP                       { $$ = make_node("Exp",             NULL, @$.first_line); insert(5, $$, $1, $2, $3, $4); }
   |  ID LP RP                            { $$ = make_node("Exp",             NULL, @$.first_line); insert(4, $$, $1, $2, $3); }
   |  Exp LB Exp RB                       { $$ = make_node("Exp",             NULL, @$.first_line); insert(5, $$, $1, $2, $3, $4); }
   |  Exp DOT ID                          { $$ = make_node("Exp",             NULL, @$.first_line); insert(4, $$, $1, $2, $3); }
   |  ID                                  { $$ = make_node("Exp",             NULL, @$.first_line); insert(2, $$, $1); }
   |  INT                                 { $$ = make_node("Exp",             NULL, @$.first_line); insert(2, $$, $1); }
   |  FLOAT                               { $$ = make_node("Exp",             NULL, @$.first_line); insert(2, $$, $1); }
   |  CHAR                                { $$ = make_node("Exp",             NULL, @$.first_line); insert(2, $$, $1); }
   |  ID LP error                         { flag = 0; printf("Error type B at Line %d: Missing closing parenthesis ')'\n", @$.first_line); }
   |  Exp LERROR Exp
   ;
Args:
      Exp COMMA Args                      { $$ = make_node("Args",            NULL, @$.first_line); insert(4, $$, $1, $2, $3); }
   |  Exp                                 { $$ = make_node("Args",            NULL, @$.first_line); insert(2, $$, $1); }
   ;

%%

void yyerror (const char* s) {}

void print_node (Node* node, int num) {
   while(num--) printf("  ");
   char* name = node->name;
   printf("%s", name);
   if (node->child_list_head->next) 
      printf(" (%d)", node->line_no);
   if (node->text) {
      if (!strcmp(name, "CHAR"))
         printf(": %c", node->text[1]);
      else
         printf(": %s", node->text);
   }
   printf("\n");
}

void traverse_tree(Node* root, int depth) {
   print_node(root, depth);
   Node *p = root->child_list_head->next;
   while (p) {
      traverse_tree(p, depth + 1);
      p = p->next;
   }
}

int main (int argc, char** argv) {
   if (argc != 2) {
      fprintf(stderr, "Usage: %s <file_path>\n", argv[0]);
      exit(-1);
   } else if (!(yyin = fopen(argv[1], "r"))) {
      perror(argv[1]);
      exit(-1);
   }
   yydebug = 1;
   yyparse();
   if (root && flag) 
      traverse_tree(root, 0);
   return 0;
}
