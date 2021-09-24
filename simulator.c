/* instruction-level simulator */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define NUMMEMORY 65536 /* maximum number of words in memory */
#define NUMREGS 8 /* number of machine registers */
#define MAXLINELENGTH 1000

typedef struct stateStruct {
    int pc;
    int mem[NUMMEMORY];
    int reg[NUMREGS];
    int numMemory; 
} stateType;

typedef struct instrStruct {
    int opcode;
    int arg0;
    int arg1;
    int arg2;
} instrStruct;

void printState(stateType *);
void initialState(stateType *);
void toBinary(int *, int);
void initialInstr(int *, instrStruct *);

int main(int argc, char *argv[])
{
    char line[MAXLINELENGTH];
    stateType state;
    instrStruct instrCode;
    FILE *filePtr;

    if (argc != 2) {
	printf("error: usage: %s <machine-code file>\n", argv[0]);
	exit(1);
    }

    filePtr = fopen(argv[1], "r");
    if (filePtr == NULL) {
	printf("error: can't open file %s", argv[1]);
	perror("fopen");
	exit(1);
    }

    /* read in the entire machine-code file into memory */
    for (state.numMemory = 0; fgets(line, MAXLINELENGTH, filePtr) != NULL;
	state.numMemory++) {
	if (sscanf(line, "%d", state.mem+state.numMemory) != 1) {
	    printf("error in reading address %d\n", state.numMemory);
	    exit(1);
	}
	printf("memory[%d]=%d\n", state.numMemory, state.mem[state.numMemory]);
    }

    initialState(&state);

    int binary[32];
    toBinary(&binary, state.mem[0]);
    initialInstr(binary, &instrCode);
    printf("========== instruction check ========== \n");
    printf("opcode = %d\n", instrCode.opcode);
    printf("arg0 = %d\n", instrCode.arg0);
    printf("arg1 = %d\n", instrCode.arg1);
    printf("arg2 = %d\n", instrCode.arg2);
    printf("======================================= \n");
    printState(&state);
    return(0);
}

void printState(stateType *statePtr)
{
    int i;
    printf("\n@@@\nstate:\n");
    printf("\tpc %d\n", statePtr->pc);
    printf("\tmemory:\n");
	for (i=0; i<statePtr->numMemory; i++) {
	    printf("\t\tmem[ %d ] %d\n", i, statePtr->mem[i]);
	}
    printf("\tregisters:\n");
	for (i=0; i<NUMREGS; i++) {
	    printf("\t\treg[ %d ] %d\n", i, statePtr->reg[i]);
	}
    printf("end state\n");
}

void initialState(stateType *state){
    state->pc = 0;
    for(int i = 0; i < 8; i++){
        state->reg[i] = 0;
    }
}

void toBinary(int *binary, int assembly){
    for(int i = 0; i < 32; i++){
        binary[i]=(assembly % 2);
        assembly = assembly/2;
    }
}

void initialInstr(int *binary,instrStruct *instr){
    int power = pow(2,15);
    int sum = 0;
    // opcode - 24,23,22
    instr->opcode = (binary[22]*1) + (binary[23]*2) + (binary[24]*4);
    switch(instr->opcode){
        // add instr
        case 0:     instr->arg0 = (binary[19]*1) + (binary[20]*2) + (binary[21]*4);
                    instr->arg1 = (binary[16]*1) + (binary[17]*2) + (binary[18]*4);
                    instr->arg2 = (binary[0]*1) + (binary[1]*2) + (binary[2]*4);
                    break;
        // nand
        case 1:     instr->arg0 = (binary[19]*1) + (binary[20]*2) + (binary[21]*4);
                    instr->arg1 = (binary[16]*1) + (binary[17]*2) + (binary[18]*4);
                    instr->arg2 = (binary[0]*1) + (binary[1]*2) + (binary[2]*4);
                    break;
        // lw
        case 2:     instr->arg0 = (binary[19]*1) + (binary[20]*2) + (binary[21]*4);
                    instr->arg1 = (binary[16]*1) + (binary[17]*2) + (binary[18]*4);
                        for (int i=15; i>=0; i--){
                            if ( i == 15 && binary[i]!= 0){
                                sum = power * -1;
                            }
                            else {
                                sum += (binary[i])*power;//The -0 is needed
                            }
                            power /= 2;
                        }
                    instr->arg2 = sum;
                    break;
        // sw
        case 3: instr->arg0 = (binary[19]*1) + (binary[20]*2) + (binary[21]*4);
                    instr->arg1 = (binary[16]*1) + (binary[17]*2) + (binary[18]*4);
                        for (int i=15; i>=0; i--)
                        {
                            if ( i == 15 && binary[i]!= 0)
                            {
                                sum = power * -1;
                            }
                            else 
                            {
                                sum += (binary[i])*power;//The -0 is needed
                            }
                            power /= 2;
                        }
                    instr->arg2 = sum;
                    break;
        // beq
        case 4: instr->arg0 = (binary[19]*1) + (binary[20]*2) + (binary[21]*4);
                    instr->arg1 = (binary[16]*1) + (binary[17]*2) + (binary[18]*4);
                        for (int i=15; i>=0; i--)
                        {
                            if ( i == 15 && binary[i]!= 0)
                            {
                                sum = power * -1;
                            }
                            else 
                            {
                                sum += (binary[i])*power;//The -0 is needed
                            }
                            power /= 2;
                        }
                    instr->arg2 = sum;
                    break;
        // jalr
        case 5:     instr->arg0 = (binary[19]*1) + (binary[20]*2) + (binary[21]*4);
                    instr->arg1 = (binary[16]*1) + (binary[17]*2) + (binary[18]*4);
                    instr->arg2 = 0;
                    break;
        // halt
        case 6:     instr->arg0 = 0;
                    instr->arg1 = 0;
                    instr->arg2 = 0;
                    break;
        // noop
        case 7:     instr->arg0 = 0;
                    instr->arg1 = 0;
                    instr->arg2 = 0;
                    break;
    }
}
