#ifndef KNP_CORE_H
#define KNP_CORE_H

#include <stdint.h>

// Error identification and handling

typedef uint64_t KNP_RESULT;



// Instruction set

#define MAX_ADDR 0xFFFF
#define MAX_KNPI 0x0FFF

typedef enum
{
	MRR,
	MVI,
	LDMR,
	STRM,
	ADD,
	SUB,
	INC,
	DEC,
	AND,
	OR,
	XOR,
	NOT,
	JMP,
	JZ,
	NZ,
	OUT
}
KNP_OPCODE;

typedef uint16_t KNP_OPRAND_VAL;

#define KNP_OPRAND_VAL_R0 0
#define KNP_OPRAND_VAL_R1 1
#define KNP_OPRAND_VAL_R2 2
#define KNP_OPRAND_VAL_R3 3
#define KNP_OPRAND_VAL_R4 4
#define KNP_OPRAND_VAL_R5 5
#define KNP_OPRAND_VAL_R6 6
#define KNP_OPRAND_VAL_R7 7

typedef enum
{
	NUL,
	IMM,
	REG
}
KNP_OPRAND_TYPE;

typedef struct
{
	KNP_OPRAND_TYPE ty;
	KNP_OPRAND_VAL  val;
}
KNP_OPRAND;

#define KNP_OPRAND_R0 { REG, KNP_OPRAND_VAL_R0 }
#define KNP_OPRAND_R1 { REG, KNP_OPRAND_VAL_R1 }
#define KNP_OPRAND_R2 { REG, KNP_OPRAND_VAL_R2 }
#define KNP_OPRAND_R3 { REG, KNP_OPRAND_VAL_R3 }
#define KNP_OPRAND_R4 { REG, KNP_OPRAND_VAL_R4 }
#define KNP_OPRAND_R5 { REG, KNP_OPRAND_VAL_R5 }
#define KNP_OPRAND_R6 { REG, KNP_OPRAND_VAL_R6 }
#define KNP_OPRAND_R7 { REG, KNP_OPRAND_VAL_R7 }

#define KNP_OPRAND_IMM(val) { IMM, val }

typedef struct 
{
	KNP_OPCODE opc;
	KNP_OPRAND opr1;
	KNP_OPRAND opr2;
	KNP_OPRAND opr3;
}
KNP_INSTRUCTION;

inline KNP_RESULT knpi_mrr  ( KNP_OPRAND op1, KNP_OPRAND op2                 );
inline KNP_RESULT knpi_mvi  ( KNP_OPRAND op1, KNP_OPRAND op2                 );
inline KNP_RESULT knpi_ldmr ( KNP_OPRAND op1, KNP_OPRAND op2                 );
inline KNP_RESULT knpi_strm ( KNP_OPRAND op1, KNP_OPRAND op2                 );
inline KNP_RESULT knpi_add  ( KNP_OPRAND op1, KNP_OPRAND op2, KNP_OPRAND op3 );
inline KNP_RESULT knpi_sub  ( KNP_OPRAND op1, KNP_OPRAND op2, KNP_OPRAND op3 );
inline KNP_RESULT knpi_inc  ( KNP_OPRAND op1, KNP_OPRAND op2                 );
inline KNP_RESULT knpi_dec  ( KNP_OPRAND op1, KNP_OPRAND op2                 );
inline KNP_RESULT knpi_and  ( KNP_OPRAND op1, KNP_OPRAND op2, KNP_OPRAND op3 );
inline KNP_RESULT knpi_or   ( KNP_OPRAND op1, KNP_OPRAND op2, KNP_OPRAND op3 );
inline KNP_RESULT knpi_xor  ( KNP_OPRAND op1, KNP_OPRAND op2, KNP_OPRAND op3 );
inline KNP_RESULT knpi_not  ( KNP_OPRAND op1, KNP_OPRAND op2                 );
inline KNP_RESULT knpi_jmp  ( KNP_OPRAND op1                                 );
inline KNP_RESULT knpi_jz   ( KNP_OPRAND op1, KNP_OPRAND op2                 );
inline KNP_RESULT knpi_jn   ( KNP_OPRAND op1, KNP_OPRAND op2                 );
inline KNP_RESULT knpi_out  ( KNP_OPRAND op1                                 );

// KNP programs

typedef struct
{
	uint16_t knpi_size;
	KNP_INSTRUCTION* pknpi;
}
KNP_PROGRAM;

KNP_RESULT loadprogram(KNP_PROGRAM* pknpp, uint16_t offset);

KNP_RESULT step();

KNP_RESULT getreg(KNP_OPRAND_VAL reg, int16_t* pval);
KNP_RESULT setreg(KNP_OPRAND_VAL reg, int16_t  nval);

#endif
