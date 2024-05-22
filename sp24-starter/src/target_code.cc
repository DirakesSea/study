#include "target_code.hh"

const char* reg_names[32] = {  "zero","ra","sp","gp","tp","t0","t1","t2","fp","s1","a0","a1","a2",
     "a3","a4","a5","a6","a7","s2","s3","s4","s5","s6","s7","s8","s9","s10","s11","t3","t4","t5","t6"
};

// sort_array    sudoku  two_dimension_array  array_hard   merge_sort
// value error           time out             value error  timeout

reg_heap* regheap;
var_reg_tb* varregtb;

void regsenvinit()
{
    regheap = (reg_heap*)malloc(sizeof(reg_heap));
    for(int i=0;i<32;i++){
      regheap->regs[i] = (reg*)malloc(sizeof(reg));
      regheap->regs[i]->free = 1;
      regheap->regs[i]->regname = strdup(reg_names[i]);
    }
    regheap->regs[0]->free = 0;
    regheap->lastinfo = 0;
    varregtb = (var_reg_tb*)malloc(sizeof(var_reg_tb));
    varregtb->infun = 0;
    varregtb->funcname = NULL;
    varregtb->inreg = (var_reg_list*)malloc(sizeof(var_reg_list));
    varregtb->inmem = (var_reg_list*)malloc(sizeof(var_reg_list));
    varregtb->inreg->head = NULL;
    varregtb->inreg->tail = NULL;
    varregtb->inmem->head = NULL;
    varregtb->inmem->tail = NULL;
    varregtb->arrlist = NULL; 
    varregtb->funcdep = 0;
}

void addvarreg(var_reg_list *var_list, int reg_no, operand *op){
   var_reg* varreg = (var_reg*)malloc(sizeof(var_reg));
   varreg->reg_no = reg_no;
   varreg->op = op;
   varreg->next = NULL;
   if(var_list->head == NULL){
     var_list->head = varreg;
     var_list->tail = varreg;
   }else{
     var_list->tail->next = varreg;
     var_list->tail = varreg;
   }
}

void delvarreg(var_reg_list *var_list, var_reg *varreg){
   if(varreg == var_list->head){
     var_reg* k = var_list->head;
     var_list->head = var_list->head->next;
     free(k);
      if(var_list->head == NULL)
        var_list->tail = NULL;
   }else{
     var_reg* t = var_list->head;
     for(;t;t=t->next)
      if(t->next == varreg) break;
     if(var_list->tail == varreg) var_list->tail = t;
      t->next = varreg->next;
      varreg->next = NULL;
     free(varreg);
   }
}

int usevarreg(FILE *fp, operand *op){
    if(op->kind != OP_CONST){
      var_reg* tmp = varregtb->inreg->head;
      while(tmp){
        if(tmp->op->kind != OP_CONST && !strcmp(tmp->op->u.name,op->u.name)){
           return tmp->reg_no;
        }
        tmp = tmp->next;
      }
      return regalloc(op);
    }else{
       if(op->u.val == 0) return ZERO;
       int regno = regalloc(op);
        fprintf(fp, "\tli %s, %d\n", regheap->regs[regno]->regname,
                op->u.val);
        return regno;
    }
}

int regalloc(operand *op)
{
    for(int i = T0;i <= T2;i++){
      if(regheap->regs[i]->free){
        regheap->regs[i]->free = 0;
        addvarreg(varregtb->inreg,i,op);
        return i;
      }
    }
    for(int i = T3;i <= T6;i++){
      if(regheap->regs[i]->free){
        regheap->regs[i]->free = 0;
        addvarreg(varregtb->inreg,i,op);
        return i;
      }
    }
    for(int i = S2;i <= S11;i++){
      if(regheap->regs[i]->free){
        regheap->regs[i]->free = 0;
        addvarreg(varregtb->inreg,i,op);
        return i;
      }
    }
    var_reg* tmp = varregtb->inreg->head;
    for(;tmp;tmp = tmp->next){
      if(tmp->op->kind == OP_CONST && tmp->reg_no != regheap->lastinfo){
         int reg_no = tmp->reg_no;
         regheap->lastinfo = reg_no;
         delvarreg(varregtb->inreg,tmp);
         addvarreg(varregtb->inreg,reg_no,op);
         return reg_no;
      }else  if(tmp->op->kind != OP_CONST && tmp->op->u.name[0] == 't' && tmp->reg_no != regheap->lastinfo){
         int reg_no = tmp->reg_no;
         regheap->lastinfo = reg_no;
         delvarreg(varregtb->inreg,tmp);
         addvarreg(varregtb->inreg,reg_no,op);
         return reg_no;
      }
    }
    tmp = varregtb->inreg->head;
    for(;tmp;tmp = tmp->next){
      if(tmp->reg_no != regheap->lastinfo){
         int reg_no = tmp->reg_no;
         regheap->lastinfo = reg_no;
         delvarreg(varregtb->inreg,tmp);
         addvarreg(varregtb->inreg,reg_no,op);
         return reg_no;
      }
    }
    printf("alloc error\n");
    return T6;
}

ircodes* printheadcode(FILE *fp){
  ircodes* codes = ircodelist->head;
  fprintf(fp,".data\n");
  while(codes->code->kind != IR_FUNC){
    if(codes->code->kind == IR_GLOBAL){
     printop(fp,codes->code->u.unop.op);
     fprintf(fp,":");
   }else if(codes->code->kind == IR_WORD){
     fprintf(fp,"\t.word ");
     fprintf(fp,"%d",codes->code->u.unop.op->u.val);
   }else break;
   fprintf(fp,"\n");
   codes = codes->next;
  }
  ircodes* tmp = codes->next;
  while (tmp)
  {
    if(tmp->code->kind == IR_GLOBAL){
     printop(fp,tmp->code->u.unop.op);
     fprintf(fp,":\n");
   }else if(tmp->code->kind == IR_WORD){
     fprintf(fp,"\t.word ");
     fprintf(fp,"%d\n",tmp->code->u.unop.op->u.val);
   }
   else if(tmp->code->kind == IR_DEC){
     fprintf(fp,"%s:\n",tmp->code->u.dec.op->u.name);
     fprintf(fp,"\t.space %d\n",tmp->code->u.dec.size);
     arr_list* arrmem = (arr_list*)malloc(sizeof(arr_list));
     arrmem->arrname = strdup(tmp->code->u.dec.op->u.name);
     arrmem->next = NULL;
     if(varregtb->arrlist == NULL)
       varregtb->arrlist = arrmem;
     else{
       arr_list* temp = varregtb->arrlist;
       for(;temp->next;temp = temp->next) ;
       temp->next = arrmem;
    }
   }
    tmp = tmp->next;
  }
 return codes;
}

int find_arrname(char *name)
{  
    for(arr_list* tmp = varregtb->arrlist;tmp;tmp = tmp->next)
     if(!strcmp(tmp->arrname,name))
       return 1; 
    return 0;
}

void printtargetlist(FILE *fp){
   ircodes* codes = printheadcode(fp);
   fprintf(fp,".text\n");
   fprintf(fp,"\tj main");
   for(;codes;codes = codes->next){
      printtargetcode(fp,codes);
   }
}

void printtargetcode(FILE *fp, ircodes *codes){
   irkind kind = codes->code->kind;
   int regno;
   if(kind == IR_LABEL){
     printop(fp,codes->code->u.unop.op);
     fprintf(fp,":");
   }else if(kind == IR_FUNC){
     fprintf(fp,"\n");
     printop(fp,codes->code->u.unop.op);
     fprintf(fp,":\n");
     Symbol* syb = lookup(codes->code->u.unop.op->u.name);
     if(!strcmp(codes->code->u.unop.op->u.name,"main"))
       varregtb->infun = 0;
     else 
       varregtb->infun = 1;
     ircodes* tmp = codes->next;
     int argc = 0;
     while (tmp && tmp->code->kind == IR_PARAM)
     {
       if(argc < 8){
         addvarreg(varregtb->inreg,A0 + argc,tmp->code->u.unop.op);
       }else{
         regno = usevarreg(fp,tmp->code->u.unop.op);
         fprintf(fp,"\tlw %s, %d(sp)\n",regheap->regs[regno]->regname,
          4 * (syb->field->type->u.function.args - argc));
       }
       argc++;
       tmp = tmp->next;
     }
   }else if(kind == IR_GOTO){
     fprintf(fp,"\tj ");
     printop(fp,codes->code->u.unop.op);
     fprintf(fp,"\n");
   }else if(kind == IR_RET){
     if(codes->code->u.dec.size == 1){  //main
       fprintf(fp,"\tli a0, 10\n");
       fprintf(fp,"\tli a1, 0\n");
       fprintf(fp,"\tecall\n");
     }else{
       if(codes->code->u.dec.op != NULL){
         regno = usevarreg(fp,codes->code->u.dec.op);
         fprintf(fp,"\tmv a0, %s\n",regheap->regs[regno]->regname);
        }
       varregtb->funcdep--; 
       fprintf(fp,"\tret\n");
     }
   }else if(kind == IR_CALL){
    char* funcname = codes->code->u.unop.op->u.name;
    if(!strcmp(funcname,"write")){
      int preno = usevarreg(fp,codes->prev->code->u.unop.op);
      fprintf(fp,"\taddi sp, sp, -8\n");
      fprintf(fp,"\tsw a0, 0(sp)\n");
      fprintf(fp,"\tsw a1, 4(sp)\n");
      fprintf(fp,"\tli a0, 1\n");
      fprintf(fp,"\tmv a1, %s\n",regheap->regs[preno]->regname);
      fprintf(fp,"\tecall\n");
      fprintf(fp,"\tlw a0, 0(sp)\n");
      fprintf(fp,"\tlw a1, 4(sp)\n");
      fprintf(fp,"\taddi sp, sp, 8\n");
    }else{
      Symbol* syb = lookup(funcname);
      int argc = syb->field->type->u.function.args - 1;
      if(varregtb->infun){
          fprintf(fp,"\taddi sp, sp, -%d\n",(7 + 1 + 1) * 4);
          for(int i = 0;i <= 7;i++)
            fprintf(fp,"\tsw %s, %d(sp)\n",regheap->regs[A0+i]->regname,i * 4);
          fprintf(fp,"\tsw ra, 32(sp)\n"); 
        }
      pushtmp(fp);
      ircodes* tmp = codes->prev;
      while (tmp && argc >= 0)
      {
        if(tmp->code->kind == IR_ARG){
          int argno = usevarreg(fp,tmp->code->u.unop.op);
          if(argc < 8){
            fprintf(fp,"\tmv %s, %s\n",regheap->regs[A0+argc]->regname,
              regheap->regs[argno]->regname);
          }else{
            fprintf(fp, "\taddi sp, sp, -4\n");
            fprintf(fp, "\tsw %s, 0(sp)\n",regheap->regs[argno]->regname);
          }
          argc--;
        }
        tmp = tmp->prev;
      }
      varregtb->funcdep++;
      fprintf(fp,"\tcall %s\n",funcname);
      if(syb->field->type->u.function.args > 8)
        fprintf(fp, "\taddi sp, sp, %d\n",4 * (syb->field->type->u.function.args-8));
      poptmp(fp);
      argc = syb->field->type->u.function.args - 1;
        if(varregtb->infun){
          for(int i = 0;i <= 7;i++)
           fprintf(fp,"\tlw %s, %d(sp)\n",regheap->regs[A0+i]->regname,i * 4);
          fprintf(fp,"\tlw ra, 32(sp)\n"); 
          fprintf(fp,"\taddi sp, sp, %d\n",(7 + 1 + 1) * 4);
        }
    }
   }else if(kind == IR_ASSIGN){
    if(codes->code->u.assign.right->kind == OP_FUNC){
      if(!strcmp(codes->code->u.assign.right->u.name,"read")){
        fprintf(fp,"\taddi sp, sp, -8\n");
        fprintf(fp,"\tsw a0, 0(sp)\n");
        fprintf(fp,"\tsw a1, 4(sp)\n");
        fprintf(fp,"\tli a0, 6\n");
        fprintf(fp,"\tecall\n");
        regno = usevarreg(fp,codes->code->u.assign.left);
        fprintf(fp,"\tmv %s, a0\n",regheap->regs[regno]->regname);
        fprintf(fp,"\tlw a1, 4(sp)\n");
        fprintf(fp,"\taddi sp, sp, 8\n");
      }else{  
        char* funcname = codes->code->u.assign.right->u.name;
        ircodes* tmp = codes->prev;
        Symbol* syb = lookup(funcname);
        int argc = syb->field->type->u.function.args - 1;
        if(varregtb->infun){
          fprintf(fp,"\taddi sp, sp, -%d\n",(7 + 1 + 1) * 4);
          for(int i = 0;i <= 7;i++)
           fprintf(fp,"\tsw %s, %d(sp)\n",regheap->regs[A0+i]->regname,i * 4);
          fprintf(fp,"\tsw ra, 32(sp)\n"); 
        }
        pushtmp(fp);
        while (tmp && argc >= 0)
        {
          if(tmp->code->kind == IR_ARG){
            int argno = usevarreg(fp,tmp->code->u.unop.op);
            if(argc < 8){
              fprintf(fp,"\tmv %s, %s\n",regheap->regs[A0+argc]->regname,
                regheap->regs[argno]->regname);
            }else{
              fprintf(fp, "\taddi sp, sp, -4\n");
              fprintf(fp, "\tsw %s, 0(sp)\n",regheap->regs[argno]->regname);
            }
            argc--;
          }
          tmp = tmp->prev;
        }
        fprintf(fp,"\tcall %s\n",funcname);
        if(syb->field->type->u.function.args > 8)
         fprintf(fp, "\taddi sp, sp, %d\n",4 * (syb->field->type->u.function.args-8));
        poptmp(fp);
        regno = usevarreg(fp,codes->code->u.assign.left);
        fprintf(fp,"\tmv %s, a0\n",regheap->regs[regno]->regname);
        argc = syb->field->type->u.function.args - 1;
        if(varregtb->infun){
          for(int i = 0;i <= 7;i++)
           fprintf(fp,"\tlw %s, %d(sp)\n",regheap->regs[A0+i]->regname,i * 4);
          fprintf(fp,"\tlw ra, 32(sp)\n"); 
          fprintf(fp,"\taddi sp, sp, %d\n",(7 + 1 + 1) * 4);
        }
      }
    }else if(codes->code->u.assign.right->kind == OP_CONST){
       regno = usevarreg(fp,codes->code->u.assign.left);
       fprintf(fp,"\tli %s, %d\n",regheap->regs[regno]->regname,codes->code->u.assign.right->u.val);
    }else{
       regno = usevarreg(fp,codes->code->u.assign.left);
       int rregno = usevarreg(fp,codes->code->u.assign.right);
       fprintf(fp,"\tmv %s, %s\n",regheap->regs[regno]->regname,regheap->regs[rregno]->regname);
    }
   }else if(kind == IR_MEM_READ){
    int leftno = usevarreg(fp,codes->code->u.assign.left);
    int rightno = usevarreg(fp,codes->code->u.assign.right);
    fprintf(fp,"\tlw %s, 0(%s)\n",regheap->regs[leftno]->regname,regheap->regs[rightno]->regname);
   }else if(kind == IR_MEM_WRITE){
    int leftno = usevarreg(fp,codes->code->u.assign.left);
    int rightno = usevarreg(fp,codes->code->u.assign.right);
    fprintf(fp,"\tsw %s, 0(%s)\n",regheap->regs[rightno]->regname,regheap->regs[leftno]->regname);
   }else if(kind == IR_ADDR_GET){
    int leftno = usevarreg(fp,codes->code->u.assign.left);
    fprintf(fp,"\tla %s, %s\n",regheap->regs[leftno]->regname,codes->code->u.assign.right->u.name);
   }else if(kind == IR_ADD){
    if(!codes->code->u.binop.result) return;
    if(codes->code->u.binop.op2->kind == OP_CONST){
     regno = usevarreg(fp,codes->code->u.binop.result);
     int op1no = usevarreg(fp,codes->code->u.binop.op1);
     fprintf(fp,"\taddi %s, %s, %d\n",regheap->regs[regno]->regname,
        regheap->regs[op1no]->regname,codes->code->u.binop.op2->u.val);
    }else{
     regno = usevarreg(fp,codes->code->u.binop.result);
      if(find_arrname(codes->code->u.binop.op1->u.name) == 0){
        int op1no = usevarreg(fp,codes->code->u.binop.op1);
        int op2no = usevarreg(fp,codes->code->u.binop.op2);
        fprintf(fp,"\tadd %s, %s, %s\n",regheap->regs[regno]->regname,
            regheap->regs[op1no]->regname,regheap->regs[op2no]->regname);
      }else{
        int op1no = regalloc(codes->code->u.binop.op1);
        int op2no = usevarreg(fp,codes->code->u.binop.op2);
        fprintf(fp,"\tla %s, %s\n",regheap->regs[op1no]->regname,codes->code->u.binop.op1->u.name);
        fprintf(fp,"\tadd %s, %s, %s\n",regheap->regs[regno]->regname,
            regheap->regs[op1no]->regname,regheap->regs[op2no]->regname);
      }
    }
   }else if(kind == IR_SUB){
    if(codes->code->u.binop.op2->kind == OP_CONST){
     regno = usevarreg(fp,codes->code->u.binop.result);
     int op1no = usevarreg(fp,codes->code->u.binop.op1);
     fprintf(fp,"\tsubi %s, %s, %d\n",regheap->regs[regno]->regname,
        regheap->regs[op1no]->regname,codes->code->u.binop.op2->u.val);
    }else{
     regno = usevarreg(fp,codes->code->u.binop.result);
     int op1no = usevarreg(fp,codes->code->u.binop.op1);
     int op2no = usevarreg(fp,codes->code->u.binop.op2);
     fprintf(fp,"\tsub %s, %s, %s\n",regheap->regs[regno]->regname,
        regheap->regs[op1no]->regname,regheap->regs[op2no]->regname);
    }
   }else if(kind == IR_MUL){
    if(codes->code->u.binop.op2->kind == OP_CONST){
     regno = usevarreg(fp,codes->code->u.binop.result);
     int op1no = usevarreg(fp,codes->code->u.binop.op1);
     fprintf(fp,"\tmuli %s, %s, %d\n",regheap->regs[regno]->regname,
        regheap->regs[op1no]->regname,codes->code->u.binop.op2->u.val);
    }else{
     regno = usevarreg(fp,codes->code->u.binop.result);
     int op1no = usevarreg(fp,codes->code->u.binop.op1);
     int op2no = usevarreg(fp,codes->code->u.binop.op2);
     fprintf(fp,"\tmul %s, %s, %s\n",regheap->regs[regno]->regname,
        regheap->regs[op1no]->regname,regheap->regs[op2no]->regname);
    }
   }else if(kind == IR_DIV){
    if(codes->code->u.binop.op2->kind == OP_CONST){
     regno = usevarreg(fp,codes->code->u.binop.result);
     int op1no = usevarreg(fp,codes->code->u.binop.op1);
     fprintf(fp,"\tdivi %s, %s, %d\n",regheap->regs[regno]->regname,
        regheap->regs[op1no]->regname,codes->code->u.binop.op2->u.val);
    }else{
     regno = usevarreg(fp,codes->code->u.binop.result);
     int op1no = usevarreg(fp,codes->code->u.binop.op1);
     int op2no = usevarreg(fp,codes->code->u.binop.op2);
     fprintf(fp,"\tdiv %s, %s, %s\n",regheap->regs[regno]->regname,
        regheap->regs[op1no]->regname,regheap->regs[op2no]->regname);
    }
   }else if(kind == IR_REM){
    if(codes->code->u.binop.op2->kind == OP_CONST){
     regno = usevarreg(fp,codes->code->u.binop.result);
     int op1no = usevarreg(fp,codes->code->u.binop.op1);
     fprintf(fp,"\tremi %s, %s, %d\n",regheap->regs[regno]->regname,
        regheap->regs[op1no]->regname,codes->code->u.binop.op2->u.val);
    }else{
     regno = usevarreg(fp,codes->code->u.binop.result);
     int op1no = usevarreg(fp,codes->code->u.binop.op1);
     int op2no = usevarreg(fp,codes->code->u.binop.op2);
     fprintf(fp,"\trem %s, %s, %s\n",regheap->regs[regno]->regname,
        regheap->regs[op1no]->regname,regheap->regs[op2no]->regname);
    }
   }else if(kind == IR_IF){
    char* relop = codes->code->u.ifsmt.relop->u.name;
    int xregno = usevarreg(fp,codes->code->u.ifsmt.x);
    int yregno = usevarreg(fp,codes->code->u.ifsmt.y);
    if(!strcmp(relop,"==")){
       fprintf(fp,"\tbeq %s, %s, %s\n",regheap->regs[xregno]->regname,
        regheap->regs[yregno]->regname,codes->code->u.ifsmt.z->u.name);
    }else if(!strcmp(relop,"!=")){
       fprintf(fp,"\tbne %s, %s, %s\n",regheap->regs[xregno]->regname,
        regheap->regs[yregno]->regname,codes->code->u.ifsmt.z->u.name);
    }else if(!strcmp(relop,">")){
       fprintf(fp,"\tbgt %s, %s, %s\n",regheap->regs[xregno]->regname,
        regheap->regs[yregno]->regname,codes->code->u.ifsmt.z->u.name);
    }else if(!strcmp(relop,">=")){
       fprintf(fp,"\tbge %s, %s, %s\n",regheap->regs[xregno]->regname,
        regheap->regs[yregno]->regname,codes->code->u.ifsmt.z->u.name);
    }else if(!strcmp(relop,"<")){
       fprintf(fp,"\tblt %s, %s, %s\n",regheap->regs[xregno]->regname,
        regheap->regs[yregno]->regname,codes->code->u.ifsmt.z->u.name);
    }else if(!strcmp(relop,"<=")){
       fprintf(fp,"\tble %s, %s, %s\n",regheap->regs[xregno]->regname,
        regheap->regs[yregno]->regname,codes->code->u.ifsmt.z->u.name);
    }
   }else if(kind == IR_DEC){
    regno = usevarreg(fp,codes->code->u.unop.op);
    fprintf(fp,"\tla %s, %s\n",regheap->regs[regno]->regname,codes->code->u.unop.op->u.name);
   }
}

void pushtmp(FILE *fp){
   fprintf(fp, "\taddi sp, sp, -68\n");
   for(int i=T0;i<=T2;i++)
      fprintf(fp,"\tsw %s, %d(sp)\n",regheap->regs[i]->regname,(i-T0)*4);
   for(int i=T3;i<=T6;i++)
      fprintf(fp,"\tsw %s, %d(sp)\n",regheap->regs[i]->regname,(i-T3+3)*4);
   for(int i=S2;i<=S11;i++)
      fprintf(fp,"\tsw %s, %d(sp)\n",regheap->regs[i]->regname,(i-S2+7)*4);    
}

void poptmp(FILE *fp){
   for(int i=T0;i<=T2;i++)
      fprintf(fp,"\tlw %s, %d(sp)\n",regheap->regs[i]->regname,(i-T0)*4);
   for(int i=T3;i<=T6;i++)
      fprintf(fp,"\tlw %s, %d(sp)\n",regheap->regs[i]->regname,(i-T3+3)*4);
   for(int i=S2;i<=S11;i++)
      fprintf(fp,"\tlw %s, %d(sp)\n",regheap->regs[i]->regname,(i-S2+7)*4);    
   fprintf(fp, "\taddi sp, sp, 68\n");
}