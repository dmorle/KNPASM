	mvi  r0, #1E  // r0 = 31
	mvi  r1, #03  // r1 = 3

	mvi  r2, #FFFF
	mrr  r3, r0

DIV_LOOP:
	jn   r3, DIV_END
	inc  r2, r2
	sub  r3, r3, r1
	jmp  DIV_LOOP

DIV_END:
	out  r0
	out  r1
	out  r2
