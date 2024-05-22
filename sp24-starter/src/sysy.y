%{
#include "ast_node.hh"

void yyerror(const char *s);
extern int yylex(void);
extern int deep;
extern int error_flag;
ast* root;
%}

%union {
    struct ast_node* node;
}

%token <node> VOID TYPE
%token <node> INTNUM
%token <node> ID 
%token <node> OR AND LBRACE LBRACKET LPARENT RBRACE RBRACKET RPARENT COMMA SEMICN   
%token <node> ADD SUB MUL DIV REM ASSIGN WHILE IF ELSE RET GT GE LT LE EQ NEQ NOT   

%type <node> Expr 
%type <node> Funcid Narraycall Funcparac Element IDvar
%type <node> CompUnit Vardef AnoVar Arraydef Arrayext Initgroup Ret GAnoVar
%type <node> Funcdef Funcpara Block Stmts Stmt StartFlag Elements GVardef     
 
%start StartFlag

%left OR
%left AND
%left EQ NEQ
%left LE LT GE GT
%left ADD SUB
%left MUL DIV REM
%left NOT

%%
StartFlag : CompUnit { $$ = new_node("StartFlag_"); add_child($$,$1); root = $$; }; 

CompUnit : GVardef CompUnit  { $$ = new_node("Compunit_"); add_child($$,$1); add_child($$,$2);  }
         | Funcdef CompUnit  { $$ = new_node("Compunit_"); add_child($$,$1); add_child($$,$2);  }
         | GVardef { $$ = new_node("Compunit_"); add_child($$,$1);  }
         | Funcdef { $$ = new_node("Compunit_"); add_child($$,$1);  };

GVardef : TYPE GAnoVar SEMICN { $$ = new_node("GVardef_"); add_child($$,$1); add_child($$,$2); add_child($$,$3);  }; 

GAnoVar :IDvar { $$ = new_node("AnoVar_"); add_child($$,$1); } | 
         IDvar COMMA GAnoVar  { $$ = new_node("AnoVar_"); add_child($$,$1); add_child($$,$2); add_child($$,$3);  }; 

Vardef : TYPE AnoVar SEMICN { $$ = new_node("Vardef_"); add_child($$,$1); add_child($$,$2); add_child($$,$3);  }; 

AnoVar : IDvar { $$ = new_node("AnoVar_"); add_child($$,$1); } | 
         IDvar COMMA AnoVar { $$ = new_node("AnoVar_"); add_child($$,$1); add_child($$,$2); add_child($$,$3);  }; 

IDvar : ID { $$ = new_node("IDvar_"); add_child($$,$1);   } |
        ID ASSIGN Expr { $$ = new_node("IDvar_"); add_child($$,$1); add_child($$,$2); add_child($$,$3); } | 
        Arraydef { $$ = new_node("IDvar_"); add_child($$,$1);  } | 
        Arraydef ASSIGN Initgroup { $$ = new_node("IDvar_"); add_child($$,$1); add_child($$,$2); add_child($$,$3);   } ;

// Init : Expr { $$ = new_node("Init_"); add_child($$,$1);  }; 

Arraydef : ID LBRACKET Expr RBRACKET  
        { $$ = new_node("Arraydef_"); add_child($$,$1); add_child($$,$2); add_child($$,$3); add_child($$,$4); } |
          ID LBRACKET Expr RBRACKET LBRACKET Expr RBRACKET 
        { $$ = new_node("Arraydef_"); add_child($$,$1); add_child($$,$2); add_child($$,$3); add_child($$,$4); 
        add_child($$,$5); add_child($$,$6); add_child($$,$7);  }  |
         ID LBRACKET Expr RBRACKET LBRACKET Expr RBRACKET LBRACKET Expr RBRACKET { $$ = new_node("Arraydef_"); add_child($$,$1); 
        add_child($$,$2); add_child($$,$3); add_child($$,$4); add_child($$,$5); add_child($$,$6); add_child($$,$7);
        add_child($$,$8); add_child($$,$9); add_child($$,$10);  };
 
Initgroup : LBRACE Elements RBRACE { $$ = new_node("Initgroup_"); add_child($$,$1); add_child($$,$2); add_child($$,$3);  } |
            LBRACE RBRACE { $$ = new_node("Initgroup_"); add_child($$,$1); add_child($$,$2);  };

Elements : Element COMMA Elements { $$ = new_node("Elements_"); add_child($$,$1); add_child($$,$2); add_child($$,$3); } |
           Element { $$ = new_node("Elements_"); add_child($$,$1); };

Element : Expr { $$ = new_node("Element_"); add_child($$,$1);  } | Initgroup { $$ = new_node("Element_"); add_child($$,$1);  };

Funcdef :  TYPE ID LPARENT RPARENT LBRACE Block RBRACE
            { $$ = new_node("Funcdef_"); add_child($$,$1); add_child($$,$2); add_child($$,$3); add_child($$,$4); add_child($$,$5);
              add_child($$,$6); add_child($$,$7);  }  |
           TYPE ID LPARENT Funcpara RPARENT LBRACE Block RBRACE
            { $$ = new_node("Funcdef_"); add_child($$,$1); add_child($$,$2); add_child($$,$3); add_child($$,$4); add_child($$,$5);
              add_child($$,$6); add_child($$,$7); add_child($$,$8); } |
           VOID ID LPARENT RPARENT LBRACE Block RBRACE
             { $$ = new_node("Funcdef_");  add_child($$,$1); add_child($$,$2); add_child($$,$3); add_child($$,$4); add_child($$,$5);
              add_child($$,$6); add_child($$,$7); } |
           VOID ID LPARENT Funcpara RPARENT LBRACE Block RBRACE
            { $$ = new_node("Funcdef_"); add_child($$,$1); add_child($$,$2); add_child($$,$3); add_child($$,$4); add_child($$,$5);
              add_child($$,$6); add_child($$,$7); add_child($$,$8);  };

Funcpara : TYPE Funcid { $$ = new_node("Funcpara_"); add_child($$,$1); add_child($$,$2);} |
           TYPE Funcid COMMA Funcpara { $$ = new_node("Funcpara_"); add_child($$,$1); add_child($$,$2); add_child($$,$3);
                             add_child($$,$4); };  

Funcid : ID { $$ = new_node("Funcid_"); add_child($$,$1); } |
         ID LBRACKET RBRACKET { $$ = new_node("Funcid_"); add_child($$,$1); add_child($$,$2); add_child($$,$3);} |
         ID LBRACKET RBRACKET LBRACKET INTNUM RBRACKET { $$ = new_node("Funcid_"); add_child($$,$1); add_child($$,$2); add_child($$,$3);
            add_child($$,$4);  add_child($$,$5);  add_child($$,$6); 
             };

Block :  Stmts  { $$ = new_node("Block_"); add_child($$,$1); };

Stmts :  Stmt { $$ = new_node("Stmts_"); add_child($$,$1);  } |
         Stmt Stmts {$$ = new_node("Stmts_");  add_child($$,$1);  add_child($$,$2); }  ;

Stmt :   SEMICN { $$ = new_node("Stmt_"); add_child($$,$1);  }|
         Expr SEMICN { $$ = new_node("Stmt_"); add_child($$,$1);  add_child($$,$2);  } |
         Vardef { $$ = new_node("Stmt_"); add_child($$,$1); } |
         Ret { $$ = new_node("Stmt_"); add_child($$,$1);  } |
         LBRACE Block RBRACE{ $$ = new_node("Stmt_"); add_child($$,$1); add_child($$,$2); add_child($$,$3); } |
         IF LPARENT Expr RPARENT Stmt 
         { $$ = new_node("Stmt_"); add_child($$,$1); add_child($$,$2); add_child($$,$3); add_child($$,$4); add_child($$,$5);} |
         IF LPARENT Expr RPARENT Stmt ELSE Stmt 
         { $$ = new_node("Stmt_");  add_child($$,$1); add_child($$,$2); add_child($$,$3); 
             add_child($$,$4); add_child($$,$5);   add_child($$,$6); add_child($$,$7); } |
         WHILE LPARENT Expr RPARENT Stmt 
         { $$ = new_node("Stmt_"); add_child($$,$1); add_child($$,$2); add_child($$,$3); add_child($$,$4); add_child($$,$5); } ;

Ret :    RET Expr SEMICN { $$ = new_node("Ret_"); add_child($$,$1); add_child($$,$2); add_child($$,$3); } |
         RET SEMICN { $$ = new_node("Ret_"); add_child($$,$1); add_child($$,$2); } ;
 
Expr :      Expr ASSIGN Expr { $$ = new_node("Expr_"); add_child($$,$1); add_child($$,$2); add_child($$,$3);} |
            Expr OR Expr { $$ = new_node("Expr_"); add_child($$,$1); add_child($$,$2); add_child($$,$3);   }|
            Expr AND Expr {$$ = new_node("Expr_");  add_child($$,$1); add_child($$,$2); add_child($$,$3);  } |
            Expr EQ Expr {$$ = new_node("Expr_"); add_child($$,$1); add_child($$,$2); add_child($$,$3);   } |
            Expr NEQ Expr {$$ = new_node("Expr_"); add_child($$,$1); add_child($$,$2); add_child($$,$3);   } |
            LPARENT Expr RPARENT{$$ = new_node("Expr_"); add_child($$,$1); add_child($$,$2); add_child($$,$3);   } |
            NOT Expr { $$ = new_node("Expr_"); add_child($$,$1); add_child($$,$2); } |
            ADD Expr { $$ = new_node("Expr_"); add_child($$,$1); add_child($$,$2); } |
            SUB Expr { $$ = new_node("Expr_"); add_child($$,$1); add_child($$,$2); } |
            Expr GT Expr {$$ = new_node("Expr_"); add_child($$,$1); add_child($$,$2); add_child($$,$3);   } |
            Expr GE Expr {$$ = new_node("Expr_"); add_child($$,$1); add_child($$,$2); add_child($$,$3);   } |
            Expr LE Expr {$$ = new_node("Expr_"); add_child($$,$1); add_child($$,$2); add_child($$,$3);   } | 
            Expr LT Expr {$$ = new_node("Expr_"); add_child($$,$1); add_child($$,$2); add_child($$,$3);   } |   
            Expr ADD Expr { $$ = new_node("Expr_");  add_child($$,$1); add_child($$,$2); add_child($$,$3); } |
            Expr SUB Expr { $$ = new_node("Expr_"); add_child($$,$1); add_child($$,$2); add_child($$,$3);  } |
            Expr MUL Expr { $$ = new_node("Expr_"); add_child($$,$1); add_child($$,$2); add_child($$,$3);  } |
            Expr DIV Expr { $$ = new_node("Expr_"); add_child($$,$1); add_child($$,$2); add_child($$,$3);  } |
            Expr REM Expr { $$ = new_node("Expr_"); add_child($$,$1); add_child($$,$2); add_child($$,$3);  } |
            ID { $$ = new_node("Expr_"); add_child($$,$1); } |                    //var
            ID Narraycall {$$ = new_node("Expr_"); add_child($$,$1); add_child($$,$2); } |   //array
            ID LPARENT RPARENT {$$ = new_node("Expr_"); add_child($$,$1); add_child($$,$2); add_child($$,$3);  } |  //func
            ID LPARENT Funcparac RPARENT {$$ = new_node("Expr_"); add_child($$,$1); add_child($$,$2);
                                           add_child($$,$3);  add_child($$,$4); } |
            INTNUM  { $$ = new_node("Expr_"); add_child($$,$1);  } ;                               
                                    

Narraycall : LBRACKET Expr RBRACKET {$$ = new_node("Narraycall_"); add_child($$,$1); add_child($$,$2); add_child($$,$3); } | 
             LBRACKET Expr RBRACKET LBRACKET Expr RBRACKET{$$ = new_node("Narraycall_");  add_child($$,$1); add_child($$,$2); add_child($$,$3); 
                                      add_child($$,$4);  add_child($$,$5);  add_child($$,$6); };

Funcparac : Expr { $$ = new_node("Funcparac_"); add_child($$,$1);  } | 
        Expr COMMA Funcparac  {$$ = new_node("Funcparac_"); add_child($$,$1); add_child($$,$2); add_child($$,$3);  };

%%

