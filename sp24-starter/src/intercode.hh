#ifndef __INTER__ 
#define __INTER__
#include "symbol_tb.hh"
#include <time.h>

struct operand{
    opkind kind;
    union{
      int val;
      char* name;  
    } u;
};

struct ircode
{
   irkind kind;
   union{
        struct {
            struct operand* op;
        } unop;
        struct {
            struct operand* right;
            struct operand* left;
        } assign;
        struct {
            struct operand* result;
            struct operand* op1;
            struct operand* op2;
        } binop;
        struct {
            struct operand* x;
            struct operand* relop;
            struct operand* y;
            struct operand* z;
        } ifsmt;
        struct {
            struct operand* op;
            int size;
        } dec;
        struct {
            struct operand* right;
            struct operand* left;
            int ofs;
        } raddr;
   }u;
};

struct ircodes {
    struct ircode *code;
    struct ircodes *prev;
    struct ircodes *next;
};

struct arg{
    struct operand* op;
    struct arg* next;
} ;

struct arglist {
    struct arg* head;
    struct arg* tail;
};

struct ircodeslist {
    struct ircodes* head;
    struct ircodes* tail;
    int tempnum;
    int labelnum;
};

struct reg
{
  int free;
  char* regname;
};

struct var_reg
{
   int reg_no;
   operand* op;
   struct var_reg* next;
};

struct var_reg_list
{
  struct var_reg* head;
  struct var_reg* tail;
};

struct reg_heap
{
   reg* regs[32];
   int lastinfo;
};

struct arr_list{
   char* arrname;
   struct arr_list* next; 
};

struct var_reg_tb
{
  struct var_reg_list* inreg;
  struct var_reg_list* inmem;
  struct arr_list* arrlist;
  int funcdep;
  int infun;
  char* funcname;
};

extern struct ircodeslist *ircodelist;

struct operand* newoperand(opkind kind,const char* val);
struct operand* newoperand(opkind kind,int val);
void printop(FILE* fp,struct operand* p);

struct ircode* newircode(irkind kind,struct operand* op);
struct ircode* newircode(irkind kind,struct operand* result,struct operand* op1,struct operand* op2);
struct ircode* newircode(irkind kind,struct operand* left,struct operand* right);
struct ircode* newircode(irkind kind,struct operand* x,struct operand* relop,struct operand* y,struct operand* z);
struct ircode* newircode(irkind kind,struct operand* op,int size);
struct ircodes* newircodes(struct ircode* src);
struct ircode* newircode(irkind kind,struct operand* result,struct operand* addr,int ofs);

Symbol* usesymbol(char* name);
Symbol* find_presymbol(char* name);
arg* newarg(operand* op);
arglist* newarglist();
void addarg(arglist*  arglist, arg* arg);

void ircodelistinit();
void addircodes(ircodes* ircodes);

operand* newtemp();
operand* newlabel();

void traverseir(ast* node);
void translatecomp(ast* node);
void translateglobal(ast* node);
void translatefunc(ast* node,stype* idtype);
void translateblock(ast* node,stype* idtype);
void translatesmt(ast* node,stype* idtype);
void translatevardefs(ast* node);
void translatevardef(ast* node);
void translateexp(ast* node,operand* op);
void translatecond(ast* node,operand* ltrue,operand* lfalse);

void genircode(irkind kind,struct operand* op);
void genircode(irkind kind,struct operand* result,struct operand* op1,struct operand* op2);
void genircode(irkind kind,struct operand* left,struct operand* right);
void genircode(irkind kind,struct operand* x,struct operand* relop,struct operand* y,struct operand* z);
void genircode(irkind kind,struct operand* op,int size);

void printircodelist(FILE* fp);




















#endif