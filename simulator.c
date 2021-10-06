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
    long long int reg[NUMREGS];
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
int binaryLen(int);


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

    initialState(&state);		// call initial state to start the simulator

    instrStruct instrArr[state.numMemory+1];		// create array for instruction

    int binary[32];
    int round = 0;
    int haltFlag = 0; 
    // while pc is not reach the last line of the machine code and pc is not point to halt instruction
    while(state.pc < state.numMemory+1 && haltFlag == 0){
        // convert machine code to binary
        toBinary(binary, state.mem[state.pc]);		
        // initial the instruction 
        initialInstr(binary, &instrCode);
        // put the instruction into an array
        instrArr[state.pc+1] = instrCode;
        // count  the state
        round++;
        // check for the opcode
        switch (instrArr[state.pc+1].opcode)
        {
        case 0:
            //add operation
            add(instrArr[state.pc+1],&state);
            break;
        case 1:
            //nand operation
            nand(instrArr[state.pc+1],&state);
            break;
        case 2:
            //lw operation
            lw(instrArr[state.pc+1],&state);
            break;
        case 3:
            //sw operation
            sw(instrArr[state.pc+1],&state);
            break;
        case 4:
            //beq operation
            beq(instrArr[state.pc+1],&state);
            break;
        case 5:
            //jalr operation
            jalr(instrArr[state.pc+1],&state);
            break;
        case 6:
            //halt operation
            halt(instrArr[state.pc+1],&state, round, &haltFlag);
            break;
        case 7:
            //noop operation
            state.pc++;
            break;
        
        default:
            // if the opcode is not match any case
	// send error exit(1)
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

// initial the first state
void initialState(stateType *state){
    state->pc = 0;		// set pc = 0
    for(int i = 0; i < 8; i++){	// set all reg = 0
        state->reg[i] = 0;		
    }
    printState(state);
}

// convert decimal to binary
void toBinary(int *binary, int assembly){
    for(int i = 0; i < 32; i++){
        binary[i]=(assembly % 2);
        assembly = assembly/2;
    }
}

// get the binary length 
int binaryLen(int dec){
    int count = 0;
    if(dec == 0){
        return 1;
    }else{
        while(dec!=0){
            count++;
            dec = dec/2;
        }
        return count;
    }
}

// initial the instruction 
void initialInstr(int *binary,instrStruct *instr){
    int power = pow(2,15); 
    int sum = 0; 
    // the opcode is placed at index 24-22 of the instruction
    instr->opcode = (binary[22]*1) + (binary[23]*2) + (binary[24]*4);
   // decompose the arguments by the opcode
    switch(instr->opcode){ 
        // add operation
        case 0:     instr->arg0 = (binary[19]*1) + (binary[20]*2) + (binary[21]*4); 
		// arg0 is placed at index 21-19
                    instr->arg1 = (binary[16]*1) + (binary[17]*2) + (binary[18]*4);
		// arg0 is placed at index 18-16
                    instr->arg2 = (binary[0]*1) + (binary[1]*2) + (binary[2]*4);
		// arg0 is placed at index 2-1
                    break;
        // nand operation
        case 1:     instr->arg0 = (binary[19]*1) + (binary[20]*2) + (binary[21]*4);
		// arg0 is placed at index 21-19
                    instr->arg1 = (binary[16]*1) + (binary[17]*2) + (binary[18]*4);
		// arg1 is placed at index 18-16
                    instr->arg2 = (binary[0]*1) + (binary[1]*2) + (binary[2]*4);
		// arg2 is placed at index 2-1
                    break;
        // lw operation
        case 2:     instr->arg0 = (binary[19]*1) + (binary[20]*2) + (binary[21]*4);
		// arg0 is placed at index 21-19
                    instr->arg1 = (binary[16]*1) + (binary[17]*2) + (binary[18]*4);
		// arg1 is placed at index 18-16
                        for (int i=15; i>=0; i--){ // convert index 15-0 to decimal 
                            if ( i == 15 && binary[i]!= 0){
                                sum = power * -1;
                            }
                            else {
                                sum += (binary[i])*power;//The -0 is needed
                            }
                            power /= 2;
                        }
                    instr->arg2 = sum;
		//arg2 is placed at index 15-0
                    break;
        // sw operation
        case 3: instr->arg0 = (binary[19]*1) + (binary[20]*2) + (binary[21]*4);
		// arg0 is placed at index 21-19
                    instr->arg1 = (binary[16]*1) + (binary[17]*2) + (binary[18]*4);
		// arg1 is placed at index 18-16
                        for (int i=15; i>=0; i--){ // convert index 15-0 to decimal 
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
		//arg2 is placed at index 15-0
                    break;
        // beq operation
        case 4: instr->arg0 = (binary[19]*1) + (binary[20]*2) + (binary[21]*4);
		 // arg0 is placed at index 21-19
                    instr->arg1 = (binary[16]*1) + (binary[17]*2) + (binary[18]*4);
		// arg1 is placed at index 18-16
                        for (int i=15; i>=0; i--){ // convert index 15-0 to decimal 
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
		//arg2 is placed at index 15-0
                    break;
        // jalr operation
        case 5:     instr->arg0 = (binary[19]*1) + (binary[20]*2) + (binary[21]*4);
		//arg0 is placed at index 21-19
                    instr->arg1 = (binary[16]*1) + (binary[17]*2) + (binary[18]*4);
		//arg1 is placed at index 18-16
                    instr->arg2 = 0;
		//arg2 is always be 0
                    break;
        // halt operation
        case 6:     instr->arg0 = 0;
                    instr->arg1 = 0;
                    instr->arg2 = 0;
		// all arguments are 0
                    break;
        // noop operation
        case 7:     instr->arg0 = 0;
                    instr->arg1 = 0;
                    instr->arg2 = 0;
		// all arguments are 0
                    break;
    }
}

void lw(instrStruct instr,stateType *state){
    int memAddr;
    memAddr = instr.arg2 + state->reg[instr.arg0];		// address = offsetField + reg[A]
    state->reg[instr.arg1] = state->mem[memAddr];		// reg[B] = mem[address]
    state->pc++;						// increase pc
    printState(state);						// call print state
}

void sw(instrStruct instr,stateType *state){
    int memAddr;
    memAddr = instr.arg2 + state->reg[instr.arg0];		// address = offsetField + reg[A]
    state->numMemory=memAddr;				// change numMemory
    state->mem[memAddr] = state->reg[instr.arg1];		// mem[address] = reg[B]
    state->pc++;						// increase pc
    printState(state);						// call print state
}

void beq(instrStruct instr,stateType *state){
    int address;
    address = state->pc + 1 + instr.arg2;                                   //set address = pc+1+offsetField
    if(state->reg[instr.arg0] == state->reg[instr.arg1]){                   //if reg[A]==reg[B] 
        state->pc = address;                                                //set pc = address
        printState(state);                                                  //call printstate function     
    }else{ 
        state->pc++;                                                        //set pc+1
        printState(state);                                                  //call printstate function
    }
}

void add(instrStruct instr,stateType *state){
    long long int sum;
    sum = state->reg[instr.arg0] + state->reg[instr.arg1]; 	// sum = reg[A] + reg[B]
    state->reg[instr.arg2] = sum;				// destReg = sum
    state->pc++;						        // set pc+1
    printState(state);						    // call printState function
}

void nand(instrStruct instr,stateType *state){
    long long int nand = 0;
    int arg0T = state->reg[instr.arg0];				// arg0T = reg[A]
    int arg1T = state->reg[instr.arg1];				// argqT = reg[B]
    int factor = 1;						
    int length;
    if(arg0T > arg1T){						    // if reg[A] > reg[B]
        length = binaryLen(arg0T);				// call binaryLen using reg[A] 
    }else{
        length = binaryLen(arg1T);				// call binaryLen using reg[B] 
    }
    for(int i=0; i<length; i++){	            // do nand operation
        nand = nand + !(((arg0T/factor)%2)*((arg1T/factor)%2))*factor;
        factor = factor*2;
    }
    state->reg[instr.arg2] = nand;				// destReg = nand
    state->pc++;						        // set pc+1
    printState(state);						    // call printState function
}

void jalr(instrStruct instr,stateType *state){
    int address;
    int arg0T = state->reg[instr.arg0]; //rA
    int arg1T = state->reg[instr.arg1]; //rB
    address = state->pc+1;
    state->reg[instr.arg1] = address; //save pc+1 --> rB
    state->pc++;
    printState(state);
    if(instr.arg1 != instr.arg0){
        state->pc = state->reg[instr.arg0]; //jump rA(add)
    }
    else {
        // jump pc+1
        state->pc = address;
    }
}

void halt(instrStruct instr,stateType *state, int round, int *flag){
    state->pc++;
    printf("machine halted \n");
    printf("total of ");
    printf("%d",round);
    printf(" instructions executed \n");
    printf("final state of machine:");
    printState(state);
    *flag = 1;	// set haltFlag = 1
}
