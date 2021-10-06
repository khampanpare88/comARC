#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#define MAXLINELENGTH 1000

int readAndParse(FILE *, char *, char *, char *, char *, char *);
int findaddress( char *,char *);
int isNumber(char *);
int toRType(char *, int, char *, char *, char *);
int toIType(char *, int, char *, char *, char *, char *, int);
int toOType(int);
int toJType(char *, int, char *, char *, int);
void opCodeCheck(char *);
void labelCheck(char *, char *);

int main(int argc, char *argv[])
{
    char *inFileString, *outFileString,*checkinFileString;
    FILE *inFilePtr, *outFilePtr,*checkinFile;
    char label[MAXLINELENGTH], opcode[MAXLINELENGTH], arg0[MAXLINELENGTH],
            arg1[MAXLINELENGTH], arg2[MAXLINELENGTH];

    if (argc != 3) {
        printf("error: usage: %s <assembly-code-file> <machine-code-file>\n",
            argv[0]);
        exit(1);
    }

    inFileString = argv[1]; 
    outFileString = argv[2]; 

    inFilePtr = fopen(inFileString, "r");
    if (inFilePtr == NULL) {
        printf("error in opening %s\n", inFileString);
        exit(1);
    }
    outFilePtr = fopen(outFileString, "w");
    if (outFilePtr == NULL) {
        printf("error in opening %s\n", outFileString);
        exit(1);
    }


    /* here is an example for how to use readAndParse to read a line from
        inFilePtr */
    int lineNumber = 0;
    while (readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2)) {
        if(strlen(label)!=0){
            labelCheck(argv[1], label);
        }
        // checking the opcode
        opCodeCheck(opcode);
        if (!strcmp(opcode, "jalr")) {
         /*do whatever you need to do for opcode " jalr" */ 
        int op = 0b101;                                              //set opcode = 101
        int wr = 0;                                     
        int arg2 = 0b0000000000000000;                //set 15-0 bit = 0
        wr = toJType(label, op, arg0, arg1, arg2);   
       //call toJType function to create machine code of jalr instruction
        fprintf(outFilePtr, "%d", wr); 
      //wirte jalr instruction to output file

        }
        if (!strcmp(opcode, ".fill")) {
            if(isNumber(arg0)){ 			 //check arg0 is a number or not
                int wr = atoi(arg0); 
                fprintf(outFilePtr, "%d", wr);
            }else{
                const char *target=arg0;
                int wr=findaddress(argv[1],arg0); 	 //call function findaddress to create instruction
                fprintf(outFilePtr, "%d", wr);
            }
        }
        if (!strcmp(opcode, "add")) {
        /* do whatever you need to do for opcode "add" */\
	// let opcode = 000
            int op = 0b000;
            int wr = 0;
            // wr is the result of add instruction
            wr = toRType(label, op, arg0, arg1, arg2);
	// write the instruction into the output file
            fprintf(outFilePtr, "%d", wr);
        }
        if (!strcmp(opcode, "nand")) {
        /* do whatever you need to do for opcode "nand" */
	// let opcode = 001
            int op = 0b001;
            int wr = 0;
	// wr is the result of add instruction
            wr = toRType(label, op, arg0, arg1, arg2);
	// write the instruction into the output file
            fprintf(outFilePtr, "%d", wr);
        }
        if (!strcmp(opcode, "halt")) {
            /* do whatever you need to do for opcode "halt" */
            int op = 0b110;		// set opcode = 111
            int wr = toOType(op);		// call function OType to create instruction
            fprintf(outFilePtr, "%d", wr);	//write the instruction into output file
        }
        if (!strcmp(opcode, "noop")) {
            /* do whatever you need to do for opcode "noop" */
            int op = 0b111;		// set opcode = 111
            int wr = toOType(op);		// call function OType to create instruction
            fprintf(outFilePtr, "%d", wr);	//write the instruction  into output file
        }
        if (!strcmp(opcode, "lw")) {
        /* do whatever you need to do for opcode "lw" */
        int op = 0b010;
        int wr = 0;
        wr = toIType(label, op, arg0, arg1, arg2, argv[1], lineNumber);
        fprintf(outFilePtr, "%d", wr);
        }

        if (!strcmp(opcode, "sw")) {
            /* do whatever you need to do for opcode "sw" */
            int op = 0b011;
            int wr = 0;
            wr = toIType(label, op, arg0, arg1, arg2, argv[1], lineNumber);
            fprintf(outFilePtr, "%d", wr);
        }

        if (!strcmp(opcode, "beq")) {
            /* do whatever you need to do for opcode "deq" */
            int op = 0b100;
            int wr = 0;
            wr = toIType(label, op, arg0, arg1, arg2, argv[1], lineNumber+1);
            fprintf(outFilePtr, "%d", wr);
        }
        fprintf(outFilePtr,"%s","\n");
        lineNumber++;
    }

    /* this is how to rewind the file ptr so that you start reading from the
        beginning of the file */
        rewind(inFilePtr);

    /* after doing a readAndParse, you may want to do the following to test the
        opcode */

    return(0);
}

/*
 * Read and parse a line of the assembly-language file.  Fields are returned
 * in label, opcode, arg0, arg1, arg2 (these strings must have memory already
 * allocated to them).
 *
 * Return values:
 *     0 if reached end of file
 *     1 if all went well
 *
 * exit(1) if line is too long.
 */
int readAndParse(FILE *inFilePtr, char *label, char *opcode, char *arg0,
    char *arg1, char *arg2)
{
    char line[MAXLINELENGTH];
    char *ptr = line;

    /* delete prior values */
    label[0] = opcode[0] = arg0[0] = arg1[0] = arg2[0] = '\0';

    /* read the line from the assembly-language file */
    if (fgets(line, MAXLINELENGTH, inFilePtr) == NULL) {
	/* reached end of file */
        return(0);
    }
    /* check for line too long (by looking for a \n) */
    if (strchr(line, '\n') == NULL) {
        /* line too long */
	printf("error: line too long\n");
	exit(1);
    }

    /* is there a label? */
    ptr = line;
    if (sscanf(ptr, "%[^\t\n ]", label)) {
	/* successfully read label; advance pointer over the label */
        ptr += strlen(label);
    }

    /*
     * Parse the rest of the line.  Would be nice to have real regular
     * expressions, but scanf will suffice.
     */
    sscanf(ptr, "%*[\t\n ]%[^\t\n ]%*[\t\n ]%[^\t\n ]%*[\t\n ]%[^\t\n ]%*[\t\n ]%[^\t\n ]",
        opcode, arg0, arg1, arg2);

    return(1);
}

int isNumber(char *string)
{
    /* return 1 if string is a number */
    int i;
    return( (sscanf(string, "%d", &i)) == 1);
}

// convert the assembly to Rtype instruction
int toRType(char *label, int opcode, char *arg0,
    char *arg1, char *arg2){
        int instr = 0b0;
        // convert arg to binary
        int arg0B = atoi (arg0);
        int arg1B = atoi (arg1);
        int arg2B = atoi (arg2);
        // place arguments into instruction
        instr = ((((((((instr << 3) + opcode) << 3) + arg0B) << 3) + arg1B) << 16) + arg2B); 
    return instr;
}

int toOType(int opcode){
    // place arguments into instruction
    int instr = opcode << 22;	// shift left opcode to bit 24 ,23 ,22
    return instr;			    // return instruction
}

int findaddress(char *arg, char *target){
        int address=0;
        char label[MAXLINELENGTH], opcode[MAXLINELENGTH], arg0[MAXLINELENGTH],
            arg1[MAXLINELENGTH], arg2[MAXLINELENGTH];
        FILE *FilePtr;
        char *FileString = arg; 
        int count;
        FilePtr = fopen(FileString, "r");
        while (readAndParse(FilePtr, label, opcode, arg0, arg1, arg2)){
            if(!strcmp (target,label)){
                return address;
            }
            address++;
        }
        // if the label is not match any label in the input file
        // print ‘undefined label’ and send error exit(1)
        printf("error: undefined label\n");
        exit(1);
        return 0;
}

// check for duplicated label
void labelCheck(char *arg, char *tempCheck){
    char labelT[MAXLINELENGTH], opcodeT[MAXLINELENGTH], arg0T[MAXLINELENGTH],
            arg1T[MAXLINELENGTH], arg2T[MAXLINELENGTH];
    FILE *FilePtr;
    char *FileString = arg; 
    int flag = 0;
    FilePtr = fopen(FileString, "r");
        // read the input file
        while (readAndParse(FilePtr, labelT, opcodeT, arg0T, arg1T, arg2T)){
	    // if there is any duplicated label 
            if(!strcmp (tempCheck,labelT)){
	    // increasing flag
                flag++;
            }
        } 
        // if flag is more than 1
        if(flag > 1){
	    // print ‘duplicated label’ and send error exit(1)
            printf("error: duplicated label\n");
            exit(1);
        }
}

// check for undefined opcode
void opCodeCheck(char *opcodeCheck){ 
    // if the opcode is not equal to the following below
    if( strcmp(opcodeCheck, "add") &&
        strcmp(opcodeCheck, "nand") &&
        strcmp(opcodeCheck, "lw") &&
        strcmp(opcodeCheck, "sw") &&
        strcmp(opcodeCheck, "beq") &&
        strcmp(opcodeCheck, "jalr") &&
        strcmp(opcodeCheck, "halt") &&
        strcmp(opcodeCheck, "noop") &&
        strcmp(opcodeCheck, ".fill")){
	// print ‘undefined opcode’ and send error exit(1)
            printf("error: undefined opcode\n");
            exit(1);
    }
}
//convert assembly to  J-Type Instruction
int toJType(char *label, int opcode, char *arg0,
    char *arg1, int arg2){
        int instr = 0b0;
        // convert arg to binary
        int arg0B = atoi (arg0);
        int arg1B = atoi (arg1);
        // place arguments into instruction
        instr = ((((((((instr << 3) + opcode) << 3) + arg0B) << 3) + arg1B) << 16) + arg2); 
    return instr;      //return instruction
}

int toIType(char *label, int opcode, char *arg0,
    char *arg1, char *arg2, char *argv, int lineNumber){
        int instr = 0b0;
        int offset;
        // convert arg to binary
        int arg0B = atoi (arg0);
        int arg1B = atoi (arg1);
        // 2'complement
        int arg2B;
        int labelv;
        if(isNumber(arg2) == 1){
            arg2B = atoi (arg2);
        }
        else{
            offset = findaddress(argv,arg2);
            if (opcode == 0b100)
            {
                arg2B = offset - lineNumber;
            }
            else{
                arg2B = offset;
            }
        }
        // check offsetField
        if(arg2B > -32768 && arg2B < 32767){
                if(arg2B < 0){
                    int temp = fabs(arg2B);
                    int binary[16];
                    //แปลงเป็นฐาน2แล้วกลับบิต
                    for(int i=0; i<16; i++){
                        binary[i]=(temp % 2);
                        temp = temp/2;
                        if(binary[i] == 1){
                            binary[i] = 0;
                        }
                        else{
                            binary[i] = 1;
                        }
                    }
                    int carry = 1;                        
                    int carryin = 0;
                    //+1 ที่บิตสุดท้าย
                    if(binary[0] == 1 && carry == 1){
                        binary[0] = 0;
                        carryin = 1;
                        for(int m=1;m<16;m++){
                            if(binary[m] == 1 && carryin == 1){
                                binary[m] = 0;
                                carryin = 1;
                            }
                            else if (binary[m] == 0 && carryin == 1){
                                binary[m] = 1;
                                carryin = 0;
                            }
                            else{
                                binary[m] = binary[m];
                            }
                        }
                    }
                    else if(binary[0]==0 && carry == 1){
                        binary[0] = 1;
                        carryin = 0;
                        for(int m=1;m<16;m++){
                            if(binary[m]==1 && carryin == 1){
                                binary[m] = 0;
                                carryin = 1;
                            }
                            else if (binary[m] == 0 && carryin == 1){
                                binary[m] = 1;
                                carryin = 0;
                            }
                            else{
                                binary[m] = binary[m];
                            }
                        }
                    }
                    //ทำเป็นฐาน10
                    arg2B = 0;
                    for(int n=0;n<16;n++){
                        binary[n] = binary[n]*pow(2,n);
                        arg2B = arg2B + binary[n];
                    }
                }
                else{
                    arg2B = arg2B;
                }
            }   
            else{
                printf("error: offsetField\n");
                exit(1);
            }
        // place arguments into instruction
        instr = ((((((((instr << 3) + opcode) << 3) + arg0B) << 3) + arg1B) << 16) + arg2B);
    return instr;
}
