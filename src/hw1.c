#include "hw1.h"


#ifdef _STRING_H
#error "Do not #include <string.h>. You will get a ZERO."
#endif

#ifdef _STRINGS_H
#error "Do not #include <strings.h>. You will get a ZERO."
#endif

#ifdef _CTYPE_H
#error "Do not #include <ctype.h>. You will get a ZERO."
#endif

/*
 * You may modify this file and/or move the functions contained here
 * to other source files (except for main.c) as you wish.
 */

//Functions used by the main function ->


int validInstruction(char *str1, char *str2){
    int arr[3];
    arr[0] = 0;
    arr[1] = 0;
    arr[2] = 0;
    int numParams = 0;
    for(char *ch = str2; *ch != '\0'; ch++){
        if(*ch == '%'){
            numParams++;
        }
    }
    switch(numParams){
        case 0 : return equals(str1, str2);
        case 1 : return sscanf(str1, str2, &arr[0]);
        case 2 : return sscanf(str1, str2, &arr[0], &arr[1]);
        case 3 : return sscanf(str1, str2, &arr[0], &arr[1], &arr[2]);
    }

    return 0;
}

/**
 * @brief Validates command line arguments passed to the program.
 * @details This function will validate all the arguments passed to the
 * program, returning 1 if validation succeeds and 0 if validation fails.
 * Upon successful return, the selected program options will be set in the
 * global variable "global_options", where they will be accessible
 * elsewhere in the program.
 *
 * @param argc The number of arguments passed to the program from the CLI.
 * @param argv The argument strings passed to the program from the CLI.
 * @return 1 if validation succeeds and 0 if validation fails.
 * Refer to the homework document for the effects of this function on
 * global variables.
 * @modifies global variable "global_options" to contain a bitmap representing
 * the selected options.
 */
int validargs(int argc, char **argv) {
    //Order = (help) (a or d) (base address) (endianness)
    //If this order is violated, return 0
    global_options = 0x00;
    help = 0;
    assem = 0;
    disassem = 0;
    endianB = 0;
    endianL = 0;




    int isValid = flagOrderCheck(argv) && flagParamCheck(argv);
    if(argc > 7){
        isValid = 0;
    }

    if(argc <= 1){
        isValid = 0;
    }

    PC = global_options;


    if(help){
        global_options = global_options | 0x01;
    }

    if(disassem){
        global_options = global_options | 0x02;
    }

    if(endianB){
        global_options = global_options | 0x04;
    }

    if(!isValid){
        global_options = 0;
    }
    return isValid;
}

int flagOrderCheck(char **argv){
    int isValid = 1;
    char** ptr = argv;

    int helpFlag = 0;
    int ADFlag = 0;
    int baseFlag = 0;
    int endianFlag = 0;
    int checkConfig = 0;
    int checkForBase = 0;
    int checkForEndian = 0;
    int checkIfFlagValue = 0;
    int argCounter = 0;
    help = 0;
    //flag orderChecker
    for (char* charRef = *ptr; charRef;  charRef = *++ptr) {
        int needsHelp = argCounter && equals(charRef, "-h");
        if(argCounter>0){ //!=0 -> so if it isnt "bin/hw1"
            checkConfig = ((!helpFlag && argCounter) || (helpFlag && (argCounter == 2))) && (equals(charRef, "-a") || equals(charRef, "-d"));
            checkForBase = (ADFlag && !baseFlag) && (equals(charRef, "-b"));
            checkForEndian = (ADFlag && !endianFlag) && (equals(charRef, "-e"));
            checkIfFlagValue = (baseFlag && isValidAddress(charRef)) || ((equals(charRef, "l") || equals(charRef, "b")));
            if(needsHelp){
                //Valid
                helpFlag = 1;
                help = 1;
            }
            else if(checkConfig){
                // assemble/disassemble flag checker
                ADFlag = 1;
                if(equals(charRef, "-a")){
                    assem = 1;
                    disassem = 0;
                }
                else{
                    assem = 0;
                    disassem = 1;
                }
            }
            else if(ADFlag && checkForBase){
                //base address flag checker
                baseFlag = 1;
            }
            else if((ADFlag || baseFlag) && checkForEndian){
                endianFlag = 1;
            }
            else if (checkIfFlagValue){
                //Base Address and Endianness
            }
            else {
                isValid = 0;
            }
        }
        argCounter++;
    }

    return isValid;
}

int len(char *str){
    int length = 0;
    for(char *temp = str; *temp; temp++){
        length++;
    }
    return length;
}

int flagParamCheck(char **argv){
    int isValid = 1;
    char** ptr = argv;
    for (char* charRef = *ptr; charRef && isValid;  charRef = *++ptr) {
        if(isValid && equals(charRef, "-b")){
            //check if the next arg is of the regex
            charRef = *++ptr;
            isValid = isValidAddress(charRef);
            //check if div by 4096 or 0001 0000 0000 0000 or 0x1000
            //check if less then 8 digits

            baseAddr = charRef;
        }

        if(isValid && equals(charRef, "-e")){
            charRef = *++ptr;
            isValid = (equals(charRef, "l") || equals(charRef, "b"));
            //check if the next arg is an L or B (lowercase)
            if(equals(charRef, "l")){
                endianL = 1;
                endianB = 0;
            }
            else{
                endianL = 0;
                endianB = 1;
            }
        }
    }

    return isValid;
}


int isValidAddress(char *str){
    int numZeros = 0;
    int length = 0;
    int isValid = 1;
    char* temp;
    for(temp = str; *temp && isValid; temp++){
        isValid = ((('a' <= *temp) && (*temp <= 'f')) || (('A' <= *temp) && (*temp <= 'F')) || (('0' <= *temp) && (*temp <= '9')));
        length++;
    }


    int address_in_hex = 0x00;
    sscanf(str,"%x", &address_in_hex);
    int div_test = address_in_hex & 0x00000FFF;

    int isRightLength = (length <= 8);

    if((isValid && !div_test) && isRightLength){
        global_options = address_in_hex;
    }
    else{
        global_options = 0;
    }

    return ((isValid && !div_test)&&isRightLength);
}

/**
 *
 * Working and Tested!!
 * Checks if two char pointers have equal chars
 * returns 0 if not equal and 1 if equal
 *
 */
int equals(char *c1, char *c2){
    int answer = 0;
    int counter = 0;
    while(answer == 0 && (c1[counter] == c2[counter])){
        if(c1[counter] == '\0' && c2[counter] == '\0'){
            //This means that the strings have reached the end
            //while being equal and they null terminate at the same time;
            answer = 1;
        }
        counter++;
    }
    if(answer == -1){
        answer = 0;
    }
    return answer;
}


/**
 * @brief Computes the binary code for a MIPS machine instruction.
 * @details This function takes a pointer to an Instruction structure
 * that contains information defining a MIPS machine instruction and
 * computes the binary code for that instruction.  The code is returned
 * in the "value" field of the Instruction structure.
 *
 * @param ip The Instruction structure containing information about the
 * instruction, except for the "value" field.
 * @param addr Address at which the instruction is to appear in memory.
 * The address is used to compute the PC-relative offsets used in branch
 * instructions.
 * @return 1 if the instruction was successfully encoded, 0 otherwise.
 * @modifies the "value" field of the Instruction structure to contain the
 * binary code for the instruction.
 */
int encode(Instruction *ip, unsigned int addr) {
    //Assemble
    //Mips to Binary
    int value = 0;
    Instr_info info = *((*ip).info);
    char* opcodeString = getOpcodeString(info.opcode);
    //printf("%s\n", opcodeString);
    char* typeString = getTypeString(info.type);
    int special = -1;

    int opcodeVal = -1; 
    int extra = (*ip).extra;;
    int rs = (*ip).regs[0];
    int rt = (*ip).regs[1];
    int rd = (*ip).regs[2];


    //checks both the special and regular because there is no way of knowing that
    //OP_ADD for example is a special opcode.
    for(int i = 0; i < 64; i++){
        if(equals(opcodeString, getOpcodeString(specialTable[i]))){
            special = i;
            opcodeVal = 0;
        }
    }

    for(int i = 0; i < 64; i++){
        if(equals(opcodeString, getOpcodeString(opcodeTable[i]))){
            opcodeVal = i;
            special = 0;
        }
    }


    


    if(equals(typeString, "RTYP")){
        if(equals(opcodeString, "OP_BREAK")){
            value = binaryAdd(value, opcodeVal, 31, 26);
            value = binaryAdd(value, extra, 25, 6);
            value = binaryAdd(value, special, 5, 0);
            //value = opcode + extra + special
        }
        else{

            value = binaryAdd(value, opcodeVal, 31, 26);
            value = binaryAdd(value, rs, 25, 21);
            value = binaryAdd(value, rt, 20, 16);
            value = binaryAdd(value, rd, 15, 11);
            value = binaryAdd(value, extra, 10, 6);
            value = binaryAdd(value, special, 5, 0);

            //value = opcode+rs+rt+rd+extra+special
        }

    }
    else if(equals(typeString, "ITYP")){
        if(equals(opcodeString, "OP_BGEZ")){
            rt = 0x01; 
            opcodeVal = 1;
        }
        if(equals(opcodeString, "OP_BGEZAL")){
            rt = 0x11;
            opcodeVal = 1;
        }
        if(equals(opcodeString, "OP_BLTZ")){
            rt = 0x00;
            opcodeVal = 1;
        }
        if(equals(opcodeString, "OP_BLTZAL")){
            rt = 0x10;
            opcodeVal = 1;
        }
        value = binaryAdd(value, opcodeVal, 31,26);
        value = binaryAdd(value, rs, 25, 21);
        value = binaryAdd(value, rt, 20, 16);


        value = binaryAdd(value, extra, 15, 0);
    }
    else if(equals(typeString, "JTYP")){
        value = binaryAdd(value, opcodeVal, 31 , 26);

        //printf("%x\n", extra);
        value = binaryAdd(value, extra, 25, 0);
    }
    else if(equals(typeString, "NTYP")){
        //Find out what to do

    }
    else{
        return 0;
    }


    //Convert calculated value to Big endian if need be...
    if(endianB){
        //convert value to BigEndian
        value = getInverseEndianness(value);
    }

    ((*ip).value) = value;
    
    
    unsigned char firstByte, secondByte, thirdByte, fourthByte;
    firstByte = getValuefromInstruction(value, 31, 24);
    secondByte = getValuefromInstruction(value, 23, 16);
    thirdByte = getValuefromInstruction(value, 15, 8);
    fourthByte = getValuefromInstruction(value, 7, 0);

    char word[4] = {fourthByte, thirdByte, secondByte, firstByte};

    for(int i = 0; i < 4; i++){
        fputc(word[i],stdout);
    }

    return 1;
}

int getInverseEndianness(int value){
    unsigned int firstByte, secondByte, thirdByte, fourthByte;

    //(int value, int leftBound, int rightBound)
    firstByte = getValuefromInstruction(value, 31, 24);
    secondByte = getValuefromInstruction(value, 23, 16);
    thirdByte = getValuefromInstruction(value, 15, 8);
    fourthByte = getValuefromInstruction(value, 7, 0);

    value = 0;
    value = binaryAdd(value, firstByte, 7, 0);
    value = binaryAdd(value, secondByte, 15, 8);
    value = binaryAdd(value, thirdByte, 23, 16);
    value = binaryAdd(value, fourthByte, 31, 24);
    return value;
}

int length(char* str){
    int len = 0;
    for(len = 0; *str; str++, len++){}
    return len;
}

int binaryAdd(int baseValue, int addingValue, int leftBound, int rightBound){
    int andValue = 0;
    for(int i = leftBound-rightBound; i >= 0; i--){
        andValue += (1 << i);
    }
    addingValue = addingValue & andValue;
    addingValue = addingValue << rightBound;
    return baseValue | addingValue;
}

/**
 * @brief Decodes the binary code for a MIPS machine instruction.
 * @details This function takes a pointer to an Instruction structure
 * whose "value" field has been initialized to the binary code for
 * MIPS machine instruction and it decodes the instruction to obtain
 * details about the type of instruction and its arguments.
 * The decoded information is returned by setting the other fields
 * of the Instruction structure.
 *
 * @param ip The Instruction structure containing the binary code for
 * a MIPS instruction in its "value" field.
 * @param addr Address at which the instruction appears in memory.
 * The address is used to compute absolute branch addresses from the
 * the PC-relative offsets that occur in the instruction.
 * @return 1 if the instruction was successfully decoded, 0 otherwise.
 * @modifies the fields other than the "value" field to contain the
 * decoded information about the instruction.
 */
int decode(Instruction *ip, unsigned int addr) {
    //Binary to Mips
    //Disassemble

    //For this the instruction must have the Value, that is all that is needed

    //int originalExtraValue = 0;

    int value = (*ip).value;

    if(endianB){
        //Convert value to little endian if need be...
        value = getInverseEndianness(value);
    }

    int opCode = getValuefromInstruction(value, 31, 26);

    Opcode instruction;
    char* instructionString; 

    int opcodeFound = 0;

    if(opCode == 0){
        //Look in the specialTable
        int specialValue = getValuefromInstruction(value, 5, 0);
        instruction = specialTable[specialValue];
        opcodeFound = 0;
    }
    else{
        instruction = opcodeTable[opCode];
    }
    instructionString = getOpcodeString(instruction);

    if(equals(instructionString, "BCOND")){
        int bcondVal = getValuefromInstruction(value, 20, 16);

        if(bcondVal == 0){
            instruction = OP_BLTZ;
        }
        else if(bcondVal == 1){ // 00001
            instruction = OP_BGEZ;
        }
        else if(bcondVal == 0x10){ //10000
            instruction = OP_BLTZAL;
        }
        else if(bcondVal == 0x11){//10001
            instruction = OP_BGEZAL;
        }
        else{
            //Error;
            return 0;
        }
        instructionString = getOpcodeString(instruction);
    }
    if(equals(instructionString, "ILLEGL")){
        return 0;
    }

    for(int i = 0; !opcodeFound && (i < 64); i++){
        char* tempOpcode = getOpcodeString(instrTable[i].opcode);
        if(equals(tempOpcode, instructionString)){
            opcodeFound = i;
        }
    }
    //This should be non 0 for all valid instruction inputs;
    if(opcodeFound){
        //get the RS, RT AND RD and Extra values
        int rs = getValuefromInstruction(value, 25, 21);
        int rt = getValuefromInstruction(value, 20, 16);
        int rd =  getValuefromInstruction(value, 15, 11);

        Instr_info info = instrTable[opcodeFound];
        char* instrTypeString = getTypeString(info.type);

        int extra = 0;
        if(equals("OP_BREAK", instructionString)){
            extra = getValuefromInstruction(value, 25,6);
            //originalExtraValue = extra;
        }
        else if(equals(instrTypeString, "RTYP")){
            extra = getValuefromInstruction(value, 10, 6);
            //originalExtraValue = extra;
        }
        else if(equals(instrTypeString, "ITYP")){
            extra = getValuefromInstruction(value, 15, 0);
            //originalExtraValue = extra;
            //Sign Extension, unsure if it works from the getValuefromInstruction,
            //But this makes sure.
            if((extra & 0x00008000) > 0){
                extra = extra | 0xFFFF0000;
            }
            if(equals(instructionString, "OP_BEQ") || 
                equals(instructionString, "OP_BGEZ") || 
                equals(instructionString, "OP_BGEZAL") || 
                equals(instructionString, "OP_BGTZ") || 
                equals(instructionString, "OP_BLEZ") || 
                equals(instructionString, "OP_BLTZAL") || 
                equals(instructionString, "OP_BLTZ") || 
                equals(instructionString, "OP_BNE")){
                extra *= 4; //PC += 4;
                extra += addr;
                extra += 4;
            }

        }
        else if(equals(instrTypeString, "JTYP")){
            extra = getValuefromInstruction(value, 25, 0);
            //originalExtraValue = extra;

            extra *= 4;
            extra += (addr  & 0xF0000000);
            //extra += 244;
        }

        int var[3];
        var[0] = 0;
        var[1] = 0;
        var[2] = 0;

        int numParams = 3;
        int numNullParams = 0;
        for(int i = 0; i < numParams; i ++){
            if(equals("RS", getSourceString(info.srcs[i]))){
                var[i] = rs;
            }
            else if(equals("RT", getSourceString(info.srcs[i]))){
                var[i] = rt;
            }
            else if(equals("RD", getSourceString(info.srcs[i]))){
                var[i] = rd;
            }
            else if(equals("EXTRA", getSourceString(info.srcs[i]))){
                var[i] = extra;
            }
            else{
                numNullParams++;
            }

        }
        numParams = numParams - numNullParams;

        
        
        if(numParams == 1){
            printf(info.format, var[0]);
        }
        else if(numParams == 2){
            printf(info.format, var[0], var[1]);
        }
        else if(numParams == 3){
            printf(info.format, var[0], var[1], var[2]);
        }
        else if(numParams == 0){
            printf("%s", info.format);
        }
        else{
            return 0;
        }





        (*ip).args[0] = var[0];
        (*ip).args[1] = var[1];
        (*ip).args[2] = var[2];
        (*ip).extra = extra;
        (*ip).regs[0] = rs;
        (*ip).regs[1] = rt;
        (*ip).regs[2] = rd;

    
        ((*ip).info) = &info;
    


        printf("\n");

    }
    else{
        //If the opcode is not found, it must be invalid
        return 0;
    }


    return 1;
}

//Leftbound must be greater then rightBound
int getValuefromInstruction(int value, int leftBound, int rightBound){
    int andValue = 0;
    for(int i = leftBound; i >= rightBound; i--){
        andValue += (1 << i);
    }
    unsigned int val = (value & andValue);
    return val>>rightBound;
}

char* getSourceString(Source src){
    switch(src){
        case NSRC : return "NSRC";
        case RS : return "RS";
        case RT : return "RT";
        case RD : return "RD";
        case EXTRA : return "EXTRA";
    }

    return "Invalid Source";
}

char* getTypeString(Type type){
    switch(type){
        case NTYP : return "NTYP";
        case ITYP : return "ITYP";
        case JTYP : return "JTYP";
        case RTYP : return "RTYP";
    }
    return "Invalid Type";
}


char* getOpcodeString(Opcode instruction){
    switch(instruction){
        case OP_0 : return "OP_0";
        case OP_ADD : return "OP_ADD";
        case OP_ADDI : return "OP_ADDI";
        case OP_ADDIU : return "OP_ADDIU";
        case OP_ADDU : return "OP_ADDU";
        case OP_AND : return "OP_AND";
        case OP_ANDI : return "OP_ANDI";
        case OP_BEQ : return "OP_BEQ";
        case OP_BGEZ : return "OP_BGEZ";
        case OP_BGEZAL : return "OP_BGEZAL";
        case OP_BGTZ : return "OP_BGTZ";
        case OP_BLEZ : return "OP_BLEZ";
        case OP_BLTZ : return "OP_BLTZ";
        case OP_BLTZAL : return "OP_BLTZAL";
        case OP_BNE : return "OP_BNE";
        case OP_15 : return "OP_15";
        case OP_DIV : return "OP_DIV";
        case OP_DIVU : return "OP_DIVU";
        case OP_J : return "OP_J";
        case OP_JAL : return "OP_JAL";
        case OP_JALR : return "OP_JALR";
        case OP_JR : return "OP_JR";
        case OP_LB : return "OP_LB";
        case OP_LBU : return "OP_LBU";
        case OP_LH : return "OP_LH";
        case OP_LHU : return "OP_LHU";
        case OP_LUI : return "OP_LUI";
        case OP_LW : return "OP_LW";
        case OP_LWL : return "OP_LWL";
        case OP_LWR : return "OP_LWR";
        case OP_30 : return "OP_30";
        case OP_MFHI : return "OP_MFHI";
        case OP_MFLO : return "OP_MFLO";
        case OP_33 : return "OP_33";
        case OP_MTHI : return "OP_MTHI";
        case OP_MTLO : return "OP_MTLO";
        case OP_MULT : return "OP_MULT";
        case OP_MULTU : return "OP_MULTU";
        case OP_NOR : return "OP_NOR";
        case OP_OR : return "OP_OR";
        case OP_ORI : return "OP_ORI";
        case OP_RFE : return "OP_RFE";
        case OP_SB : return "OP_SB";
        case OP_SH : return "OP_SH";
        case OP_SLL : return "OP_SLL";
        case OP_SLLV : return "OP_SLLV";
        case OP_SLT : return "OP_SLT";
        case OP_SLTI : return "OP_SLTI";
        case OP_SLTIU : return "OP_SLTIU";
        case OP_SLTU : return "OP_SLTU";
        case OP_SRA : return "OP_SRA";
        case OP_SRAV : return "OP_SRAV";
        case OP_SRL : return "OP_SRL";
        case OP_SRLV : return "OP_SRLV";
        case OP_SUB : return "OP_SUB";
        case OP_SUBU : return "OP_SUBU";
        case OP_SW : return "OP_SW";
        case OP_SWL : return "OP_SWL";
        case OP_SWR : return "OP_SWR";
        case OP_XOR : return "OP_XOR";
        case OP_XORI : return "OP_XORI";
        case OP_SYSCALL : return "OP_SYSCALL";
        case OP_BREAK : return "OP_BREAK";
        case OP_UNIMP : return "OP_UNIMP";
        case SPECIAL : return "SPECIAL";
        case BCOND : return "BCOND";
        case ILLEGL : return "ILLEGL";
    }

    return "INVALID OP CODE";
}











