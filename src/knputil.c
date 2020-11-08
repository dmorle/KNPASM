#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <knpasm/knputil.h>

bool iswhitespace(char c)
{
	return c == ' ' || c == '\t';
}

bool isalphanumeric(char c)
{
	return
		('0' <= c && c <= '9') ||
		('A' <= c && c <= 'Z') ||
		('a' <= c && c <= 'z') || c == '_';
}

bool ishex(char c)
{
	return
		('0' <= c && c <= '9') ||
		('A' <= c && c <= 'F');
}

uint8_t hctoi(char c)
{
	int cond = ('0' <= c && c <= '9');
	return (c - '0') * cond + (c + 0x0A - 'A') * (1 - cond);
}

void touppercase(char* str)
{
	for (char* pC = str; *pC; pC++)
		*pC -= 0x20 * ('a' <= *pC && *pC <= 'z');
}

size_t readline(char** pbuf, size_t n)
{
	bool complete = false;
	size_t idx = 0;

	while (!complete)
	{
		if (!fgets(*pbuf + idx, n, stdin))
			return 0;

		for (size_t i = idx; i < idx + n; i++)
		{
			if ((*pbuf)[i] == '\n')
			{
				(*pbuf)[i] = '\0';
				complete = true;
				goto RET;
			}
		}

		idx += n - 1;
		char* tmp = realloc(*pbuf, idx + n);
		if (!tmp)
		{
			free(*pbuf);
			printf("Out of memory\n");
			exit(1);
		}
		*pbuf = tmp;
	}

RET:
	return idx + n;
}

void printknpi(KNP_INSTRUCTION knpi)
{
	switch (knpi.opc)
	{
	case NOOP:
		printf("No instruction\n");
		break;
	case MRR:
		printf("MRR   R%d, R%d\n", knpi.opr1.val, knpi.opr2.val);
		break;
	case MVI:
		printf("MVI   R%d, #%04x\n", knpi.opr1.val, knpi.opr2.val);
		break;
	case LDMR:
		printf("LDMR  R%d, R%d\n", knpi.opr1.val, knpi.opr2.val);
		break;
	case STRM:
		printf("LDMR  R%d, R%d\n", knpi.opr1.val, knpi.opr2.val);
		break;
	case ADD:
		printf("ADD   R%d, R%d, R%d\n", knpi.opr1.val, knpi.opr2.val, knpi.opr3.val);
		break;
	case SUB:
		printf("SUB   R%d, R%d, R%d\n", knpi.opr1.val, knpi.opr2.val, knpi.opr3.val);
		break;
	case INC:
		printf("INC   R%d, R%d\n", knpi.opr1.val, knpi.opr2.val);
		break;
	case DEC:
		printf("DEC   R%d, R%d\n", knpi.opr1.val, knpi.opr2.val);
		break;
	case AND:
		printf("AND   R%d, R%d, R%d\n", knpi.opr1.val, knpi.opr2.val, knpi.opr3.val);
		break;
	case OR:
		printf("OR    R%d, R%d, R%d\n", knpi.opr1.val, knpi.opr2.val, knpi.opr3.val);
		break;
	case XOR:
		printf("XOR   R%d, R%d, R%d\n", knpi.opr1.val, knpi.opr2.val, knpi.opr3.val);
		break;
	case NOT:
		printf("NOT   R%d, R%d\n", knpi.opr1.val, knpi.opr2.val);
		break;
	case JMP:
		printf("JMP   #%04x\n", knpi.opr1.val);
		break;
	case JZ:
		printf("JZ    R%d, #%04x\n", knpi.opr1.val, knpi.opr2.val);
		break;
	case JN:
		printf("JN    R%d, #%04x\n", knpi.opr1.val, knpi.opr2.val);
		break;
	case OUT:
		printf("OUT   R%d\n", knpi.opr1.val);
		break;
	}
}

void opctoa(KNP_OPCODE opc, char* buf)
{
	switch (opc)
	{
	case NOOP:
		strcpy(buf, "NOOP");
		break;
	case MRR:
		strcpy(buf, "MRR");
		break;
	case MVI:
		strcpy(buf, "MVI");
		break;
	case LDMR:
		strcpy(buf, "LDMR");
		break;
	case STRM:
		strcpy(buf, "LDMR");
		break;
	case ADD:
		strcpy(buf, "ADD");
		break;
	case SUB:
		strcpy(buf, "SUB");
		break;
	case INC:
		strcpy(buf, "INC");
		break;
	case DEC:
		strcpy(buf, "DEC");
		break;
	case AND:
		strcpy(buf, "AND");
		break;
	case OR:
		strcpy(buf, "OR");
		break;
	case XOR:
		strcpy(buf, "XOR");
		break;
	case NOT:
		strcpy(buf, "NOT");
		break;
	case JMP:
		strcpy(buf, "JMP");
		break;
	case JZ:
		strcpy(buf, "JZ");
		break;
	case JN:
		strcpy(buf, "JN");
		break;
	case OUT:
		strcpy(buf, "OUT");
		break;
	}
}
