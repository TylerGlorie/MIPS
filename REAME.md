#MIPS
This program is designed to simulate the process of a MIPS processor cycle-by-cycle

---------------------------------------------------------------

Intructions for use:

First compile:
 
$ gcc -o spimcore spimcore.c project.c
 
After compilation, to use MySPIM, you would type the following command in UNIX:
 
$ ./spimcore <filename>.asc

----------------------------------------------------------------

2.5. Format of the input machine code file

MySPIM is designed to take hexadecimal that's been formatted into machine codes, with filename xxx.asc, as the input. An example .asc file is below. anything after “#” on each line is treated as a comment.

20010000 	# addi $1, $0, 0
200200c8 	# addi $2, $0, 200
10220003 	# beq $1, $2, 3
00000020 	# delay slot
20210001 	# addi $1, $1, 1
00000020 	# no operation

The simulation will end once an illegal instruction, such asthe instruction  0x00000000, is processed.

----------------------------------------------------------------

Commands for the program:
r
Dump registers contents

m
Dump memory contents (in Hexadecimal format)

s[n]
Step n instructions (simulate the next n instruction). If n is not typed, 1 is assumed

c
Continue (carry on the simulation until the program halts (with illegal instruction))

H
Check if the program has halted

d
ads1 ads2 Hexadecimal dump from address ads1 to ads2

I
Inquire memory size

P
Print the input file

g
Display all control signals

 