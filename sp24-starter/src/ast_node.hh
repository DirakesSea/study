#ifndef __AST__ 
#define __AST__


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "enum.hh"

typedef struct ast_node
{ 
  struct ast_node *child;
  struct ast_node *sibing;  
  Nodetype type;
  char* name;
  char* val;
}ast;

struct ast_node* new_node(const char* name, Nodetype type=TOKEN_OTHER,char* val="");

void add_child(struct ast_node* parent,struct ast_node* child);


#endif