#include "spimcore.h"
/*This code has been hobbled together by the one and only Tyler Glorie*/

/* ALU */
/* 10 Points */
void ALU(unsigned A,unsigned B,char ALUControl,unsigned *ALUresult,char *Zero)
{
	switch (ALUControl) {
		// ADD
		case 0:
			*ALUresult = A+B;
			break;

		// SUB
		case 1:
			*ALUresult = A-B;
			break;

		// SLT signed
		case 2:
			if ((signed)A < (signed)B)
				*ALUresult = 1;
			else
				*ALUresult = 0;
			break;

		// SLT unsigned
		case 3:
			if (A < B)
				*ALUresult = 1;
			else
				*ALUresult = 0;
			break;

		// AND
		case 4:
			*ALUresult = (A & B);
			break;

		// OR
		case 5:
			*ALUresult = (A | B);
			break;

		// Left Shift *16
		case 6:
            *ALUresult = B << 16;
            break;

		// NOT
		case 7:
			*ALUresult = ~A;
			break;
	}

	// set the zero bit if needed
	if (*ALUresult == 0) {
		*Zero = 1;
	} else {
		*Zero = 0;
	}
}

/* instruction fetch */
/* 10 Points */
int instruction_fetch(unsigned PC,unsigned *Mem,unsigned *instruction)
{
    
	//check if PC is BYTE ADDRESSED
    if(PC % 4 != 0)
        return 1;
	/*retrieve instruction with PC from Memory and copy to instruction, shifting by 2 to get instruction because memory is BYTE ADDRESSED as recently revealed.*/
	*instruction = Mem[(PC >> 2)];
	return 0;
}


/* instruction partition */
/* 10 Points */
void instruction_partition(unsigned instruction, unsigned *op, unsigned *r1,unsigned *r2, unsigned *r3, unsigned *funct, unsigned *offset, unsigned *jsec)
{
    //parse the instruction to the datastruct.

    //set opcode to upper six bits
    *op = (instruction >> 26) & 0b00000000000000000000000000111111;

    //set register 1 to next 5 bits, anded to only copy rightmost 5 bits
    *r1 = (instruction >> 21) & 0b11111;

    //set register 2 to next 5 bits, anded to only copy rightmost 5 bits
    *r2 = (instruction >> 16) & 0b11111;;

    //set register 3 to next 5 bits, anded to only copy rightmost 5 bits
    *r3 = (instruction >> 11) & 0b11111;

    //copy first 6 bits for the function code
    *funct = instruction & 0b00000000000000000000000000111111;

    //copy the first 16 bits for offset
    *offset = instruction & 0b00000000000000001111111111111111;

    //copy the first 26 bits for jump destination
    *jsec = instruction & 0b00000011111111111111111111111111;
}



/* instruction decode */
/* 15 Points */
int instruction_decode(unsigned op,struct_controls *controls)
{
	//halt if invalid opcode
	int halt = 0;

	switch (op) {
        //r-type: ALUop = 0111
		case 0:
			controls->RegDst = 1;
			controls->Jump = 0;
			controls->Branch = 0;
			controls->MemRead = 0;
			controls->MemtoReg = 0;
			controls->ALUOp = 7;
			controls->MemWrite = 0;
			controls->ALUSrc = 0;
			controls->RegWrite = 1;
            break;
            
        //beq
        case 4:
            controls->RegDst = 2;
            controls->RegWrite = 0;
            controls->ALUSrc = 0;
            controls->MemRead = 0;
            controls->MemWrite = 0;
            controls->MemtoReg = 2;
            controls->Jump = 0;
            controls->Branch = 1;
            controls->ALUOp = 1;
            break;
        //addi
        case 8:
            controls->RegDst = 0;
            controls->RegWrite = 1;
            controls->ALUSrc = 1;
            controls->MemRead = 0;
            controls->MemWrite = 0;
            controls->MemtoReg = 0;
            controls->Jump = 0;
            controls->Branch = 0;
            controls->ALUOp = 0;
            break;
            
        //slt
        case 10:
            controls->RegDst = 0;
            controls->RegWrite = 1;
            controls->ALUSrc = 1;
            controls->MemRead = 0;
            controls->MemWrite = 0;
            controls->MemtoReg = 0;
            controls->Jump = 0;
            controls->Branch = 0;
            controls->ALUOp = 2;
            break;
            
        //sltu
        case 11:
            controls->RegDst = 0;
            controls->RegWrite = 1;
            controls->ALUSrc = 1 ;
            controls->MemRead = 0;
            controls->MemWrite = 0;
            controls->MemtoReg = 0;
            controls->Jump = 0;
            controls->Branch = 0;
            controls->ALUOp = 3;
            break;
            
        //lw
        case 35:
            controls->RegDst = 0;
            controls->RegWrite = 1;
            controls->ALUSrc = 1;
            controls->MemRead = 1;
            controls->MemWrite = 0;
            controls->MemtoReg = 1;
            controls->Jump = 0;
            controls->Branch = 0;
            controls->ALUOp = 0;
            break;
            
        //lui
        case 15:
            controls->RegDst = 0;
            controls->RegWrite = 1;
            controls->ALUSrc = 1;
            controls->MemRead = 0;
            controls->MemWrite = 0;
            controls->MemtoReg = 0;
            controls->Jump = 0;
            controls->Branch = 0;
            controls->ALUOp = 6;
            break;
            
        //sw
        case 43:
            controls->RegDst = 2; // 2 for don't care
            controls->RegWrite = 0;
            controls->ALUSrc = 1;
            controls->MemRead = 0;
            controls->MemWrite = 1;
            controls->MemtoReg = 2;
            controls->Jump = 0;
            controls->Branch = 0;
            controls->ALUOp = 0;
            break;
            
        //jump
        case 2:
            controls->RegDst = 0;
            controls->RegWrite = 0;
            controls->ALUSrc = 0;
            controls->MemRead = 0;
            controls->MemWrite = 0;
            controls->MemtoReg = 0;
            controls->Jump = 1;
            controls->Branch = 0;
            controls->ALUOp = 0;
            break;
		default:
			// if not defined, halt
			halt = 1;
	}
	return halt;
}


/* Read Register */
/* 5 Points */
void read_register(unsigned r1,unsigned r2,unsigned *Reg,unsigned *data1,unsigned *data2)
{
	//copy data from registers to data1 and data2
    *data1 = Reg[r1];
    *data2 = Reg[r2];
}


/* Sign Extend */
/* 10 Points */
void sign_extend(unsigned offset,unsigned *extended_value)
{
    unsigned negative = offset >> 15;
    //if negative, extend 1s
    if(negative == 1)
        *extended_value = offset | 0xFFFF0000;
    //non-negative extend 0s
    else
        *extended_value = offset & 0x0000FFFF;
}

/* ALU operations */
/* 10 Points */
int ALU_operations(unsigned data1,unsigned data2,unsigned extended_value,unsigned funct,char ALUOp,char ALUSrc,unsigned *ALUresult,char *Zero)
{
    //if ALUSrc asserted, set second ALU input to sign extended; I-type
    if (ALUSrc == 1) {
        data2 = extended_value;
    }
    // check if R-type instruction
    if (ALUOp == 7) {
        // set the correct ALUOp for each R-type instruction
        switch(funct) {
            //add
            case 32:
                ALUOp = 0;
                break;
            //sub
            case 34:
                ALUOp = 1;
                break;
            //slt signed
            case 42:
                ALUOp = 2;
                break;
            //slt unsigned
            case 43:
                ALUOp = 3;
                break;
            //and
            case 36:
                ALUOp = 4;
                break;
            //or
            case 37:
                ALUOp = 5;
                break;
            //shift left *16
            case 4:
                ALUOp = 6;
                break;
            //nor
            case 39:
                ALUOp = 7;
                break;
            //default to halt
            default:
                return 1;
        }
    }
    //pass ALU the values
    ALU(data1, data2, ALUOp, ALUresult, Zero);
    
    return 0;
}

/* Read / Write Memory */
/* 10 Points */
int rw_memory(unsigned ALUresult,unsigned data2,char MemWrite,char MemRead,unsigned *memdata,unsigned *Mem)
{
	//check if address is BYTE ADDRESSED
    if ((MemWrite == 1 || MemRead == 1) && ALUresult % 4 != 0)
	{
		//halt
		return 1;
	}
	//if memory needs to be read, set the memdata to memory of BYTE ADDRESSED shifted ALUresult
	if (MemRead == 1)
	{
		*memdata = Mem[ALUresult >> 2];
	}
    //if MemWrite is 1 copy data2 to calculated address
	if (MemWrite == 1)
	{
		Mem[ALUresult >> 2] = data2;
	}

	return 0;
}


/* Write Register */
/* 10 Points */
void write_register(unsigned r2,unsigned r3,unsigned memdata,unsigned ALUresult,char RegWrite,char RegDst,char MemtoReg,unsigned *Reg)
{
    if (RegWrite == 1 ) {
        if (MemtoReg == 1) {
            if (RegDst == 1)
            	//copy memdata to rd
                Reg[r3] = memdata;
            else
            	//copy memdata to rt
                Reg[r2] = memdata;
        }
        else {
            if (RegDst == 1)
            	//copy ALUresult to rd
                Reg [r3] = ALUresult;
            else
            	//copy ALUresult to rt
                Reg [r2] = ALUresult;
        }
    }
}

/* PC update */
/* 10 Points */
void PC_update(unsigned jsec,unsigned extended_value,char Branch,char Jump,char Zero,unsigned *PC)
{
    //increment PC by 4 every cycle
    *PC = *PC  + 4;
    //for jump set PC to shifted jump destination ORed with upper 4 bits of PC
    if(Jump == 1)
    {
    	//IT WORKS (this part is the magic transformation of program counter jump dests
		*PC = (jsec << 2) | (*PC & 0xf0000000);
    }
    //for a branch replace PC with shifted destination because BYTE ADDRESSED
    else if (Branch == 1 && Zero)
    {
		*PC += (extended_value << 2);
    }
}

