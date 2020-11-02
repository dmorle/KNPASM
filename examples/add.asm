// step for 6 clock cycles
// MVI <=> Move immediate (literal) to register
	mvi r0, #0001   // this is a comment
	mvi r1, #0002
	add r2, r0, r1
	out r0
	out r1
	out r2
