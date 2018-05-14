#ifndef HW_H
#define HW_H

#include "const.h"
#include "instruction.h"



//Additional helper functions && variables
//Functions:
int equals(char *c1, char *c2);
int flagOrderCheck(char **argv);
int flagParamCheck(char **argv);
int isValidAddress(char *str);
int len(char *str);
int getOpcode(Opcode op);

int getInverseEndianness(int value);

char* getOpcodeString(Opcode instruction);
char* getTypeString(Type type);
char* getSourceString(Source src);


int validInstruction(char*, char*);//1 is valid, 0 if not.



//Returns the value seen from @leftBound:@rightBound in the bits of @value
int getValuefromInstruction(int value, int leftBound, int rightBound);
int binaryAdd(int baseValue, int addingValue, int leftBound, int rightBound);
int length(char*);

//Variables:
int help;
int assem;
int disassem;
int endianB;
int endianL;
char* baseAddr;
int PC;
int base_addr;


#endif

