#include "ast_node.hh"

struct ast_node* new_node(const char* name, Nodetype type,char* val){
   struct ast_node* node = (struct ast_node*)malloc(sizeof(struct ast_node));
   node->child = NULL;
   node->sibing = NULL;
   node->name = strdup(name);
   node->type = type;
   node->val = strdup(val);
   return node;
}

void add_child(struct ast_node* parent,struct ast_node* child){
    struct ast_node* p = parent->child;
    if(p == NULL) parent->child = child; 
    else{
     while(p->sibing != NULL){ p = p->sibing; }
     p->sibing = child; 
    }
}