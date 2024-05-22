#ifndef __TARGET__
#define __TARGET__

#include "intercode.hh"

void regsenvinit();
void addvarreg(var_reg_list* var_list, int reg_no, operand* op);
void delvarreg(var_reg_list* var_list, var_reg* varreg);
int usevarreg(FILE* fp, operand* op);
int regalloc(operand* op);

void printtargetlist(FILE* fp);
ircodes* printheadcode(FILE* fp);
int find_arrname(char* name);
void printtargetcode(FILE* fp,ircodes* codes);
void pushtmp(FILE* fp);
void poptmp(FILE* fp);


#endif