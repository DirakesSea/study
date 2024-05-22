#include <stdio.h>
#include "target_code.hh"
#include "sysy.tab.hh"

extern int yyparse();
extern int yylineno;
extern ast* root;
int error_flag = 0;

int main(int argc, char** argv){
    extern FILE* yyin;
    if(argc >= 2)  yyin = fopen(argv[1], "r");
    yylineno = 1;
    yyparse();
    //printf("parse done\n");
    fclose(yyin);  
    if(error_flag) return 1;
    if(argc == 2){
      inittable();  
      traversetree(root);
    }
    if(error_flag) return 1;
    if(argc == 3){ 
      FILE* yyout = fopen(argv[2],"wt+");
      ircodelistinit();
      regsenvinit(); 
      traverseir(root);
      if(argv[1][9] == '3'){
        printircodelist(yyout);
      }else if(argv[1][9] == '4'){
        printtargetlist(yyout); 
      }  

    }
   return 0;
}

void yyerror(const char *msg) {
    printf("Syntax Error Line %d: %s\n", yylineno, msg);
    error_flag = 1;
} 

//  ./compiler tests/lab1/
//  python3 test.py ./compiler lab1
