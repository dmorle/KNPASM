#include <knpasm/knpcore.h>

static int16_t r0;
static int16_t r1;
static int16_t r2;
static int16_t r3;
static int16_t r4;
static int16_t r5;
static int16_t r6;
static int16_t r7;

static int16_t ram[MAX_ADDR + 1];

static KNP_INSTRUCTION knpi_mem[MAX_KNPI + 1];
static uint16_t pc;
static uint16_t po;

void knpi_mrr(KNP_OPRAND op1, KNP_OPRAND op2, KNP_OPRAND op3)
{
	if (op1.ty != REG || op2.ty != REG || op3.ty != NUL)
	{
		printf("Invalid opcode types for a MRR instruction\n");
		exit(1);
	}

	int16_t val;
	getreg(op2.val, &val);
	setreg(op1.val,  val);
}

void knpi_mvi(KNP_OPRAND op1, KNP_OPRAND op2, KNP_OPRAND op3)
{
	if (op1.ty != REG || op2.ty != IMM || op3.ty != NUL)
	{
		printf("Invalid opcode types for an MVI instruction\n");
		exit(1);
	}

	setreg(op1.val, op2.val);
}

void knpi_ldmr(KNP_OPRAND op1, KNP_OPRAND op2, KNP_OPRAND op3)
{
	if (op1.ty != REG || op2.ty != REG || op3.ty != NUL)
	{
		printf("Invalid opcode types for a LDMR instruction\n");
		exit(1);
	}

	int16_t val;
	getreg(op2.val, &val);
	setreg(op1.val, ram[val]);
}

void knpi_strm(KNP_OPRAND op1, KNP_OPRAND op2, KNP_OPRAND op3)
{
	if (op1.ty != REG || op2.ty != REG || op3.ty != NUL)
	{
		printf("Invalid opcode types for an STRM instruction\n");
		exit(1);
	}

	int16_t val1, val2;
	getreg(op1.val, &val1);
	getreg(op2.val, &val2);
	ram[val2] = val1;
}

void knpi_add(KNP_OPRAND op1, KNP_OPRAND op2, KNP_OPRAND op3)
{
	if (op1.ty != REG || op2.ty != REG || op3.ty != REG)
	{
		printf("Invalid opcode types for an ADD instruction\n");
		exit(1);
	}

	int16_t val1, val2;
	getreg(op2.val, &val1);
	getreg(op3.val, &val2);
	setreg(op1.val, val1 + val2);
}

void knpi_sub(KNP_OPRAND op1, KNP_OPRAND op2, KNP_OPRAND op3)
{
	if (op1.ty != REG || op2.ty != REG || op3.ty != REG)
	{
		printf("Invalid opcode types for a SUB instruction\n");
		exit(1);
	}

	int16_t val1, val2;
	getreg(op2.val, &val1);
	getreg(op3.val, &val2);
	setreg(op1.val, val1 - val2);
}

void knpi_inc(KNP_OPRAND op1, KNP_OPRAND op2, KNP_OPRAND op3)
{
	if (op1.ty != REG || op2.ty != REG || op3.ty != NUL)
	{
		printf("Invalid opcode types for an INC instruction\n");
		exit(1);
	}

	int16_t val;
	getreg(op2.val, &val);
	setreg(op1.val, val + 1);
}

void knpi_dec(KNP_OPRAND op1, KNP_OPRAND op2, KNP_OPRAND op3)
{
	if (op1.ty != REG || op2.ty != REG || op3.ty != NUL)
	{
		printf("Invalid opcode types for a DEC instruction\n");
		exit(1);
	}

	int16_t val;
	getreg(op2.val, &val);
	setreg(op1.val, val - 1);
}

void knpi_and(KNP_OPRAND op1, KNP_OPRAND op2, KNP_OPRAND op3)
{
	if (op1.ty != REG || op2.ty != REG || op3.ty != REG)
	{
		printf("Invalid opcode types for an AND instruction\n");
		exit(1);
	}

	int16_t val1, val2;
	getreg(op2.val, &val1);
	getreg(op3.val, &val2);
	setreg(op1.val, val1 & val2);
}

void knpi_or(KNP_OPRAND op1, KNP_OPRAND op2, KNP_OPRAND op3)
{
	if (op1.ty != REG || op2.ty != REG || op3.ty != REG)
	{
		printf("Invalid opcode types for an OR instruction\n");
		exit(1);
	}

	int16_t val1, val2;
	getreg(op2.val, &val1);
	getreg(op3.val, &val2);
	setreg(op1.val, val1 | val2);
}

void knpi_xor(KNP_OPRAND op1, KNP_OPRAND op2, KNP_OPRAND op3)
{
	if (op1.ty != REG || op2.ty != REG || op3.ty != REG)
	{
		printf("Invalid opcode types for an XOR instruction\n");
		exit(1);
	}

	int16_t val1, val2;
	getreg(op2.val, &val1);
	getreg(op3.val, &val2);
	setreg(op1.val, val1 ^ val2);
}

void knpi_not(KNP_OPRAND op1, KNP_OPRAND op2, KNP_OPRAND op3)
{
	if (op1.ty != REG || op2.ty != REG || op3.ty != NUL)
	{
		printf("Invalid opcode types for a NOT instruction\n");
		exit(1);
	}

	int16_t val;
	getreg(op2.val, &val);
	setreg(op1.val, ~val);
}

void knpi_jmp(KNP_OPRAND op1, KNP_OPRAND op2, KNP_OPRAND op3)
{
	if (op1.ty != IMM || op2.ty != NUL || op3.ty != NUL)
	{
		printf("Invalid opcode types for a JMP instruction\n");
		exit(1);
	}

	pc = po + op1.val;
}

void knpi_jz(KNP_OPRAND op1, KNP_OPRAND op2, KNP_OPRAND op3)
{
	if (op1.ty != REG || op2.ty != IMM || op3.ty != NUL)
	{
		printf("Invalid opcode types for a JZ instruction\n");
		exit(1);
	}

	int16_t val;
	getreg(op1.val, &val);
	if (!val)
		pc = po + op2.val;
}

void knpi_jn(KNP_OPRAND op1, KNP_OPRAND op2, KNP_OPRAND op3)
{
	if (op1.ty != REG || op2.ty != IMM || op3.ty != NUL)
	{
		printf("Invalid opcode types for a JN instruction\n");
		exit(1);
	}

	int16_t val;
	getreg(op1.val, &val);
	if (val < 0)
		pc = po + op2.val;
}

void knpi_out(KNP_OPRAND op1, KNP_OPRAND op2, KNP_OPRAND op3)
{
	if (op1.ty != REG || op2.ty != NUL || op3.ty != NUL)
	{
		printf("Invalid opcode types for an OUT instruction\n");
		exit(1);
	}
	
	int16_t val;
	getreg(op1.val, &val);
	printf("Register %d: %5d\n", op1.val, val);
}


void loadprogram(KNP_PROGRAM* pknpp, uint16_t offset)
{
	if (pknpp->knpi_size + offset > MAX_KNPI)
	{
		// program memory overflow
		printf(
			"Program size is too large to load into the instruction memory\n"
			"Program size: %u    Instruction memory size: %u    Offset: %u",
			MAX_KNPI, pknpp->knpi_size, offset
		);
		exit(1);
	}

	for (size_t i = offset; i < (size_t)pknpp->knpi_size + offset; i++)
		knpi_mem[i] = pknpp->pknpi[i];

	pc = po = offset;
}

void step()
{
	KNP_INSTRUCTION knpi = knpi_mem[pc++];

	switch (knpi.opc)
	{
	case MRR:
		return knpi_mrr (knpi.opr1, knpi.opr2, knpi.opr3);
	case MVI:
		return knpi_mvi (knpi.opr1, knpi.opr2, knpi.opr3);
	case LDMR:
		return knpi_ldmr(knpi.opr1, knpi.opr2, knpi.opr3);
	case STRM:
		return knpi_strm(knpi.opr1, knpi.opr2, knpi.opr3);
	case ADD:
		return knpi_add (knpi.opr1, knpi.opr2, knpi.opr3);
	case SUB:
		return knpi_sub (knpi.opr1, knpi.opr2, knpi.opr3);
	case INC:
		return knpi_inc (knpi.opr1, knpi.opr2, knpi.opr3);
	case DEC:
		return knpi_dec (knpi.opr1, knpi.opr2, knpi.opr3);
	case AND:
		return knpi_and (knpi.opr1, knpi.opr2, knpi.opr3);
	case OR:
		return knpi_or  (knpi.opr1, knpi.opr2, knpi.opr3);
	case XOR:
		return knpi_xor (knpi.opr1, knpi.opr2, knpi.opr3);
	case NOT:
		return knpi_not (knpi.opr1, knpi.opr2, knpi.opr3);
	case JMP:
		return knpi_jmp (knpi.opr1, knpi.opr2, knpi.opr3);
	case JZ:
		return knpi_jz  (knpi.opr1, knpi.opr2, knpi.opr3);
	case JN:
		return knpi_jn  (knpi.opr1, knpi.opr2, knpi.opr3);
	case OUT:
		return knpi_out (knpi.opr1, knpi.opr2, knpi.opr3);
	}
}

void getreg(KNP_OPRAND_VAL reg, int16_t* pval)
{
	switch (reg)
	{
	case KNP_OPRAND_VAL_R0:
		*pval = r0;
		return;
	case KNP_OPRAND_VAL_R1:
		*pval = r1;
		return;
	case KNP_OPRAND_VAL_R2:
		*pval = r2;
		return;
	case KNP_OPRAND_VAL_R3:
		*pval = r3;
		return;
	case KNP_OPRAND_VAL_R4:
		*pval = r4;
		return;
	case KNP_OPRAND_VAL_R5:
		*pval = r5;
		return;
	case KNP_OPRAND_VAL_R6:
		*pval = r6;
		return;
	case KNP_OPRAND_VAL_R7:
		*pval = r7;
		return;
	default:
		printf("Register %d is not a valid register\n", reg);
		exit(1);
	}
}

void setreg(KNP_OPRAND_VAL reg, int16_t nval)
{
	switch (reg)
	{
	case KNP_OPRAND_VAL_R0:
		r0 = nval;
		return;
	case KNP_OPRAND_VAL_R1:
		r1 = nval;
		return;
	case KNP_OPRAND_VAL_R2:
		r2 = nval;
		return;
	case KNP_OPRAND_VAL_R3:
		r3 = nval;
		return;
	case KNP_OPRAND_VAL_R4:
		r4 = nval;
		return;
	case KNP_OPRAND_VAL_R5:
		r5 = nval;
		return;
	case KNP_OPRAND_VAL_R6:
		r6 = nval;
		return;
	case KNP_OPRAND_VAL_R7:
		r7 = nval;
		return;
	default:
		printf("Register %d is not a valid register\n", reg);
		exit(1);
	}
}

int16_t getmem(uint16_t addr)
{
	return ram[addr];
}

KNP_INSTRUCTION getins()
{
	return knpi_mem[pc];
}
