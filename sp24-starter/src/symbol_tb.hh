#ifndef __SYMBOL__ 
#define __SYMBOL__
#include "ast_node.hh"
#define SIZE 109

typedef struct stype Type;

struct paralist
{  
   char* name;
   Type* type;
   struct paralist* next;
};

typedef struct stype
{
  Kind kind;
  union symbol_tb
  {
    Basictype basic;
    struct {
      struct stype* elem;
      int size[3];
    }array;
    struct {
      int args;
      struct paralist* params;
      struct stype* ret;
    }function;
  }u;
  
}Type;

typedef struct Symbol
{ 
   int depth;
   struct paralist* field;
   struct Symbol* nextsymbol;  
   struct Symbol* nexthash; 
}symbol;

struct stack
{
   struct Symbol* symbols[SIZE];
   int stackdepth;
};

struct mytable{
   struct Symbol* hashtable[SIZE];
   struct stack* auxstack;
};

extern struct mytable* table;

unsigned int hash(char* name);

struct stype* newbasic(Basictype basic);
struct stype* newarray(struct stype* elem, int size[]);
struct stype* newfunction(int args,struct paralist* params,struct stype* ret);
struct stype* copytype(struct stype* src);
void deltype(struct stype* p);
int checktype(struct stype* m,struct stype* n);

struct paralist* newparam(char* name, struct stype* params);
struct paralist* copyparam(struct paralist* src);

struct Symbol* newsymbol(int depth,struct paralist* field);
void delsymbol(struct Symbol* p);
int checksymbol(struct Symbol* p);

void inittable();
void insert(struct Symbol* p);
struct Symbol* lookup(char* name);
void delstack();

void traversetree(ast* root);
void transunit(ast* root);
struct stype* newidtype(ast* node);
void funcdec(ast *node,stype *idtype);
void varlistdec(ast *node,stype *idtype);
Symbol* vardec(ast *node,stype *idtype);
void transblock(ast *node,stype *idtype);
void Varlist(ast* node,Symbol* p);
paralist* paramdec(ast* node);
void transstms(ast* node,stype *idtype);
void transstm(ast* node,stype *idtype);
stype* transexp(ast* node);

#endif