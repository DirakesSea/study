#include "symbol_tb.hh"

struct mytable* table;
extern int error_flag;
unsigned int hash(char* name){
   unsigned int h = 0;
   char *s;
   for(s=name;*s;s++)
    h = h * 65599 + *s;
   return h; 
}

struct stype* newbasic(Basictype basic){
   struct stype* p = (struct stype*)malloc(sizeof(struct stype));
   p->kind = BASIC;
   p->u.basic = basic;
   return p;
}

struct stype* newarray(struct stype* elem, int size[]){
   struct stype* p = (struct stype*)malloc(sizeof(struct stype));
   p->kind = ARRAY;
   p->u.array.elem = elem;
   int i;
   for(i=0;i<3;i++) p->u.array.size[i] = size[i];
   return p;
}

struct stype* newfunction(int args,struct paralist* params,struct stype* ret){
   struct stype* p = (struct stype*)malloc(sizeof(struct stype));
   p->kind = FUNCTION;
   p->u.function.args = args;
   if(params) p->u.function.params = params;  else p->u.function.params = NULL;
   if(ret) p->u.function.ret = ret; else p->u.function.ret = NULL;
   return p;
}

stype *copytype(stype *src)
{  
   struct stype* p = (struct stype*)malloc(sizeof(struct stype));
   p->kind = src->kind;
   if(src->kind == BASIC){
     p->u.basic = src->u.basic;
   }else if(src->kind == ARRAY){
     int i;
     for(i=0;i<3;i++) p->u.array.size[i] = src->u.array.size[i];
     p->u.array.elem = copytype(src->u.array.elem);
   }else if(src->kind == FUNCTION){
     p->u.function.args = src->u.function.args;
     p->u.function.ret =  copytype(src->u.function.ret);
     p->u.function.params = copyparam(src->u.function.params);
   }
   return p;
}

void deltype(struct stype* p){
   if(p->kind == ARRAY){
     deltype(p->u.array.elem);
   }else if(p->kind == FUNCTION){
     deltype(p->u.function.ret);
     struct paralist* tmp = p->u.function.params;
     while(tmp){
      struct paralist* k = tmp->next;
      deltype(tmp->type);
      tmp = k; 
     }
   }
  free(p); 
}

int checktype(struct stype* m,struct stype* n){
   if(m == NULL || n == NULL) return 1;
   if(m->kind == FUNCTION || n->kind == FUNCTION || m->kind != n->kind) return 0;
   if(m->kind == BASIC) return m->u.basic == n->u.basic;
   else if(m->kind == ARRAY)  return checktype(m->u.array.elem,n->u.array.elem);
   return 0;
}

struct paralist* copyparam(struct paralist* src){
   struct paralist* cur = NULL;
   struct paralist* head = NULL;
   struct paralist* p = src;
   while(p){
     if(!head){
        head = newparam(p->name, p->type);
        cur = head;
        p = p->next;
     } else {
        cur->next = newparam(p->name, p->type);
        cur = cur->next;
        p = p->next;
     }  
   }
   return head; 
}

struct paralist* newparam(char* name,struct stype* params){
    struct paralist* p = (struct paralist*)malloc(sizeof(struct paralist));
    p->name = strdup(name);
    if(params) p->type = copytype(params);
    else p->type = NULL;
    p->next = NULL;
    return p;
}

struct Symbol* newsymbol(int depth,struct paralist* field){
   struct Symbol* p = (struct Symbol*)malloc(sizeof(struct Symbol));
   p->depth = depth;
   p->field = field;
   p->nexthash = NULL;
   p->nextsymbol = NULL;
   return p;
}

void delsymbol(struct Symbol* p){
   if(p->field) free(p->field);
   free(p);
}

int checksymbol(Symbol *p)  //-1:no find   0:no fit   1:perfect   
{  
   Symbol *tmp = lookup(p->field->name);
   if(tmp == NULL) return 0;
   else{
     while(tmp){
      if(!strcmp(p->field->name,tmp->field->name) && table->auxstack->stackdepth == tmp->depth
        && tmp->field->type->kind == p->field->type->kind) 
        return 1;
      tmp = tmp->nexthash;
     } 
   }
   return 0;
}

void inittable(){
   int i;
   table = (struct mytable*)malloc(sizeof(struct mytable));
   table->auxstack = (struct stack*)malloc(sizeof(struct stack) * SIZE);
   for(i=0;i<SIZE;i++){ 
     table->hashtable[i] = (struct Symbol* )malloc(sizeof(struct Symbol));
     table->hashtable[i] = NULL; 
     table->auxstack->symbols[i] = (struct Symbol* )malloc(sizeof(struct Symbol));
     table->auxstack->symbols[i] = NULL;
   }  
   table->auxstack->stackdepth = 0;
   stype* ret = (stype* )malloc(sizeof(stype));
   ret->kind = BASIC;
   ret->u.basic = INT_TYPE;
   Symbol* fread = newsymbol(table->auxstack->stackdepth,newparam((char*)"read",newfunction(0,NULL,ret)));
   paralist* para = newparam((char*)"t",ret);
   ret->u.basic = VOID_TYPE;
   Symbol* fwrite = newsymbol(table->auxstack->stackdepth,newparam((char*)"write",newfunction(1,para,ret)));
   insert(fread);
   insert(fwrite);
}

void insert(struct Symbol* p){
  int index = hash(p->field->name) % SIZE;
  p->nexthash = table->hashtable[index];
  table->hashtable[index] = p;
  p->nextsymbol = table->auxstack->symbols[table->auxstack->stackdepth];
  table->auxstack->symbols[table->auxstack->stackdepth] = p;
}

struct Symbol* lookup(char* name){
   int index = hash(name) % SIZE;
   struct Symbol* res = table->hashtable[index]; 
   for(;res;res = res->nexthash){
      if(!strcmp(res->field->name,name)) return res;
   }
   return NULL;
}

void delstack(){
   if(table->auxstack->stackdepth == 0) return;
   struct Symbol* temp = table->auxstack->symbols[table->auxstack->stackdepth]; 
  // printf("dep:%d\n",table->auxstack->stackdepth);
   while (temp) {
        struct Symbol* k = temp;
        temp = temp->nextsymbol;
        int index = hash(k->field->name) % SIZE;
        if (k == table->hashtable[index])
          table->hashtable[index] = k->nexthash;
        else {
         struct Symbol* cur = table->hashtable[index];
         if(!cur) continue;
         struct Symbol* last = cur;
         while (cur != k) {
               last = cur;
               cur = cur->nexthash;
        }
        last->nexthash = cur->nexthash;
        cur->nexthash = NULL;
       }
      free(k->field);
    }
    table->auxstack->symbols[table->auxstack->stackdepth] = NULL;
    table->auxstack->stackdepth--;
}

void traversetree(ast *root)
{
   if(root == NULL) return;
   if(!strcmp(root->name,"GVardef_") || !strcmp(root->name,"Funcdef_"))
      transunit(root);
   traversetree(root->child);
   traversetree(root->sibing);
}

void transunit(ast* root){  //GVardef Funcdef
  stype *idtype = newidtype(root->child);
  char* name = root->child->sibing->sibing->name;
  if(!strcmp(name,"lparent")){
     funcdec(root->child->sibing,idtype);
     ast* node = root->child->sibing->sibing;
     while(strcmp(node->name,"Block_") != 0) 
        node = node->sibing;
     table->auxstack->stackdepth++;   
     transblock(node ,idtype);
     delstack();
  }else{  
     varlistdec(root->child->sibing,idtype);
  }
}

stype *newidtype(ast *node)
{
   if(!strcmp(node->name,"void")){
     return newbasic(VOID_TYPE);
   }
     return newbasic(INT_TYPE);
}

void varlistdec(ast *node,stype *idtype){  //GAnoVar
//printf("call vardef\n");
    ast* tmp = node->child;
    while(tmp){   
      Symbol* syb = vardec(tmp,idtype);   
      if(checksymbol(syb) == 1){
        printf("redefine gvar\n");
        error_flag = 1;
      }else{
        insert(syb);
      //  printf("new id:%s depth:%d\n",syb->field->name,syb->depth);
      }
     if(tmp->sibing){ tmp = tmp->sibing->sibing->child;   }
     else{ break; }
    }
}

Symbol* vardec(ast *node, stype *idtype) //IDvar
{
   ast* id = node->child;  
   ast* grd = id;
   while(grd->child) grd = grd->child;
   Symbol* sym = newsymbol(table->auxstack->stackdepth,newparam(grd->val,NULL));
   if(!strcmp(id->name,"id") && strcmp(node->name,"Funcid_") != 0){
     sym->field->type = copytype(idtype);
     if(id->sibing){

     }
   }else if(!strcmp(node->name,"Funcid_")){
     if(!node->child->sibing){
      sym->field->type = copytype(idtype);
     }
     else if(!node->child->sibing->sibing->sibing){
        int size[3] = {50,0,0};
        sym->field->type = newarray(idtype,size); 
     }else{
        int size[3] = {50,0,0};
        size[1] = atoi(node->child->sibing->sibing->sibing->sibing->val);
       // printf("param:%s size1:%d\n",node->child->val,size[1]);
        sym->field->type = newarray(idtype,size); 
     } 
   }else{
     int count = 1;
     int size[3] = {0,0,0};
     size[0] = atoi(grd->sibing->sibing->child->val);
     ast* tmp = grd->sibing->sibing->sibing;
     while(tmp->sibing){
       size[count++] = atoi(tmp->sibing->sibing->child->val);
       tmp = tmp->sibing->sibing->sibing;
     }
     sym->field->type = newarray(idtype,size); 
     sym->field->name = grd->val;
     sym->field->next = NULL;
     if(id->sibing){
     } 
   }
  return sym;
}

void funcdec(ast *node,stype *idtype){ //ID
   Symbol* p = newsymbol(table->auxstack->stackdepth,newparam(node->val,newfunction(0,NULL,idtype)));
   if(!strcmp(node->sibing->sibing->name,"Funcpara_")){
      Varlist(node->sibing->sibing,p);
   }
   if(checksymbol(p) == 1){
        printf("redefine func\n");
        error_flag = 1;
      }else{
        insert(p);
    }
}

void Varlist(ast* node,Symbol* p){ //Funcpara_
    table->auxstack->stackdepth++;
    int argc = 1;
    ast* temp = node->child;
    paralist* cur = NULL;
    paralist* paramDec = paramdec(temp);
    p->field->type->u.function.params = copyparam(paramDec);
    cur = p->field->type->u.function.params;
    while (temp->sibing->sibing) {
        temp = temp->sibing->sibing->sibing->child;
        paramDec = paramdec(temp);
        if (paramDec) {
            cur->next = copyparam(paramDec);
            cur = cur->next;
            argc++;
        }
    }
    p->field->type->u.function.args = argc;
    table->auxstack->stackdepth--;
}

paralist* paramdec(ast* node){ //TYPE Funcid
   stype* idtype = newidtype(node);
   Symbol* sym = vardec(node->sibing,idtype);
   if(checksymbol(sym) == 1){
        printf("redefine lvar\n");
        error_flag = 1;
        return NULL;
      }else{
        insert(sym);
      //  printf("param def:%s  %d\n",sym->field->name,sym->depth);
        return sym->field;
      }
}

void transblock(ast *node, stype *idtype) //Block
{  
   //table->auxstack->stackdepth++;
   transstms(node->child,idtype); 
   //delstack();
}

void transstms(ast* node,stype *idtype){
   while(node){
      transstm(node->child,idtype);
      node = node->child->sibing;
   }
}

void transstm(ast* node,stype *idtype){  //Stmt 
    ast* p = node->child;
    stype* etype = NULL;
    if(!strcmp(p->name,"semicn")){
    
    }else if(!strcmp(p->name,"Expr_")){
      etype = transexp(p);
    }else if(!strcmp(p->name,"Vardef_")){
      etype = newidtype(p->child);
      varlistdec(p->child->sibing,etype);
    }else if(!strcmp(p->name,"Varcall_")){
      
    }else if(!strcmp(p->name,"Funcall_")){

    }else if(!strcmp(p->name,"Ret_")){
       if(!strcmp(p->child->sibing->name,"semicn")){
          if(idtype->kind == BASIC && idtype->u.basic != VOID_TYPE){
            printf("func type error\n");
            error_flag = 1;
          }
       }else{
          if(idtype->kind == BASIC && idtype->u.basic != INT_TYPE){
            printf("func type error\n");
            error_flag = 1;
          }else{
           etype = transexp(p->child->sibing);
          }
       }   
    }else if(!strcmp(p->name,"lbrace")){    
       transblock(p->sibing,idtype);    
    }else if(!strcmp(p->name,"if")){
       ast* smt = p->sibing->sibing->sibing->sibing;
       etype = transexp(p->sibing->sibing);
       transstm(smt,idtype);
       if(smt->sibing) transstm(smt->sibing->sibing,idtype);
    }else if(!strcmp(p->name,"while")){
       ast* smt = p->sibing->sibing->sibing->sibing;
       etype = transexp(p->sibing->sibing);
       transstm(smt,idtype);
    }else if(!strcmp(p->name,"continue")){
     
    }else if(!strcmp(p->name,"break")){

    }else if(!strcmp(p->name,"write")){
       
    }else if(!strcmp(p->name,"read")){

    }else{
      printf("error stmt\n");
      error_flag = 1; 
    } 
}

stype* transexp(ast* node){ 
    return NULL;
}