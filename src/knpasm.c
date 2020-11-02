#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <knpasm/knpcore.h>

static KNP_PROGRAM prog;

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

		if (!strcmp(argv[2], "-i"))
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
