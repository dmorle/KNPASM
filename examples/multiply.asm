// step for 35 clock cycles
	mvi  r0, #5
	mvi  r1, #6
	mvi  r6, #7FFF
	mvi  r7, #7FFE
	strm r0, r6
	strm r1, r7
	mvi  r2, #0
LOOP:
	add  r2, r2, r0
	dec  r1, r1
	jz   r1, END
	jmp  LOOP
END:
	ldmr r0, r6
	ldmr r1, r7
	out  r0
	out  r1
	out  r2
