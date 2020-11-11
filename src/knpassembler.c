#include <stdbool.h>
#include <string.h>

#include <knpasm/knpcore.h>
#include <knpasm/knputil.h>

#define MAX_LABELS 256
#define MAX_LABEL_SIZE 64

char    labelstr[MAX_LABELS][MAX_LABEL_SIZE];
int16_t labelval[MAX_LABELS];

inline bool iscomment(const char* pC)
{
	return *pC == '/' && pC[1] == '/';
}

void cleanline(char* dst, const char* src)
{
	int i = 0;
	bool prefix = true;
	char* pC = src;
	while (iswhitespace(*pC))
		pC++;
	
	while (pC[1] && !iscomment(pC))
	{
		dst[i++] = *pC;
		pC++;
	}
	if (*pC != '/')
		dst[i] = *pC;
	i--;

	for (; iswhitespace(dst[i]); i--);
	
	dst[i + 1] = '\0';
}

KNP_OPCODE knpiopcode(char* strknpi)
{
	if (!strncmp(strknpi, "MRR ", 4))
		return MRR;
	if (!strncmp(strknpi, "MVI ", 4))
		return MVI;
	if (!strncmp(strknpi, "LDMR ", 5))
		return LDMR;
	if (!strncmp(strknpi, "STRM ", 5))
		return STRM;
	if (!strncmp(strknpi, "ADD ", 4))
		return ADD;
	if (!strncmp(strknpi, "SUB ", 4))
		return SUB;
	if (!strncmp(strknpi, "INC ", 4))
		return INC;
	if (!strncmp(strknpi, "DEC ", 4))
		return DEC;
	if (!strncmp(strknpi, "AND ", 4))
		return AND;
	if (!strncmp(strknpi, "OR ", 3))
		return OR;
	if (!strncmp(strknpi, "XOR ", 4))
		return XOR;
	if (!strncmp(strknpi, "NOT ", 4))
		return NOT;
	if (!strncmp(strknpi, "JMP ", 4))
		return JMP;
	if (!strncmp(strknpi, "JZ ", 3))
		return JZ;
	if (!strncmp(strknpi, "JN ", 3))
		return JN;
	if (!strncmp(strknpi, "OUT ", 4))
		return OUT;

	return NOOP;
}

void readprogram(KNP_PROGRAM* pprog, FILE* pf)
{
#define LINESIZE 128

	size_t label_idx = 0;
	size_t knpi_idx = 0;
	size_t line_num = 0;
	char buf[LINESIZE];
	char* linebuf = buf;

	while (fgets(linebuf, LINESIZE, pf) != NULL)
	{
		line_num++;
		if (knpi_idx > MAX_KNPI)
			return 1;

		cleanline(linebuf, linebuf);
		if (!*linebuf)
			continue;
		touppercase(linebuf);
		
		KNP_OPCODE opc = knpiopcode(linebuf);
		if (opc == NOOP)
		{
			// must be a label
			char* lbl_offset = labelstr[label_idx];
			bool valid_lbl = false;
			for (char* pC = linebuf; *pC; pC++)
			{
				if (!isalphanumeric(*pC))
				{
					if (*pC != ':' || pC[1] != '\0')
						goto SYNTAX_ERROR;
					valid_lbl = true;
					break;
				}
				else
				{
					*lbl_offset = *pC;
					lbl_offset++;
					labelval[label_idx] = knpi_idx;
				}
			}
			*lbl_offset = 0;
			if (!valid_lbl)
				goto SYNTAX_ERROR;
			for (int i = 0; i < label_idx; i++)
				if (!strcmp(labelstr[i], labelstr[label_idx]))
				{
					// either the label is defined twice,
					// or it was already encountered in a j* knpi
					if (labelval[i] != -1)
					{
						// label was defined twice
						printf("Multiple definitions of label %s found\n", labelstr[i]);
						exit(1);
					}

					// already encountered in a j* knpi using the label
					labelval[i] = knpi_idx;
					label_idx--;
					break;
				}
			label_idx++;
			continue;
		}

		uint8_t  knpi_offset = 0;
		uint64_t val1, val2, val3;
		switch (opc)
		{
		case MRR:
		case INC:
		case DEC:
		case NOT:
			knpi_offset = 3;

			while (iswhitespace(linebuf[++knpi_offset]));
			if (linebuf[knpi_offset] != 'R')
				goto SYNTAX_ERROR;
			val1 = (uint64_t)linebuf[++knpi_offset] - '0';
			if (val1 < 0 || val1 > 7)
				goto REGNUM_ERROR;

			while (iswhitespace(linebuf[++knpi_offset]));
			if (linebuf[knpi_offset] != ',')
				goto SYNTAX_ERROR;
			while (iswhitespace(linebuf[++knpi_offset]));
			if (linebuf[knpi_offset] != 'R')
				goto SYNTAX_ERROR;
			val2 = (uint64_t)linebuf[++knpi_offset] - '0';
			if (val2 < 0 || val2 > 7)
				goto REGNUM_ERROR;

			if (linebuf[++knpi_offset])
				goto SYNTAX_ERROR;
			pprog->pknpi[knpi_idx++] = (KNP_INSTRUCTION)
			{
				opc,
				KNP_OPRAND_REG(val1),
				KNP_OPRAND_REG(val2)
			};
			break;
		
		case LDMR:
		case STRM:
			knpi_offset = 4;

			while (iswhitespace(linebuf[++knpi_offset]));
			if (linebuf[knpi_offset] != 'R')
				goto SYNTAX_ERROR;
			val1 = (uint64_t)linebuf[++knpi_offset] - '0';
			if (val1 < 0 || val1 > 7)
				goto REGNUM_ERROR;

			while (iswhitespace(linebuf[++knpi_offset]));
			if (linebuf[knpi_offset] != ',')
				goto SYNTAX_ERROR;
			while (iswhitespace(linebuf[++knpi_offset]));
			if (linebuf[knpi_offset] != 'R')
				goto SYNTAX_ERROR;
			val2 = (uint64_t)linebuf[++knpi_offset] - '0';
			if (val2 < 0 || val2 > 7)
				goto REGNUM_ERROR;

			if (linebuf[++knpi_offset])
				goto SYNTAX_ERROR;
			pprog->pknpi[knpi_idx++] = (KNP_INSTRUCTION)
			{
				opc,
				KNP_OPRAND_REG(val1),
				KNP_OPRAND_REG(val2)
			};
			break;
		
		case ADD:
		case SUB:
		case AND:
		case XOR:
			knpi_offset = 3;

			while (iswhitespace(linebuf[++knpi_offset]));
			if (linebuf[knpi_offset] != 'R')
				goto SYNTAX_ERROR;
			val1 = (uint64_t)linebuf[++knpi_offset] - '0';
			if (val1 < 0 || val1 > 7)
				goto REGNUM_ERROR;

			while (iswhitespace(linebuf[++knpi_offset]));
			if (linebuf[knpi_offset] != ',')
				goto SYNTAX_ERROR;
			while (iswhitespace(linebuf[++knpi_offset]));
			if (linebuf[knpi_offset] != 'R')
				goto SYNTAX_ERROR;
			val2 = (uint64_t)linebuf[++knpi_offset] - '0';
			if (val2 < 0 || val2 > 7)
				goto REGNUM_ERROR;

			while (iswhitespace(linebuf[++knpi_offset]));
			if (linebuf[knpi_offset] != ',')
				goto SYNTAX_ERROR;
			while (iswhitespace(linebuf[++knpi_offset]));
			if (linebuf[knpi_offset] != 'R')
				goto SYNTAX_ERROR;
			val3 = (uint64_t)linebuf[++knpi_offset] - '0';
			if (val3 < 0 || val3 > 7)
				goto REGNUM_ERROR;
			
			if (linebuf[++knpi_offset])
				goto SYNTAX_ERROR;
			pprog->pknpi[knpi_idx++] = (KNP_INSTRUCTION)
			{
				opc,
				KNP_OPRAND_REG(val1),
				KNP_OPRAND_REG(val2),
				KNP_OPRAND_REG(val3)
			};
			break;

		case OR:
			knpi_offset = 2;

			while (iswhitespace(linebuf[++knpi_offset]));
			if (linebuf[knpi_offset] != 'R')
				goto SYNTAX_ERROR;
			val1 = (uint64_t)linebuf[++knpi_offset] - '0';
			if (val1 < 0 || val1 > 7)
				goto REGNUM_ERROR;

			while (iswhitespace(linebuf[++knpi_offset]));
			if (linebuf[knpi_offset] != ',')
				goto SYNTAX_ERROR;
			while (iswhitespace(linebuf[++knpi_offset]));
			if (linebuf[knpi_offset] != 'R')
				goto SYNTAX_ERROR;
			val2 = (uint64_t)linebuf[++knpi_offset] - '0';
			if (val2 < 0 || val2 > 7)
				goto REGNUM_ERROR;

			while (iswhitespace(linebuf[++knpi_offset]));
			if (linebuf[knpi_offset] != ',')
				goto SYNTAX_ERROR;
			while (iswhitespace(linebuf[++knpi_offset]));
			if (linebuf[knpi_offset] != 'R')
				goto SYNTAX_ERROR;
			val3 = (uint64_t)linebuf[++knpi_offset] - '0';
			if (val3 < 0 || val3 > 7)
				goto REGNUM_ERROR;

			if (linebuf[++knpi_offset])
				goto SYNTAX_ERROR;
			pprog->pknpi[knpi_idx++] = (KNP_INSTRUCTION)
			{
				opc,
				KNP_OPRAND_REG(val1),
				KNP_OPRAND_REG(val2),
				KNP_OPRAND_REG(val3)
			};
			break;

		case MVI:
			knpi_offset = 3;

			while (iswhitespace(linebuf[++knpi_offset]));
			if (linebuf[knpi_offset++] != 'R')
				goto SYNTAX_ERROR;
			val1 = (uint64_t)linebuf[knpi_offset] - '0';
			if (val1 < 0 || val1 > 7)
				goto REGNUM_ERROR;

			while (iswhitespace(linebuf[++knpi_offset]));
			if (linebuf[knpi_offset] != ',')
				goto SYNTAX_ERROR;
			while (iswhitespace(linebuf[++knpi_offset]));
			if (linebuf[knpi_offset++] != '#')
				goto SYNTAX_ERROR;
			if (!ishex(linebuf[knpi_offset]))
				goto SYNTAX_ERROR;
			val2 = hctoi(linebuf[knpi_offset++]);
			while (ishex(linebuf[knpi_offset]))
			{
				val2 <<= 4;
				val2 += hctoi(linebuf[knpi_offset++]);
			}
			if (val2 > 0xFFFF)
			{
				printf("Immediate value overflow on line %zu\n", line_num);
				exit(1);
			}

			if (linebuf[knpi_offset])
				goto SYNTAX_ERROR;
			pprog->pknpi[knpi_idx++] = (KNP_INSTRUCTION)
			{
				opc,
				KNP_OPRAND_REG(val1),
				KNP_OPRAND_IMM(val2)
			};
			break;

		case JMP:
			knpi_offset = 3;
			while (iswhitespace(linebuf[++knpi_offset]));

			{
				uint16_t i;
				for (i = 0; i < label_idx; i++)
					if (!strcmp(labelstr[i], linebuf + knpi_offset))
					{
						// label definition was found
						val1 = i;
						break;
					}
				if (i == label_idx)
				{
					// ensuring the jmp knpi was valid
					for (char* pC = linebuf + knpi_offset; *pC; pC++)
						if (!isalphanumeric(*pC))
							goto SYNTAX_ERROR; // invalid oprand for jmp

					// label definition was not found, creating an empty one
					strcpy(labelstr[label_idx], linebuf + knpi_offset);
					labelval[label_idx] = -1;
					val1 = label_idx++;
				}
			}

			pprog->pknpi[knpi_idx++] = (KNP_INSTRUCTION)
			{
				opc,
				KNP_OPRAND_IMM(val1)
			};
			break;

		case JZ:
		case JN:
			knpi_offset = 2;

			while (iswhitespace(linebuf[++knpi_offset]));
			if (linebuf[knpi_offset++] != 'R')
				goto SYNTAX_ERROR;
			val1 = (uint64_t)linebuf[knpi_offset] - '0';
			if (val1 < 0 || val1 > 7)
				goto REGNUM_ERROR;
			while (iswhitespace(linebuf[++knpi_offset]));
			if (linebuf[knpi_offset++] != ',')
				goto SYNTAX_ERROR;
			while (iswhitespace(linebuf[++knpi_offset]));

			{
				uint16_t i;
				for (i = 0; i < label_idx; i++)
					if (!strcmp(labelstr[i], linebuf + knpi_offset))
					{
						// label definition was found
						val2 = i;
						break;
					}
				if (i == label_idx)
				{
					// ensuring the jmp knpi was valid
					for (char* pC = linebuf + knpi_offset; *pC; pC++)
						if (!isalphanumeric(*pC))
							goto SYNTAX_ERROR; // invalid oprand for jmp

					// label definition was not found, creating an empty one
					strcpy(labelstr[label_idx], linebuf + knpi_offset);
					labelval[label_idx] = -1;
					val2 = label_idx++;
				}
			}

			pprog->pknpi[knpi_idx++] = (KNP_INSTRUCTION)
			{
				opc,
				KNP_OPRAND_REG(val1),
				KNP_OPRAND_IMM(val2)
			};
			break;

		case OUT:
			knpi_offset = 3;

			while (iswhitespace(linebuf[++knpi_offset]));
			if (linebuf[knpi_offset++] != 'R')
				goto SYNTAX_ERROR;
			val1 = (uint64_t)linebuf[knpi_offset++] - '0';
			if (val1 < 0 || val1 > 7)
				goto REGNUM_ERROR;

			if (linebuf[knpi_offset])
				goto SYNTAX_ERROR;
			pprog->pknpi[knpi_idx++] = (KNP_INSTRUCTION)
			{
				opc,
				KNP_OPRAND_REG(val1)
			};
			break;
		}

		linebuf = buf;
		continue;

	SYNTAX_ERROR:
		if (opc != NOOP)
		{
			char buf[5] = "";
			opctoa(opc, buf);
			printf("Syntax error reading %s instruction on line %zu\n", buf, line_num);
		}
		else
			printf("Syntax error on line: %zu\n", line_num);
		exit(1);

	REGNUM_ERROR:
		printf("Encountered invalid register on line %zu\n", line_num);
		exit(1);
	}
	pprog->knpi_size = knpi_idx;

	// replacing the jmp refs with knpi line values
	for (int i = 0; i < pprog->knpi_size; i++)
	{
		KNP_OPCODE opc = pprog->pknpi[i].opc;
		if (opc == JMP)
			pprog->pknpi[i].opr1.val = labelval[pprog->pknpi[i].opr1.val];
		else if (opc == JZ || opc == JN)
			pprog->pknpi[i].opr2.val = labelval[pprog->pknpi[i].opr2.val];
	}
}
