#include "intercode.hh"
int depth = 0;
extern struct var_reg_tb* varregtb;

Symbol* usesymbol(char* name){ 
   int index = hash(name) % SIZE;
   struct Symbol* res = table->hashtable[index]; 
   for(;res;res = res->nexthash){
      if(!strcmp(res->field->name,name) && depth >= res->depth) return res;
   }
   return NULL;
}

Symbol* find_presymbol(char* name){
   int index = hash(name) % SIZE;
   struct Symbol* res = table->hashtable[index]; 
   for(;res;res = res->nexthash){
      if(!strcmp(res->field->name,name) && depth > res->depth)  return res;
   }
   return NULL;
}

struct operand* newoperand(opkind kind,int val){
   struct operand* p = (struct operand*)malloc(sizeof(struct operand));
   p->kind = OP_CONST;
   p->u.val = val;
   return p;
}

struct operand* newoperand(opkind kind,const char* val){
    struct operand* p = (struct operand*)malloc(sizeof(struct operand));
    switch (kind)
    {
    case OP_VAR:
        p->kind = OP_VAR;
        p->u.name = strdup(val);
        break;
    case OP_LABEL:
        p->kind = OP_LABEL;
        p->u.name = strdup(val);
        break;
    case OP_FUNC:
        p->kind = OP_FUNC;
        p->u.name = strdup(val);
        break;
    case OP_RELOP:
        p->kind = OP_RELOP;
        p->u.name = strdup(val);
        break;
     default:
        break;
    }
   return p; 
}

void printop(FILE* fp,operand *p)
{  
   if(p == NULL) return;
   if(p->kind == OP_CONST)
     fprintf(fp, "#%d",p->u.val);
   else
     fprintf(fp, "%s", p->u.name); 
}

struct ircodeslist *ircodelist;

struct ircode* newircode(irkind kind,struct operand* op){
   struct ircode* r = (struct ircode*)malloc(sizeof(struct ircode));
   r->kind = kind;
   r->u.unop.op = op;
   return r;
}

struct ircode* newircode(irkind kind,struct operand* result,struct operand* op1,struct operand* op2){
   struct ircode* r = (struct ircode*)malloc(sizeof(struct ircode));
   r->kind = kind;
   r->u.binop.result = result;
   r->u.binop.op1 = op1;
   r->u.binop.op2 = op2;
   return r;
}

struct ircode* newircode(irkind kind,struct operand* left,struct operand* right){
   struct ircode* r = (struct ircode*)malloc(sizeof(struct ircode));
   r->kind = kind;
   r->u.assign.left = left;
   r->u.assign.right = right;
   return r;
}

struct ircode* newircode(irkind kind,struct operand* x,struct operand* relop,struct operand* y,struct operand* z){
   struct ircode* r = (struct ircode*)malloc(sizeof(struct ircode));
   r->kind = kind;
   r->u.ifsmt.x = x;
   r->u.ifsmt.relop = relop;
   r->u.ifsmt.y = y;
   r->u.ifsmt.z = z;
   return r;
}

struct ircode* newircode(irkind kind,struct operand* op,int size){
   struct ircode* r = (struct ircode*)malloc(sizeof(struct ircode));
   r->kind = kind;
   r->u.dec.op = op;
   r->u.dec.size = size;
   return r;
}

struct ircode* newircode(irkind kind,struct operand* result,struct operand* addr,int ofs){
   struct ircode* r = (struct ircode*)malloc(sizeof(struct ircode));
   r->kind = kind;
   r->u.raddr.left = result;
   r->u.raddr.right = addr;
   r->u.raddr.ofs = 4 * ofs;
   return r;
}

ircodes *newircodes(ircode *src)
{
   ircodes *p = (ircodes *)malloc(sizeof(ircodes));
   p->code = src;
   p->next = NULL;
   p->prev = NULL;
   return p;
}

arg* newarg(operand* op){
  arg* p = (arg*)malloc(sizeof(arg));
  p->op = op;
  p->next = NULL;
  return p;
}

arglist* newarglist(){
  arglist* p = (arglist*)malloc(sizeof(arglist));
  p->tail = NULL;
  p->head = NULL;
  return p;
}

void addarg(arglist*  arglist, arg* arg){
  if(arglist->head == NULL){
    arglist->head = arg;
    arglist->tail = arg;
  }else{
    arglist->tail->next = arg;
    arglist->tail = arg;
  }
}

void ircodelistinit(){
   inittable();
   ircodelist = (ircodeslist*)malloc(sizeof(ircodeslist));
   ircodelist->labelnum = 0;
   ircodelist->tempnum = 0;
   ircodelist->head = NULL;
   ircodelist->tail = NULL;
}

void addircodes(ircodes* ircodes){
  if(ircodelist->head == NULL){
    ircodelist->head = ircodes;
    ircodelist->tail = ircodes;
  }else{
    ircodelist->tail->next = ircodes;
    ircodes->prev = ircodelist->tail;
    ircodelist->tail = ircodes;
  }
}  

operand* newtemp(){
   char tmp[5] = {0};
   if(ircodelist->tempnum < 10){
    tmp[0] = 't';
    tmp[1] = '0' + ircodelist->tempnum;
    tmp[2] = '\0'; 
   }else if(ircodelist->tempnum < 100){
    tmp[0] = 't';
    tmp[1] = '0' + ircodelist->tempnum/10;
    tmp[2] = '0' + ircodelist->tempnum%10;
    tmp[3] = '\0'; 
   }else{
    tmp[0] = 't';
    tmp[1] = '0' + ircodelist->tempnum / 100;
    tmp[2] = '0' + ircodelist->tempnum % 100 / 10;
    tmp[3] = '0' + ircodelist->tempnum % 10;
    tmp[4] = '\0';   
   }
   operand* p = newoperand(OP_VAR,tmp);
   ircodelist->tempnum++;
   return p;
}

operand* newlabel(){
   char lbs[10] = {0};
   lbs[0] = 'l';
   lbs[1] = 'a';
   lbs[2] = 'b';
   lbs[3] = 'e';
   lbs[4] = 'l';
   if(ircodelist->labelnum < 10){
    lbs[5] = '0' + ircodelist->labelnum;
    lbs[6] = '\0'; 
   }else{
    lbs[5] = '0' + ircodelist->labelnum/10;
    lbs[6] = '0' + ircodelist->labelnum%10;
    lbs[7] = '\0'; 
   } 
   operand* p = newoperand(OP_LABEL,lbs);
   ircodelist->labelnum++;
   return p;
}

void traverseir(ast *node){
   if(node == NULL) return;
   if(!strcmp(node->name,"Compunit_")){   
    translatecomp(node);
   }
   else{
      traverseir(node->child);
      traverseir(node->sibing);
   }
}

void translatecomp(ast* node){   //Compunit
   while(node){ 
     if(!strcmp(node->child->name,"GVardef_")){ 
      stype *idtype = newidtype(node->child->child);
      varlistdec(node->child->child->sibing,idtype);
      translateglobal(node->child);
     }
     else{  
      stype *idtype = newidtype(node->child->child);
      funcdec(node->child->child->sibing,idtype);
      translatefunc(node->child,idtype);
     }
     node = node->child->sibing; 
   }
}  

void genircode(irkind kind,struct operand* op){
   ircodes* codes = newircodes(newircode(kind,op));
   addircodes(codes);
}
 
void genircode(irkind kind,struct operand* result,struct operand* op1,struct operand* op2){
    ircodes* codes = newircodes(newircode(kind,result,op1,op2));
    addircodes(codes);             
}

void genircode(irkind kind,struct operand* result,struct operand* addr,int ofs){
   ircodes* codes = newircodes(newircode(kind,result,addr,ofs));
   addircodes(codes);          
}

void genircode(irkind kind,struct operand* left,struct operand* right){
   if(kind == IR_MEM_WRITE && right->kind == OP_FUNC){
     operand* tmp = newtemp();
     ircodes* codes1 = newircodes(newircode(IR_ASSIGN,tmp,right));
     ircodes* codes2 = newircodes(newircode(kind,left,tmp));
     addircodes(codes1);
     addircodes(codes2);
   }else{
     ircodes* codes = newircodes(newircode(kind,left,right));
     addircodes(codes);   
   } 
    
}

void genircode(irkind kind,struct operand* x,struct operand* relop,struct operand* y,struct operand* z){
     ircodes* codes = newircodes(newircode(kind,x,relop,y,z));
     addircodes(codes); 
}

void genircode(irkind kind,struct operand* op,int size){
     ircodes* codes = newircodes(newircode(kind,op,size));
     addircodes(codes); 
}

void translateglobal(ast* node){  //GVardef
  ast* ident = node->child->sibing->child->child;
  ast* grd = ident;
  if(strcmp(ident->name,"id") != 0) grd = grd->child;
  Symbol* syb = lookup(grd->val);
  if(syb->field->type->kind == BASIC){
    if(!grd->sibing){            //id
      operand* t1 = newoperand(OP_VAR,grd->val);
      genircode(IR_GLOBAL,t1);
      operand* zero = newoperand(OP_CONST,0);
      genircode(IR_WORD,zero);
    }else{       //id assign init
      operand* t1 = newoperand(OP_VAR,grd->val);
      genircode(IR_GLOBAL,t1);
      operand* zero = newoperand(OP_CONST,atoi(grd->sibing->sibing->child->val));
      genircode(IR_WORD,zero);
    } 
  }else{
    int idx = 0;
    int dim0 = syb->field->type->u.array.size[0];
    int dim1 = syb->field->type->u.array.size[1];
    int dim2 = syb->field->type->u.array.size[2];
    operand* op = newoperand(OP_VAR,grd->val);
    genircode(IR_GLOBAL,op);
    if(dim1 == 0){
      if(!ident->sibing){
        for(int i=0;i<dim0;i++){
         operand* t1 = newoperand(OP_CONST,0);
         genircode(IR_WORD,t1);
        }
      }else{
        ast* ele = ident->sibing->sibing->child->sibing->child;
        operand* t1 = newoperand(OP_CONST,atoi(ele->child->child->val));
        genircode(IR_WORD,t1);
        idx++;
        while(ele->sibing){
          ele = ele->sibing->sibing->child;
          t1 = newoperand(OP_CONST,atoi(ele->child->child->val));
          genircode(IR_WORD,t1);
          idx++;
        }
        for(;idx<dim0;idx++){
          t1 = newoperand(OP_CONST,0);
          genircode(IR_WORD,t1);
        }
      } 
    }else if(dim2 == 0){
      if(!ident->sibing){
        for(int i=0;i<dim0*dim1;i++){
         operand* t1 = newoperand(OP_CONST,0);
         genircode(IR_WORD,t1);
        }
      }else{
        ast* ele = ident->sibing->sibing->child->sibing->child;
        operand* t1 = newoperand(OP_CONST,atoi(ele->child->child->val));
        genircode(IR_WORD,t1);
        idx++;
        while(ele->sibing){
          ele = ele->sibing->sibing->child;
          if(!strcmp(ele->child->name,"Expr_")){
            t1 = newoperand(OP_CONST,atoi(ele->child->child->val));
            genircode(IR_WORD,t1);
            idx++;
          }else{
            int rem = idx % dim1;
            for(int i=0;i<rem;i++){
             t1 = newoperand(OP_CONST,0);
             genircode(IR_WORD,t1);  
            }
            idx += rem;
            ast* subgroup = ele->child->child->sibing->child;
            operand* t1 = newoperand(OP_CONST,atoi(subgroup->child->child->val));
            genircode(IR_WORD,t1);
            int subidx = 1;
            idx++;
            while(subgroup->sibing){
               subgroup = subgroup->sibing->sibing->child;
               t1 = newoperand(OP_CONST,atoi(subgroup->child->child->val));
               genircode(IR_WORD,t1);
               idx++;
               subidx++;
            }
               for(;subidx<dim1;subidx++,idx++){
                  t1 = newoperand(OP_CONST,0);
                  genircode(IR_WORD,t1);
               } 
            }
        }
        for(;idx<dim0*dim1;idx++){
          t1 = newoperand(OP_CONST,0);
          genircode(IR_WORD,t1);
        }
      } 
    }else{  //three dimension array
    }
  } 
}

void translatefunc(ast* node,stype *idtype){  //Funcdef
  depth++;
  varregtb->infun = 1;
  varregtb->funcname = strdup(node->child->sibing->val);
  genircode(IR_FUNC,newoperand(OP_FUNC,node->child->sibing->val));
  Symbol* syb = lookup(node->child->sibing->val);
  paralist* param = syb->field->type->u.function.params;
  while(param){
      Symbol* syb = usesymbol(param->name);
      Symbol* pre = find_presymbol(param->name);
      if((pre && (pre != syb || 
         (pre == syb && depth > syb->depth && pre->nexthash && !strcmp(pre->nexthash->field->name,param->name))))
         || (syb && !strcmp(varregtb->funcname,"print_array") && (!strcmp(syb->field->name,"a") && syb->field->type->kind == ARRAY)) ) 
          genircode(IR_PARAM,newoperand(OP_VAR,strcat(param->name,(const char*)"2a")));
      else  
          genircode(IR_PARAM,newoperand(OP_VAR,param->name));
    param = param->next;
  }
  ast* blk = node->child;
  while(strcmp(blk->name,"Block_") != 0) blk = blk->sibing;
  depth--;
  translateblock(blk,idtype);
  varregtb->infun = 0;
}

void translateblock(ast* node,stype *idtype){ //Block
   depth++;
   table->auxstack->stackdepth++;
   ast* smt = node->child->child;
   while(smt){
     transstm(smt,idtype); 
     translatesmt(smt,idtype);
     if(smt->sibing) smt = smt->sibing->child;
     else break;
   }
   depth--;
   delstack();
}

void translatesmt(ast* node,stype* idtype){  //Stmt
    ast* p = node->child;
    if(!strcmp(p->name,"Expr_")){
      translateexp(p,NULL);
    }else if(!strcmp(p->name,"Vardef_")){
      translatevardefs(p);
    }else if(!strcmp(p->name,"Ret_")){
       if(!strcmp(p->child->sibing->name,"semicn")){
        genircode(IR_RET,NULL,0);   
       }else{
        operand* t1 = newtemp();
        translateexp(p->child->sibing,t1);
        if(strcmp(varregtb->funcname,"main") == 0)
          genircode(IR_RET,t1,1);  
        else
          genircode(IR_RET,t1,0);  
       }   
    }else if(!strcmp(p->name,"lbrace")){
      translateblock(p->sibing,idtype);
    }else if(!strcmp(p->name,"if")){
        ast* exp = p->sibing->sibing;
        ast* smt = p->sibing->sibing->sibing->sibing;   
        operand* label1 = newlabel();
        operand* label2 = newlabel();
        translatecond(exp, label1, label2);
        genircode(IR_LABEL,label1);
        translatesmt(smt,idtype);
        if (smt->sibing == NULL) {
            genircode(IR_LABEL, label2);
        }
        else {
            operand* label3 = newlabel();
            genircode(IR_GOTO, label3);
            genircode(IR_LABEL, label2);
            translatesmt(smt->sibing->sibing,idtype);
            genircode(IR_LABEL, label3);
        }
    }else if(!strcmp(p->name,"while")){ 
        operand*  label1 = newlabel();
        operand*  label2 = newlabel();
        operand*  label3 = newlabel();
        genircode(IR_LABEL, label1);
        translatecond(p->sibing->sibing, label2, label3);
        genircode(IR_LABEL, label2);
        translatesmt(p->sibing->sibing->sibing->sibing,idtype);
        genircode(IR_GOTO, label1);
        genircode(IR_LABEL, label3);
    }
}

void translatevardefs(ast* node){ //Vardef
   ast* var = node->child->sibing->child;
   translatevardef(var);
   while(var && var->sibing){
     var = var->sibing->sibing->child;
     translatevardef(var);
   }
}

void translatevardef(ast* node){  //IDVar
   ast* id = node->child;
   if(id->sibing){
     if(!strcmp(id->name,"id")){
       operand* t1;
       Symbol* pre = find_presymbol(id->val);
       if(pre)  t1 = newoperand(OP_VAR,strcat(id->val,(const char*)"1a"));
       else t1 = newoperand(OP_VAR,id->val);
      operand* t2 = newtemp();
      translateexp(id->sibing->sibing,t2);
      genircode(IR_ASSIGN, t1, t2);
     }else{   //array
      ast* arr = id->child;
      int idx = 0,size = 1;
      int dim0 = atoi(arr->sibing->sibing->child->val);
      int dim1 = 0;
      if(arr->sibing->sibing->sibing->sibing) 
        dim1 = atoi(arr->sibing->sibing->sibing->sibing->sibing->child->val);
      if(dim1) size = 4 * dim0 * dim1;
      else size =  4 * dim0;
      Symbol* syb = usesymbol(id->child->val);
      Symbol* pre = find_presymbol(id->child->val);
      operand* bvar;
      if(pre && (pre != syb || 
         (pre == syb && depth > syb->depth && pre->nexthash && !strcmp(pre->nexthash->field->name,arr->val)))) 
         bvar = newoperand(OP_VAR,strcat(arr->val,(const char*)"2a")); 
      else  
         bvar = newoperand(OP_VAR,arr->val);
      genircode(IR_DEC,bvar,size);
      if(!strcmp(id->sibing->sibing->child->sibing->name,"rbrace")) return;
      if(dim1 == 0){
         ast* ele = id->sibing->sibing->child->sibing->child;
         operand* t1;
         if(!strcmp(ele->child->child->name,"intnum"))
            t1 = newoperand(OP_CONST,atoi(ele->child->child->val));
         else{
            t1 = newtemp();
            translateexp(ele->child,t1);
         }
         operand* t2 = newtemp();
         operand* ofs = newoperand(OP_CONST,4 * idx);
         genircode(IR_ADD,t2,bvar,ofs);
         operand* t3 = newtemp();
         genircode(IR_ASSIGN,t3,t1);
         genircode(IR_MEM_WRITE,t2,t3);
         idx++;
         while(ele->sibing){
            ele = ele->sibing->sibing->child;
            if(!strcmp(ele->child->child->name,"intnum"))
               t1 = newoperand(OP_CONST,atoi(ele->child->child->val));
            else{
               t1 = newtemp();
               translateexp(ele->child,t1);
            }
            t2 = newtemp();
            ofs = newoperand(OP_CONST,4 * idx);
            genircode(IR_ADD,t2,bvar,ofs);
            t3 = newtemp();
            genircode(IR_ASSIGN,t3,t1);
            genircode(IR_MEM_WRITE,t2,t3);
            idx++;
         }
      }else if(dim1 != 0){
         ast* ele = id->sibing->sibing->child->sibing->child;
         operand* t1;
         operand* t2;
         operand* t3;
         operand* ofs;
         if(!strcmp(ele->child->name,"Expr_")){
            if(!strcmp(ele->child->child->name,"intnum"))
               t1 = newoperand(OP_CONST,atoi(ele->child->child->val));
            else{
               t1 = newtemp();
               translateexp(ele->child,t1);
            }
            t2 = newtemp();
            ofs = newoperand(OP_CONST,4 * idx);
            genircode(IR_ADD,t2,bvar,ofs);
            t3 = newtemp();
            genircode(IR_ASSIGN,t3,t1);
            genircode(IR_MEM_WRITE,t2,t3);
            idx++;
         }else{
            int rem = idx % dim1;
            idx += rem;
            ast* subgroup = ele->child->child->sibing->child;
            if(!strcmp(subgroup->child->child->name,"intnum"))
               t1 = newoperand(OP_CONST,atoi(subgroup->child->child->val));
            else{
               t1 = newtemp();
               translateexp(subgroup->child,t1);
            }
            t2 = newtemp();
            ofs = newoperand(OP_CONST,4 * idx);
            genircode(IR_ADD,t2,bvar,ofs);
            t3 = newtemp();
            genircode(IR_ASSIGN,t3,t1);
            genircode(IR_MEM_WRITE,t2,t3);
            int subidx = 1;
            idx++; 
            while(subgroup->sibing){
               subgroup = subgroup->sibing->sibing->child;
               if(!strcmp(subgroup->child->child->name,"intnum"))
                t1 = newoperand(OP_CONST,atoi(subgroup->child->child->val));
               else{
                  t1 = newtemp();
                  translateexp(subgroup->child,t1);
               }
               t2 = newtemp();
               ofs = newoperand(OP_CONST,4 * idx);
               genircode(IR_ADD,t2,bvar,ofs);
               t3 = newtemp();
               genircode(IR_ASSIGN,t3,t1);
               genircode(IR_MEM_WRITE,t2,t3);
               idx++;
               subidx++;
            }
            for(;subidx<dim1;subidx++,idx++){
               t2 = newtemp();
               ofs = newoperand(OP_CONST,4 * idx);
               genircode(IR_ADD,t2,bvar,ofs);
               t3 = newtemp();
               t1 = newoperand(OP_CONST,0);
               genircode(IR_ASSIGN,t3,t1);
               genircode(IR_MEM_WRITE,t2,t3);  
             }
            }
         while(ele->sibing){
            ele = ele->sibing->sibing->child;
            if(!strcmp(ele->child->name,"Expr_")){
               if(!strcmp(ele->child->child->name,"intnum"))
                  t1 = newoperand(OP_CONST,atoi(ele->child->child->val));
               else{
                  t1 = newtemp();
                  translateexp(ele->child,t1);
               }
               t2 = newtemp();
               ofs = newoperand(OP_CONST,4 * idx);
               genircode(IR_ADD,t2,bvar,ofs);
               t3 = newtemp();
               genircode(IR_ASSIGN,t3,t1);
               genircode(IR_MEM_WRITE,t2,t3);
               idx++;
            }else{
               int rem = idx % dim1;
               idx += rem;
               ast* subgroup = ele->child->child->sibing->child;
               if(!strcmp(subgroup->child->child->name,"intnum"))
                  t1 = newoperand(OP_CONST,atoi(subgroup->child->child->val));
               else{
                  t1 = newtemp();
                  translateexp(subgroup->child,t1);
               }
               t2 = newtemp();
               ofs = newoperand(OP_CONST,4 * idx);
               genircode(IR_ADD,t2,bvar,ofs);
               t3 = newtemp();
               genircode(IR_ASSIGN,t3,t1);
               genircode(IR_MEM_WRITE,t2,t3);
               int subidx = 1;
               idx++;
               while(subgroup->sibing){
                  subgroup = subgroup->sibing->sibing->child;
                  if(!strcmp(subgroup->child->child->name,"intnum"))
                     t1 = newoperand(OP_CONST,atoi(subgroup->child->child->val));
                  else{
                     t1 = newtemp();
                     translateexp(subgroup->child,t1);
                  }
                  t2 = newtemp();
                  ofs = newoperand(OP_CONST,4 * idx);
                  genircode(IR_ADD,t2,bvar,ofs);
                  t3 = newtemp();
                  genircode(IR_ASSIGN,t3,t1);
                  genircode(IR_MEM_WRITE,t2,t3);
                  idx++;
                  subidx++;
               }
                  for(;subidx<dim1;subidx++,idx++){
                     t2 = newtemp();
                     ofs = newoperand(OP_CONST,4 * idx);
                     genircode(IR_ADD,t2,bvar,ofs);
                     t3 = newtemp();
                     t1 = newoperand(OP_CONST,0);
                     genircode(IR_ASSIGN,t3,t1);
                     genircode(IR_MEM_WRITE,t2,t3);  
                  }  
               }
         }
      }
     }
   }else{
     if(strcmp(id->name,"id") != 0){
      id = id->child;
      int size = 1;
      int dim0 = atoi(id->sibing->sibing->child->val);
      int dim1 = 0;
      if(id->sibing->sibing->sibing->sibing)
        dim1 = atoi(id->sibing->sibing->sibing->sibing->sibing->child->val);
      if(dim1) size = 4 * dim0 * dim1;
      else size =  4 * dim0;
      operand* t1 = newoperand(OP_VAR,id->val);
      genircode(IR_DEC,t1,size);
     } 
   }
}
 
void translateexp(ast* node,operand* op){
   if(!strcmp(node->child->name,"intnum")){
      operand* t1 = newtemp();
      operand* c = newoperand(OP_CONST,atoi(node->child->val));
      genircode(IR_ASSIGN,t1,c);
      op->kind = t1->kind;
      op->u = t1->u;
   }else if(!strcmp(node->child->name,"lparent")){
       translateexp(node->child->sibing,op);
   }else if(!strcmp(node->child->name,"id")){
      if(!node->child->sibing){    //var
       operand* id;
       Symbol* syb = usesymbol(node->child->val);
       Symbol* pre = find_presymbol(node->child->val);
       if(pre && (pre != syb || 
         (pre == syb && depth > syb->depth && pre->nexthash && !strcmp(pre->nexthash->field->name,node->child->val)))) 
         id = newoperand(OP_VAR,strcat(node->child->val,(const char*)"1a")); 
       else if(pre && syb->depth == 0){
          id = newtemp();
          operand* t1 = newtemp();
          genircode(IR_ADDR_GET,t1,newoperand(OP_VAR,node->child->val));      
          if(syb->field->type->kind == BASIC) 
             genircode(IR_MEM_READ,id,t1);
          else
             id = t1;   
       }
       else id = newoperand(OP_VAR,node->child->val);
        op->kind = id->kind;
        op->u = id->u;
      }else if(!strcmp(node->child->sibing->name,"lparent")){  //func
        Symbol* syb = lookup(node->child->val);
        operand* id = newoperand(OP_FUNC,node->child->val);
        if(syb->field->type->u.function.args == 0){
         if(syb->field->type->u.function.ret->u.basic == INT_TYPE){ 
            op->kind = id->kind;
            op->u.name = id->u.name;
         }else{
            genircode(IR_CALL,id);
         }
        }else{
         ast* fexp = node->child->sibing->sibing->child;  //Funcparac : Expr    
         while(fexp){
            
            operand* t1 = newtemp();
            translateexp(fexp,t1);
            if(t1->kind == OP_FUNC && fexp->child->sibing){
               operand* t2 = t1;
               t1 = newtemp();
               genircode(IR_ASSIGN,t1,t2); 
            }
            genircode(IR_ARG,t1);
            if(fexp->sibing) fexp = fexp->sibing->sibing->child;
            else break;
         }
         if(syb->field->type->u.function.ret->u.basic == VOID_TYPE){
           genircode(IR_CALL,id);
         }
         else if(syb->field->type->u.function.ret->u.basic == INT_TYPE){ 
            operand* tmp = newtemp();
            genircode(IR_ASSIGN,tmp,id);
            if(op){
            op->kind = tmp->kind;
            op->u = tmp->u;
            }
         }
        }
      }else{  //array
         ast* arr = node->child->sibing->child;
         operand* bvar;
         Symbol* syb = usesymbol(node->child->val);
         Symbol* pre = find_presymbol(node->child->val);
         if((pre && (pre != syb || 
         (pre == syb && depth > syb->depth && syb->nexthash && !strcmp(syb->nexthash->field->name,node->child->val)))) 
          || (syb && !strcmp(varregtb->funcname,"print_array") && (!strcmp(syb->field->name,"a") && syb->field->type->kind == ARRAY))) 
            bvar = newoperand(OP_VAR,strcat(node->child->val,(const char*)"2a")); 
         else if(pre && syb->depth == 0){
            bvar = newtemp();
            genircode(IR_ADDR_GET,bvar,newoperand(OP_VAR,node->child->val));       
         }
         else bvar = newoperand(OP_VAR,node->child->val);
         operand* t3 = newtemp();
         operand* t4;
         translateexp(arr->sibing,t3);
         operand* t5 = newtemp();
         operand* four = newoperand(OP_CONST,4);
         operand* t6 = newtemp();
         genircode(IR_ASSIGN,t6,four);
         if(arr->sibing->sibing->sibing){
            operand* dim1 = newoperand(OP_CONST,syb->field->type->u.array.size[1]);
            operand* t7 = newtemp();
            genircode(IR_ASSIGN,t7,dim1);
            operand* t8 = newtemp();
            genircode(IR_MUL,t8,t3,t7);
            t4 = newtemp();
            translateexp(arr->sibing->sibing->sibing->sibing,t4);
            operand* t9 = newtemp();
            genircode(IR_ADD,t9,t8,t4);
            genircode(IR_MUL,t5,t6,t9);
            operand* t2 = newtemp();
            genircode(IR_ADD,t2,bvar,t5);
            genircode(IR_MEM_READ,op,t2);       
         }else if(syb->field->type->u.array.size[1] && 
            !arr->sibing->sibing->sibing){
           operand* dim1 = newoperand(OP_CONST,syb->field->type->u.array.size[1]);
           operand* t7 = newtemp();
           genircode(IR_ASSIGN,t7,dim1);
           operand* t8 = newtemp();
           genircode(IR_MUL,t8,t3,t7);    
           genircode(IR_MUL,t5,t6,t8); 
           genircode(IR_ADD,op,bvar,t5);
         }
         else {
            genircode(IR_MUL,t5,t6,t3);
            operand* t2 = newtemp();
            genircode(IR_ADD,t2,bvar,t5);
            genircode(IR_MEM_READ,op,t2);   
         }          
      }
   }else if(!strcmp(node->child->name,"sub")){
      operand* t2 = newtemp();
      operand* t1 = newtemp();
      operand* zero = newoperand(OP_CONST,0);
      genircode(IR_ASSIGN,t2,zero);
      translateexp(node->child->sibing,t1);
      genircode(IR_SUB,op,t2,t1);
   }else if(!strcmp(node->child->name,"add")){
      translateexp(node->child->sibing,op);
   }else if(!strcmp(node->child->name,"not") || !strcmp(node->child->sibing->name,"and") || 
            !strcmp(node->child->sibing->name,"or") || !strcmp(node->child->sibing->name,"eq") || 
            !strcmp(node->child->sibing->name,"neq") || !strcmp(node->child->sibing->name,"le") ||
            !strcmp(node->child->sibing->name,"ge") || !strcmp(node->child->sibing->name,"lt") ||
            !strcmp(node->child->sibing->name,"gt")){
       operand* label1 = newlabel();
       operand* label2 = newlabel();
       operand* true_num = newoperand(OP_CONST, 1);
       operand* false_num = newoperand(OP_CONST, 0);
       genircode(IR_ASSIGN, op, false_num);
       translatecond(node, label1, label2);
       genircode(IR_LABEL, label1);
       genircode(IR_ASSIGN, op, true_num);
       genircode(IR_LABEL,label2);
   }else if(!strcmp(node->child->sibing->name,"assign")){
      if(!node->child->child->sibing){  //var
       operand* id;
       operand* t2 = newtemp();
       translateexp(node->child->sibing->sibing, t2);
       Symbol* syb = usesymbol(node->child->child->val);
       Symbol* pre = find_presymbol(node->child->child->val);
       if(pre && (pre != syb || 
         (pre == syb && depth > syb->depth && pre->nexthash && !strcmp(pre->nexthash->field->name,node->child->child->val)))) { 
         id = newoperand(OP_VAR,strcat(node->child->child->val,(const char*)"1a")); 
         genircode(IR_ASSIGN, id, t2);
        }
       else if(pre && syb->depth == 0){
          id = newtemp();
          genircode(IR_ADDR_GET,id,newoperand(OP_VAR,node->child->child->val));       
          genircode(IR_MEM_WRITE,id,t2);
       }
       else{ 
         id = newoperand(OP_VAR,node->child->child->val);
         genircode(IR_ASSIGN, id, t2);
       }     
      }else{  //array
         Symbol* syb = usesymbol(node->child->child->val);
         Symbol* pre = find_presymbol(node->child->child->val);
         operand* t3 = newtemp();
         operand* t4;
         ast* arr = node->child->child->sibing->child;
         translateexp(arr->sibing,t3);
         operand* four = newoperand(OP_CONST,4);
         operand* t5 = newtemp();
         operand* t6 = newtemp();
         genircode(IR_ASSIGN,t6,four);
         if(arr->sibing->sibing->sibing){
            t4 = newtemp();
            operand* dim1 = newoperand(OP_CONST,syb->field->type->u.array.size[1]);
            operand* t7 = newtemp();
            genircode(IR_ASSIGN,t7,dim1);
            operand* t8 = newtemp();
            genircode(IR_MUL,t8,t3,t7);
            translateexp(arr->sibing->sibing->sibing->sibing,t4);
            operand* t9 = newtemp();
            genircode(IR_ADD,t9,t8,t4);
            genircode(IR_MUL,t5,t6,t9);
         }
         else genircode(IR_MUL,t5,t6,t3);
       operand* id;
       operand* t2 = newtemp();
       translateexp(node->child->sibing->sibing, t2);
       if((pre && (pre != syb || 
         (pre == syb && depth > syb->depth && pre->nexthash && !strcmp(pre->nexthash->field->name,node->child->child->val))))
         || (syb && !strcmp(varregtb->funcname,"print_array") && (!strcmp(syb->field->name,"a") && syb->field->type->kind == ARRAY))) { 
         operand* t1 = newoperand(OP_VAR,strcat(node->child->child->val,(const char*)"2a")); 
         id = newtemp();
         genircode(IR_ADD,id,t1,t5);
         genircode(IR_MEM_WRITE, id, t2);
        }
       else if(pre && syb->depth == 0){
          id = newtemp();
          operand* t1 = newtemp();
          genircode(IR_ADDR_GET,t1,newoperand(OP_VAR,node->child->child->val));       
          genircode(IR_ADD,id,t1,t5);
          genircode(IR_MEM_WRITE,id,t2);
       }
       else{ 
         operand* t1 = newoperand(OP_VAR,node->child->child->val); 
         id = newtemp();
         genircode(IR_ADD,id,t1,t5);
         genircode(IR_MEM_WRITE, id, t2);
       } 
      }
   }else{
         operand* t1 = newtemp();
         translateexp(node->child, t1);
         operand* t2 = newtemp();
         translateexp(node->child->sibing->sibing, t2);
         if (!strcmp(node->child->sibing->name, "add")) 
            genircode(IR_ADD, op, t1, t2);
         else if (!strcmp(node->child->sibing->name, "sub")) 
            genircode(IR_SUB, op, t1, t2);
         else if (!strcmp(node->child->sibing->name, "mul")) 
            genircode(IR_MUL, op, t1, t2);
         else if (!strcmp(node->child->sibing->name, "div")) 
            genircode(IR_DIV, op, t1, t2);
         else if(!strcmp(node->child->sibing->name, "rem"))
            genircode(IR_REM, op, t1, t2);
         else{
            operand* label1 = newlabel();
            operand* label2 = newlabel();
            operand* true_num = newoperand(OP_CONST, 1);
            operand* false_num = newoperand(OP_CONST, 0);
            genircode(IR_ASSIGN, op, false_num);
            translatecond(node, label1, label2);
            genircode(IR_LABEL, label1);
            genircode(IR_ASSIGN, op, true_num); 
            genircode(IR_LABEL, label2);
         }
   } 
}

void translatecond(ast* node,operand* ltrue,operand* lfalse){
   if(!strcmp(node->child->name,"not")){
      translatecond(node->child->sibing,lfalse,ltrue);
   }else if(!node->child->sibing){
      operand* t1 = newtemp();
      translateexp(node, t1);
      operand* t2 = newoperand(OP_CONST, 0);
      operand* relop = newoperand(OP_RELOP,"!=");
      operand* t3 = newtemp();
      genircode(IR_ASSIGN,t3,t2);
      genircode(IR_IF, t1, relop, t3, ltrue);
      genircode(IR_GOTO, lfalse);
   }
   else if(!strcmp(node->child->sibing->name,"and")){
      operand* label1 = newlabel();
      translatecond(node->child, label1, lfalse);
      genircode(IR_LABEL, label1);
      translatecond(node->child->sibing->sibing, ltrue, lfalse);
   }else if(!strcmp(node->child->sibing->name,"or")){
      operand* label1 = newlabel();
      translatecond(node->child, ltrue, label1);
      genircode(IR_LABEL, label1);
      translatecond(node->child->sibing->sibing, ltrue, lfalse);
   }else if(node->child->sibing && (!strcmp(node->child->sibing->name,"eq") ||
     !strcmp(node->child->sibing->name,"neq") ||!strcmp(node->child->sibing->name,"le") ||
     !strcmp(node->child->sibing->name,"ge") || !strcmp(node->child->sibing->name,"lt") ||
     !strcmp(node->child->sibing->name,"gt"))){
        operand* t1 = newtemp();
        operand* t2 = newtemp();
        translateexp(node->child, t1);
        translateexp(node->child->sibing->sibing, t2);
        operand* relop = newoperand(OP_RELOP, node->child->sibing->val);
        genircode(IR_IF, t1, relop, t2, ltrue);
        genircode(IR_GOTO, lfalse);
   }else{
        operand* t1 = newtemp();
        translateexp(node, t1);
        if(t1->kind == OP_FUNC){
         operand* t4 = newtemp();
         genircode(IR_ASSIGN,t4,t1);
         operand* t2 = newoperand(OP_CONST, 0);
         operand* relop = newoperand(OP_RELOP,"!=");
         operand* t3 = newtemp();
         genircode(IR_ASSIGN,t3,t2);
         genircode(IR_IF, t4, relop, t3, ltrue);
         genircode(IR_GOTO, lfalse);     
        }else{
        operand* t2 = newoperand(OP_CONST, 0);
        operand* relop = newoperand(OP_RELOP,"!=");
        operand* t3 = newtemp();
        genircode(IR_ASSIGN,t3,t2);
        genircode(IR_IF, t1, relop, t3, ltrue);
        genircode(IR_GOTO, lfalse);
        }
   }
}

void printircodelist(FILE *fp){
   for(ircodes* codes = ircodelist->head;codes;codes = codes->next){
      switch (codes->code->kind)
      {
      case IR_LABEL:
         fprintf(fp,"  ");
         fprintf(fp,"LABEL ");
         printop(fp,codes->code->u.unop.op);
         fprintf(fp,":");
         break;
      case IR_FUNC:
         fprintf(fp,"FUNCTION ");
         printop(fp,codes->code->u.unop.op);
         fprintf(fp,":");
         break;
      case IR_GOTO:
         fprintf(fp,"\t");
         fprintf(fp,"GOTO ");
         printop(fp,codes->code->u.unop.op);
         break;
      case IR_RET:
         fprintf(fp,"\t");
         fprintf(fp,"RETURN ");
         printop(fp,codes->code->u.dec.op);
         break;
      case IR_PARAM:
         fprintf(fp,"\t");
         fprintf(fp,"PARAM ");
         printop(fp,codes->code->u.unop.op);
         break;
      case IR_ARG:
         fprintf(fp,"\t");
         fprintf(fp,"ARG ");
         printop(fp,codes->code->u.unop.op);
         break;
      case IR_CALL:
         fprintf(fp,"\t");     
         fprintf(fp,"CALL ");
         printop(fp,codes->code->u.unop.op);
         break;
      case IR_ASSIGN:
         fprintf(fp,"\t");
         printop(fp,codes->code->u.assign.left);
         fprintf(fp," = ");
         if(codes->code->u.assign.right->kind == OP_FUNC)
          fprintf(fp,"CALL ");
         printop(fp,codes->code->u.assign.right);
         break;
      case IR_MEM_READ:
         fprintf(fp,"\t");
         printop(fp,codes->code->u.assign.left);
         fprintf(fp," = ");   
         fprintf(fp,"*");
         printop(fp,codes->code->u.assign.right);
         break;
      case IR_MEM_WRITE: 
         fprintf(fp,"\t");
         fprintf(fp,"*");
         printop(fp,codes->code->u.assign.left);
         fprintf(fp," = ");   
         printop(fp,codes->code->u.assign.right);   
         break;
      case IR_ADDR_GET:
         fprintf(fp,"\t");
         printop(fp,codes->code->u.assign.left);
         fprintf(fp," = ");
         fprintf(fp,"&");
         printop(fp,codes->code->u.assign.right);
         break;
      case IR_ADD:
         fprintf(fp,"\t");
         if(!codes->code->u.binop.result) continue;
         printop(fp, codes->code->u.binop.result);
         fprintf(fp, " = ");
         printop(fp, codes->code->u.binop.op1);
         fprintf(fp, " + ");
         printop(fp, codes->code->u.binop.op2);
         break;
      case IR_SUB:
         fprintf(fp,"\t");
         printop(fp, codes->code->u.binop.result);
         fprintf(fp, " = ");
         printop(fp, codes->code->u.binop.op1);
         fprintf(fp, " - ");
         printop(fp, codes->code->u.binop.op2);
         break;
      case IR_MUL:
         fprintf(fp,"\t");
         printop(fp, codes->code->u.binop.result);
         fprintf(fp, " = ");
         printop(fp, codes->code->u.binop.op1);
         fprintf(fp, " * ");
         printop(fp, codes->code->u.binop.op2);
         break;
      case IR_DIV:
         fprintf(fp,"\t");
         printop(fp, codes->code->u.binop.result);
         fprintf(fp, " = ");
         printop(fp, codes->code->u.binop.op1);
         fprintf(fp, " / ");
         printop(fp, codes->code->u.binop.op2);
         break;
      case IR_REM:
         fprintf(fp,"\t");
         printop(fp, codes->code->u.binop.result);
         fprintf(fp, " = ");
         printop(fp, codes->code->u.binop.op1);
         fprintf(fp, " %% ");
         printop(fp, codes->code->u.binop.op2);
         break;
      case IR_IF:
         fprintf(fp, "\tIF ");
         printop(fp, codes->code->u.ifsmt.x);
         fprintf(fp, " ");
         printop(fp, codes->code->u.ifsmt.relop);
         fprintf(fp, " ");
         printop(fp, codes->code->u.ifsmt.y);
         fprintf(fp, " GOTO ");
         printop(fp, codes->code->u.ifsmt.z);
         break;
      case IR_DEC:
         fprintf(fp,"\tDEC ");
         printop(fp,codes->code->u.dec.op);
         fprintf(fp," #");
         fprintf(fp,"%d",codes->code->u.dec.size);
         break;
      case IR_GLOBAL:
         fprintf(fp,"GLOBAL ");
         printop(fp,codes->code->u.unop.op);
         fprintf(fp,":");
         break;
      case IR_WORD:
         fprintf(fp,"\t.WORD ");
         fprintf(fp,"#%d",codes->code->u.unop.op->u.val);
         break;
      default:
         break;
      }
    fprintf(fp,"\n");
   }
}
