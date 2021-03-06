﻿#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <knpasm/knpcore.h>
#include <knpasm/knputil.h>

static KNP_PROGRAM prog;

static char  _r0str[] = "R0";
static char  _r1str[] = "R1";
static char  _r2str[] = "R2";
static char  _r3str[] = "R3";
static char  _r4str[] = "R4";
static char  _r5str[] = "R5";
static char  _r6str[] = "R6";
static char  _r7str[] = "R7";
static char* valid_idn[] = { _r0str, _r1str, _r2str, _r3str, _r4str, _r5str, _r6str, _r7str };

static inline void stepcommand(char* linebuf)
{
	if (!linebuf[4])
	{
		step();
		return;
	}

	char* pC = linebuf + 4;
	if (!iswhitespace(*pC))
	{
		printf("Invalid step command: %s\n", linebuf);
		return;
	}
	while (iswhitespace(*(++pC)));
	char* exprstr = pC;
	bool numstr = true;
	for (; *pC; pC++)
		if (*pC < '0' || '9' < *pC)
		{
			numstr = false;
			break;
		}

	if (numstr)
	{
		size_t n = atoi(exprstr);
		for (; n != 0; n--)
			step();
	}

	// evaluate as an expression
	expr_t* pexpr = make_expr(exprstr, valid_idn, sizeof(valid_idn)/sizeof(void*));
	if (!pexpr)
	{
		printf("Invalid expression\n");
		return;
	}

	idnmap_t map;
	idnmap_init(&map);
	for (int i = 0; i < sizeof(valid_idn) / sizeof(void*); i++)
		idnmap_insert(&map, valid_idn[i], 0);

	int16_t val;
	while (1)
	{
		step();
		for (int i = 0; i < sizeof(valid_idn) / sizeof(void*); i++)
		{
			getreg(i, &val);
			idnmap_replace(&map, valid_idn[i], val);
		}
		if (eval_expr(pexpr, &map))
			break;
	}
	free_expr(pexpr);
}

static inline void regcommand(char* linebuf)
{
	int16_t reg;
	if (!linebuf[3])
	{
		for (int i = 0; i < 8; i++)
		{
			getreg(i, &reg);
			printf("Register %d: %5d\n", i, reg);
		}
		return;
	}

	size_t offset = 3;
	if (!iswhitespace(linebuf[3]))
	{
		printf("Invalid reg command: %s\n", linebuf);
		return;
	}
	while (iswhitespace(linebuf[++offset]));
	if (linebuf[offset] < '0' || '7' < linebuf[offset] || linebuf[offset + 1])
	{
		printf("Invalid reg command: %s\n", linebuf);
		return;
	}
	uint8_t val = linebuf[offset] - '0';
	getreg(val, &reg);
	printf("Register %d: %5d\n", val, reg);

	printf("Not implemented\n");
}

static inline void memcommand(char* linebuf)
{
	if (!linebuf[3])
	{
		printf("Invalid mem command: %s\n", linebuf);
		return;
	}

	size_t offset = 3;
	if (!iswhitespace(linebuf[3]))
	{
		printf("Invalid mem command: %s\n", linebuf);
		return;
	}
	while (iswhitespace(linebuf[++offset]));
	if (linebuf[offset] != '#')
	{
		printf("Invalid mem command: %s\n", linebuf);
		return;
	}

	uint16_t addr = 0;
	while (linebuf[++offset])
	{
		if (!ishex(linebuf[offset]))
		{
			printf("Invalid mem command: %s\n", linebuf);
			return;
		}
		addr <<= 4;
		addr += hctoi(linebuf[offset]);
	}

	printf("Memory Address 0x%04x: %5d\n", addr, getmem(addr));
}

static void runtest()
{
	KNP_INSTRUCTION pknpi[] =
	{
		{ MVI, KNP_OPRAND_R0, KNP_OPRAND_IMM(5) },
		{ ADD, KNP_OPRAND_R0, KNP_OPRAND_R0, KNP_OPRAND_R0 },
		{ OUT, KNP_OPRAND_R0 },
		{ JMP, KNP_OPRAND_IMM(1) }
	};
	memcpy(prog.pknpi, pknpi, sizeof(pknpi));
	prog.knpi_size = 4;

	loadprogram(&prog, 0);
	while (1)
		step();
	exit(0);
}

static void runendless(char* asm_file)
{
	FILE* pf = fopen(asm_file, "r");
	if (!pf)
	{
		printf("Unable to open file %s\n", asm_file);
		exit(1);
	}
	readprogram(&prog, pf);
	fclose(pf);
	loadprogram(&prog, 0);
	while (1)
		step();
	exit(0);
}

static void runstep(char* asm_file, uint16_t n)
{
	FILE* pf = fopen(asm_file, "r");
	readprogram(&prog, pf);
	fclose(pf);
	loadprogram(&prog, 0);
	for (; n != 0; n--)
		step();
	exit(0);
}

static void runinteractive(char* asm_file)
{
	FILE* pf = fopen(asm_file, "r");
	readprogram(&prog, pf);
	fclose(pf);
	loadprogram(&prog, 0);

	int linesize = 10;
	char* linebuf = malloc(sizeof(char) * linesize);
	while (1)
	{
		linesize = readline(&linebuf, linesize);
		if (linesize == 0)
		{
			printf("An error occured while reading your command\n");
			break;
		}
		if (!strcmp(linebuf, "break"))
			break;
		else if (!strncmp(linebuf, "step", 4))
			stepcommand(linebuf);
		else if (!strncmp(linebuf, "reg", 3))
			regcommand(linebuf);
		else if (!strncmp(linebuf, "mem", 3))
			memcommand(linebuf);
		else if (!strcmp(linebuf, "ins"))
			printknpi(getins());
		else
			printf("Invalid command: %s\n", linebuf);
		printf("\n");
	}
	free(linebuf);
	exit(0);
}

int main(int argc, char *argv[])
{
	if (argc == 1)
		runtest();

	else if (argc == 2)
		runendless(argv[1]);

	else if (argc == 3)
	{
		// run interactive

		if (strcmp(argv[2], "-i"))
		{
			printf("Invalid argument: %s\n", argv[2]);
			exit(1);
		}

		runinteractive(argv[1]);
	}

	else if (argc == 4)
	{
		// run n steps

		if (strcmp(argv[2], "--step") && strcmp(argv[2], "-n"))
		{
			printf("Invalid argument: %s\n", argv[2]);
			exit(1);
		}

		for (char* pC = argv[3]; *pC; pC++)
			if (*pC < '0' || '9' < *pC)
			{
				printf("Invalid argument: %s\n", argv[3]);
				exit(1);
			}
		
		runstep(argv[1], (uint16_t)atoi(argv[3]));
	}

	else
	{
		printf("Invalid arguments\n");
		exit(1);
	}
}
