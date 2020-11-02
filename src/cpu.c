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

KNP_RESULT knpi_mrr(KNP_OPRAND op1, KNP_OPRAND op2, KNP_OPRAND op3)
{
	if (op1.ty != REG || op2.ty != REG || op3.ty != NUL)
		return 1;

	int16_t val;
	getreg(op2.val, &val);
	setreg(op1.val,  val);
	
	return 0;
}

KNP_RESULT knpi_mvi(KNP_OPRAND op1, KNP_OPRAND op2, KNP_OPRAND op3)
{
	if (op1.ty != REG || op2.ty != IMM || op3.ty != NUL)
		return 1;

	setreg(op1.val, op2.val);

	return 0;
}

KNP_RESULT knpi_ldmr(KNP_OPRAND op1, KNP_OPRAND op2, KNP_OPRAND op3)
{
	if (op1.ty != REG || op2.ty != REG || op3.ty != NUL)
		return 1;

	int16_t val;
	getreg(op2.val, &val);
	setreg(op1.val, ram[val]);

	return 0;
}

KNP_RESULT knpi_strm(KNP_OPRAND op1, KNP_OPRAND op2, KNP_OPRAND op3)
{
	if (op1.ty != REG || op2.ty != REG || op3.ty != NUL)
		return 1;

	int16_t val1, val2;
	getreg(op1.val, &val1);
	getreg(op2.val, &val2);
	ram[val2] = val1;

	return 0;
}

KNP_RESULT knpi_add(KNP_OPRAND op1, KNP_OPRAND op2, KNP_OPRAND op3)
{
	if (op1.ty != REG || op2.ty != REG || op3.ty != REG)
		return 1;

	int16_t val1, val2;
	getreg(op2.val, &val1);
	getreg(op3.val, &val2);
	setreg(op1.val, val1 + val2);

	return 0;
}

KNP_RESULT knpi_sub(KNP_OPRAND op1, KNP_OPRAND op2, KNP_OPRAND op3)
{
	if (op1.ty != REG || op2.ty != REG || op3.ty != REG)
		return 1;

	int16_t val1, val2;
	getreg(op2.val, &val1);
	getreg(op3.val, &val2);
	setreg(op1.val, val1 - val2);

	return 0;
}

KNP_RESULT knpi_inc(KNP_OPRAND op1, KNP_OPRAND op2, KNP_OPRAND op3)
{
	if (op1.ty != REG || op2.ty != REG || op3.ty != NUL)
		return 1;

	int16_t val;
	getreg(op2.val, &val);
	setreg(op1.val, val + 1);

	return 0;
}

KNP_RESULT knpi_dec(KNP_OPRAND op1, KNP_OPRAND op2, KNP_OPRAND op3)
{
	if (op1.ty != REG || op2.ty != REG || op3.ty != NUL)
		return 1;

	int16_t val;
	getreg(op2.val, &val);
	setreg(op1.val, val - 1);

	return 0;
}

KNP_RESULT knpi_and(KNP_OPRAND op1, KNP_OPRAND op2, KNP_OPRAND op3)
{
	if (op1.ty != REG || op2.ty != REG || op3.ty != REG)
		return 1;

	int16_t val1, val2;
	getreg(op2.val, &val1);
	getreg(op3.val, &val2);
	setreg(op1.val, val1 & val2);

	return 0;
}

KNP_RESULT knpi_or(KNP_OPRAND op1, KNP_OPRAND op2, KNP_OPRAND op3)
{
	if (op1.ty != REG || op2.ty != REG || op3.ty != REG)
		return 1;

	int16_t val1, val2;
	getreg(op2.val, &val1);
	getreg(op3.val, &val2);
	setreg(op1.val, val1 | val2);

	return 0;
}

KNP_RESULT knpi_xor(KNP_OPRAND op1, KNP_OPRAND op2, KNP_OPRAND op3)
{
	if (op1.ty != REG || op2.ty != REG || op3.ty != REG)
		return 1;

	int16_t val1, val2;
	getreg(op2.val, &val1);
	getreg(op3.val, &val2);
	setreg(op1.val, val1 ^ val2);

	return 0;
}

KNP_RESULT knpi_not(KNP_OPRAND op1, KNP_OPRAND op2, KNP_OPRAND op3)
{
	if (op1.ty != REG || op2.ty != REG || op3.ty != NUL)
		return 1;

	int16_t val;
	getreg(op2.val, &val);
	setreg(op1.val, ~val);

	return 0;
}

KNP_RESULT knpi_jmp(KNP_OPRAND op1, KNP_OPRAND op2, KNP_OPRAND op3)
{
	if (op1.ty != IMM || op2.ty != NUL || op3.ty != NUL)
		return 1;

	pc = po + op1.val;

	return 0;
}

KNP_RESULT knpi_jz(KNP_OPRAND op1, KNP_OPRAND op2, KNP_OPRAND op3)
{
	if (op1.ty != REG || op2.ty != IMM || op3.ty != NUL)
		return 1;

	int16_t val;
	getreg(op1.val, &val);
	if (!val)
		pc = po + op2.val;

	return 0;
}

KNP_RESULT knpi_jn(KNP_OPRAND op1, KNP_OPRAND op2, KNP_OPRAND op3)
{
	if (op1.ty != REG || op2.ty != IMM || op3.ty != NUL)
		return 1;

	int16_t val;
	getreg(op1.val, &val);
	if (val < 0)
		pc = po + op2.val;

	return 0;
}

KNP_RESULT knpi_out(KNP_OPRAND op1, KNP_OPRAND op2, KNP_OPRAND op3)
{
	if (op1.ty != REG || op2.ty != NUL || op3.ty != NUL)
		return 1;
	
	KNP_RESULT kr = 0;
	int16_t val;
	kr = getreg(op1.val, &val);
	if (kr)
		return kr;
	printf("REG%d: %5d\n", op1.val, val);

	return 0;
}


KNP_RESULT loadprogram(KNP_PROGRAM* pknpp, uint16_t offset)
{
	if (pknpp->knpi_size + offset > MAX_KNPI)
		return 1; // program memory overflow

	for (size_t i = offset; i < (size_t)pknpp->knpi_size + offset; i++)
		knpi_mem[i] = pknpp->pknpi[i];

	pc = po = offset;

	return 0;
}

KNP_RESULT step()
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

KNP_RESULT getreg(KNP_OPRAND_VAL reg, int16_t* pval)
{
	switch (reg)
	{
	case KNP_OPRAND_VAL_R0:
		*pval = r0;
		return 0;
	case KNP_OPRAND_VAL_R1:
		*pval = r1;
		return 0;
	case KNP_OPRAND_VAL_R2:
		*pval = r2;
		return 0;
	case KNP_OPRAND_VAL_R3:
		*pval = r3;
		return 0;
	case KNP_OPRAND_VAL_R4:
		*pval = r4;
		return 0;
	case KNP_OPRAND_VAL_R5:
		*pval = r5;
		return 0;
	case KNP_OPRAND_VAL_R6:
		*pval = r6;
		return 0;
	case KNP_OPRAND_VAL_R7:
		*pval = r7;
		return 0;
	default:
		return 1;
	}
}

KNP_RESULT setreg(KNP_OPRAND_VAL reg, int16_t nval)
{
	switch (reg)
	{
	case KNP_OPRAND_VAL_R0:
		r0 = nval;
		return 0;
	case KNP_OPRAND_VAL_R1:
		r1 = nval;
		return 0;
	case KNP_OPRAND_VAL_R2:
		r2 = nval;
		return 0;
	case KNP_OPRAND_VAL_R3:
		r3 = nval;
		return 0;
	case KNP_OPRAND_VAL_R4:
		r4 = nval;
		return 0;
	case KNP_OPRAND_VAL_R5:
		r5 = nval;
		return 0;
	case KNP_OPRAND_VAL_R6:
		r6 = nval;
		return 0;
	case KNP_OPRAND_VAL_R7:
		r7 = nval;
		return 0;
	default:
		return 1;
	}
}
