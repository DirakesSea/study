#ifndef __ENUM__
#define __ENUM__

typedef enum node_type{
  TOKEN_NUM,
  TOKEN_ID,
  TOKEN_TYPE,
  TOKEN_OTHER
}Nodetype;

typedef enum basic_type{
  VOID_TYPE,
  INT_TYPE
}Basictype;

typedef enum smt_kind{
  BASIC,
  ARRAY,
  FUNCTION
}Kind;

typedef enum op_kind{
     OP_VAR,
     OP_CONST,
     OP_LABEL,
     OP_FUNC,
     OP_RELOP,

} opkind;

typedef enum ir_kind{
    IR_LABEL,
    IR_FUNC,
    IR_GOTO,
    IR_RET,
    IR_PARAM,
    IR_ARG,
    IR_CALL,

    IR_ASSIGN,
    IR_MEM_READ,
    IR_MEM_WRITE,
    IR_ADDR_GET,
    
    IR_ADD,
    IR_SUB,
    IR_MUL,
    IR_DIV,
    IR_REM,

    IR_IF,
    
    IR_DEC,
    IR_GLOBAL,
    IR_WORD
} irkind;

typedef enum regnum{
  ZERO,
  RA,
  SP,
  GP,
  TP,
  T0,
  T1,
  T2,
  FP,
  S1,
  A0,
  A1,
  A2,
  A3,
  A4,
  A5,
  A6,
  A7,
  S2,
  S3,
  S4,
  S5,
  S6,
  S7,
  S8,
  S9,
  S10,
  S11,
  T3,
  T4,
  T5,
  T6
}regno;

#endif