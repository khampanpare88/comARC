/* Assembler code fragment */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAXLINELENGTH 1000

int readAndParse(FILE *, char *, char *, char *, char *, char *);
int isNumber(char *);
int toRType(char *, int, char *, char *, char *);
int toOType(int);
int instrcheck(FILE *);
int main(int argc, char *argv[])
{
    char *inFileString, *outFileString;
    FILE *inFilePtr, *outFilePtr;
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
    if (! readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2) ) {
        /* reached end of file */
    }

    /* this is how to rewind the file ptr so that you start reading from the
        beginning of the file */
        rewind(inFilePtr);

    /* after doing a readAndParse, you may want to do the following to test the
        opcode */
    if (!strcmp(opcode, "add")) {
        /* do whatever you need to do for opcode "add" */
        int op = 0b000;
        int wr;
        wr = toRType(label, op, arg0, arg1, arg2);
        fprintf(outFilePtr, "%d", wr);
    }
    if (!strcmp(opcode, "nand")) {
        /* do whatever you need to do for opcode "nand" */
        int op = 0b001;
        int wr = 0;
        wr = toRType(label, op, arg0, arg1, arg2);
        fprintf(outFilePtr, "%d", wr);
    }
    if (!strcmp(opcode, "halt")) {
        int op = 0b110;
        int wr = toOType(op);
        fprintf(outFilePtr, "%d", wr);
    }
    if (!strcmp(opcode, "noop")) {
        int op = 0b111;
        int wr = toOType(op);
        fprintf(outFilePtr, "%d", wr);
    }
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
    int instr = opcode << 22;
    return instr;
}
