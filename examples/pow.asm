// step for 90 clock cycles

	//					int base = 5;
	//					int exp = 4;
	mvi  r0, #5
	mvi  r1, #4

	//					int pow = base;
	mrr  r2, r0
	//					for (int i = 0; i < exp - 1; i++)
	//					{
	mrr  r3, r1
	dec  r3, r3

POW_LOOP:
	jz   r3, POW_END

	//						int mul = 0;
	mvi  r4, #0
	//						for (int j = 0; j < base; j++)
	//						{
	mrr  r5, r0

MUL_LOOP:
	jz   r5, MUL_END

	//							mul += pow;
	add  r4, r4, r2
	dec  r5, r5
	jmp  MUL_LOOP
	//						}

MUL_END:
	//						pow = mul;
	mrr  r2, r4
	//					}
	dec  r3, r3
	jmp  POW_LOOP

POW_END:
	out  r0
	out  r1
	out  r2
