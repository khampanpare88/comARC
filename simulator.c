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
void lw(instrStruct,stateType *);
void sw(instrStruct,stateType *);
void beq(instrStruct,stateType *);
void add(instrStruct,stateType *);
void nand(instrStruct,stateType *);
void jalr(instrStruct,stateType *);
void halt(instrStruct,stateType *,int,int *);

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

    instrStruct instrArr[state.numMemory];

    int binary[32];
    int round =0;
    int haltFlag = 0;

    while(state.pc < state.numMemory && haltFlag == 0){
        toBinary(binary, state.mem[state.pc]);
        initialInstr(binary, &instrCode);
        instrArr[state.pc] = instrCode;
        round++;

        switch (instrArr[state.pc].opcode)
        {
        case 0:
            //add
            add(instrArr[state.pc],&state);
            break;
        case 1:
            //nand
            nand(instrArr[state.pc],&state);
            break;
        case 2:
            //lw
            lw(instrArr[state.pc],&state);
            
            break;
        case 3:
            //sw
            sw(instrArr[state.pc],&state);
            
            break;
        case 4:
            //beq
            beq(instrArr[state.pc],&state);
            break;
        case 5:
            //jalr
            jalr(instrArr[state.pc],&state);
            break;
        case 6:
            //halt
            halt(instrArr[state.pc],&state, round, &haltFlag);
            break;
        case 7:
            //noop
            state.pc++;
            break;
        
        default:
            exit(1);
            break;
        }

    }
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
    printState(state);
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

void lw(instrStruct instr,stateType *state){
    int memAddr;
    memAddr = instr.arg2 + state->reg[instr.arg0];
    state->reg[instr.arg1] = state->mem[memAddr];
    state->pc++;
    printState(state);
}

void sw(instrStruct instr,stateType *state){
    int memAddr;
    memAddr = instr.arg2 + state->reg[instr.arg1];
    state->reg[instr.arg0] = state->mem[memAddr];
    state->pc++;
    printState(state);
}

void beq(instrStruct instr,stateType *state){
    int address;
    address = state->pc + 1 + instr.arg2;
    if(state->reg[instr.arg0] == state->reg[instr.arg1]){
        state->pc = address;
        printState(state);
    }else{
        state->pc++;
        printState(state);
    }
}

void add(instrStruct instr,stateType *state){
    int sum;
    sum = state->reg[instr.arg0] + state->reg[instr.arg1];
    state->reg[instr.arg2] = sum;
    state->pc++;
    printState(state);
}

void nand(instrStruct instr,stateType *state){
    int nand;
    int arg0T = state->reg[instr.arg0];
    int arg1T = state->reg[instr.arg1];
    nand = !((arg0T%2)*(arg1T%2))*1 + !(((arg0T/2)%2)*((arg1T/2)%2))*2 + !(((arg0T/4)%2)*((arg1T/4)%2))*4;
    state->reg[instr.arg2] = nand;
    state->pc++;
    printState(state);
}

void jalr(instrStruct instr,stateType *state){
    int address;
    int arg0T = state->reg[instr.arg0]; //rA
    int arg1T = state->reg[instr.arg1]; //rB
    address = state->pc+1;
    state->reg[instr.arg1] = address; //save pc+1 --> rB
    if(state->reg[instr.arg1] != state->reg[instr.arg0]){
        state->pc = state->reg[instr.arg1]; //jump rA(add)
    }
    else {
        // jump pc+1
        state->pc = address;
    }
    state->pc++;
    printState(state);
}

void halt(instrStruct instr,stateType *state, int round, int *flag){
    state->pc++;
    printf("machine halted \n");
    printf("total of ");
    printf("%d",round);
    printf(" instructions executed \n");
    printf("final state of machine:");
    printState(state);
    *flag = 1;
}