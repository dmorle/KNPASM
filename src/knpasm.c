#include <stdio.h>
#include <stdlib.h>

#include <knpasm/knpcore.h>

int main(int argc, char *argv[])
{
	if (argc != 2)
		exit(1);

	KNP_PROGRAM prog;
	KNP_INSTRUCTION pknpi[] = {
		{ MVI, KNP_OPRAND_R0, KNP_OPRAND_IMM(5) },
		{ ADD, KNP_OPRAND_R0, KNP_OPRAND_R0, KNP_OPRAND_R0 },
		{ OUT, KNP_OPRAND_R0 },
		{ JMP, KNP_OPRAND_IMM(1) }
	};
	prog.pknpi = pknpi;
	prog.knpi_size = 4;

	loadprogram(&prog, 0);
	while (1)
		step();

	exit(0);
}
