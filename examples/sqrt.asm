	mvi r0, #31  // 49
	
	mvi r1, #FFFF
	mrr r2, r0
	mvi r7, #2

GUESS_LOOP:
	jn  r2, GUESS_END
	inc r1, r1
	sub r2, r2, r7
	jmp GUESS_LOOP

GUESS_END:
	mvi r2, #FF

SQRT_LOOP:
	jz  r2, SQRT_END

	// r3 = xn
	mrr r3, r1
	
	// calculating xn/2
	mvi r4, #FFFF
	mrr r5, r3

XN_2_LOOP:
	jn  r5, XN_2_END
	inc r4, r4
	sub r5, r5, r7
	jmp XN_2_LOOP

XN_2_END:
	sub r1, r1, r4

	// r3 = 2xn
	add r3, r3, r3

	// calculating a/r3
	mvi r4, #FFFF
	mrr r5, r0

A_2XN_LOOP:
	jn  r5, A_2XN_END
	inc r4, r4
	sub r5, r5, r3
	jmp A_2XN_LOOP

A_2XN_END:
	add r1, r1, r4

	dec r2, r2
	jmp SQRT_LOOP

SQRT_END:
	out r0
	out r1

END:
	jmp END
