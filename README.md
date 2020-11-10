# KNPASM - Khalid's Nano Processor

This repository contains a simulation tool for the theoretical Khalid's Nano Processor which was used as a learning tool in DLD 2.

To simulate the processor for a given KNP assembly file, pass the file as a command line argument to the program.

```
knpasm <asm-file> [{-i|--step <number-of-steps>}]
```

### Instruction Set

| Instruction Description      | Mnemonic | Format           | RTL Operation                                            |
|------------------------------|----------|------------------|----------------------------------------------------------|
| Move register to register    | `MRR `	  | `Rd, Rs`      	 | Rd &leftarrow; Rs                                        |
| Move immediate to register   | `MVI `	  | `Rd, #CV`		 | Rd &leftarrow; #CV		                                |
| Load from memory to register | `LDMR`	  | `Rd, Ra	`		 | Rd &leftarrow; M[Ra]		                                |
| Store register to memory     | `STRM`	  | `Ra, Rs`		 | M[Ra] &leftarrow; Rs		                                |
| Add two registers            | `ADD `	  | `Rd, Rs1, Rs2`	 | Rd &leftarrow; Rs1 + Rs2	                                |
| Subtract two registers	   | `SUB `	  | `Rd, Rs1, Rs2`	 | Rd &leftarrow; Rs1 - Rs2	                                |
| Increment register		   | `INC `	  | `Rd, Rs`		 | Rd &leftarrow; Rs + 1	                                |
| Decrement register		   | `DEC `	  | `Rd, Rs`		 | Rd &leftarrow; Rs - 1	                                |
| Logical AND				   | `AND `	  | `Rd, Rs1, Rs2`	 | Rd &leftarrow; Rs1 \& Rs2                                |
| Logical OR				   | `OR  `	  | `Rd, Rs1, Rs2`	 | Rd &leftarrow; Rs1 \| Rs2                                |
| Logical XOR				   | `XOR `	  | `Rd, Rs1, Rs2`	 | Rd &leftarrow; Rs1 ^ Rs2	                                |
| Logical NOT				   | `NOT `	  | `Rd, Rs`		 | Rd &leftarrow; ~Rs		                                |
| Uncondition Jump			   | `JMP `	  | `AO`			 | PC &leftarrow; AO                                        |
| Jump on zero				   | `JZ  `	  | `Rs, AO`		 | Rs == 0 ? PC &leftarrow; PC + AO : PC &leftarrow; PC + 1 |
| Jump on negative			   | `JN  `	  | `Rs, AO`		 | Rs < 0 ? PC &leftarrow; PC + AO : PC &leftarrow; PC + 1  |

 - R* refers to a register *
 - #CV refers to a hex value with a preceeding \#
 - M refers to program memory
 - PC refers to the program counter
 - AO refers to a label

### Interactive run

An assembly file can be run in interactive mode using the `-i` option. 
Once interactive mode has been started, you can use any of the commands shown in the table below.

| Command Description            | Command 
|--------------------------------|---------
| stops program execution		 | `break`
| steps through instructions	 | `step [{ \<num-of-steps\> \| \<conditional-expression\> }]`
| retrieves register value(s)	 | `reg [\<register\>]`
| retrieves a value from memory	 | `mem #CV`
| prints the current instruction | `ins`
| sets an internal cpu value     | `set { mem #CV #CV \| Rd #CV }`
