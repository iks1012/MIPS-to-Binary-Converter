#include <stdlib.h>

#ifdef _STRING_H
#error "Do not #include <string.h>. You will get a ZERO."
#endif

#ifdef _STRINGS_H
#error "Do not #include <strings.h>. You will get a ZERO."
#endif

#ifdef _CTYPE_H
#error "Do not #include <ctype.h>. You will get a ZERO."
#endif

#include "hw1.h"
#include "debug.h"

int main(int argc, char **argv){
    int isValid = 0;
    if(!validargs(argc, argv))
        USAGE(*argv, EXIT_FAILURE);
    else{
        isValid = 1;
    }
    debug("Options: 0x%X", global_options);
    if(global_options & 0x1) {
        USAGE(*argv, EXIT_SUCCESS);
    }

    if(isValid){

        unsigned int PC = (global_options & 0xFFFFF000); 
        base_addr = PC;

        if(disassem){ //Binary to Mips
            int takeInput = 1;
            while(takeInput){

                if (feof(stdin)){
                    takeInput = 0;
                }

                if(takeInput){
                    

                    int value = 0;

                    //get value by getChars 4 times, pass through


                    char firstByte, secondByte, thirdByte, fourthByte;


                    fourthByte = fgetc(stdin);
                    thirdByte = fgetc(stdin);
                    secondByte = fgetc(stdin);
                    firstByte = fgetc(stdin);

                    value = binaryAdd(value, firstByte, 31, 24);
                    value = binaryAdd(value, secondByte, 23, 16);
                    value = binaryAdd(value, thirdByte, 15, 8);
                    value = binaryAdd(value, fourthByte, 7,0);



                    Instruction in; //{value, &info, regs, extra, args};
                    in.value = value;

                    if(!decode(&in, PC)){
                        takeInput = 0;
                        return EXIT_FAILURE;
                    } 
                }

                
                
                PC+=4;
            }
        }
        else{ //Mips to Binary
            int klm = 0;

            char c = '\0';
            int takeInput = 1;
            char* inputString = "";
            char *inPtr = inputString;
            while(c != EOF && takeInput){

                if (feof(stdin)){
                    takeInput = 0;
                    return EXIT_SUCCESS;
                }


                
                int charIndex = 0;
                char instructionChars[120];
                int i = 0;
                while((instructionChars[charIndex] = fgetc(stdin))!='\n'){
                    if(instructionChars[charIndex] == EOF){
                        takeInput = 0;
                        break;
                    }
                    charIndex++;
                }

                //New line removal
                instructionChars[charIndex] = 0;
                int indexOfMatch = -1;
                char* str = instructionChars; 
                Instr_info info;
                for(int i = 0; indexOfMatch==-1 && i < 64; i++){
                    info = instrTable[i];
                    if(validInstruction(str, info.format)){
                        indexOfMatch = i;
                    }
                }



                if(indexOfMatch == -1 && !takeInput){
                    return EXIT_FAILURE;

                }
                else{
                    Instr_info info = instrTable[indexOfMatch];

                    //get the values of args in an array
                    int args[3];
                    args[0] = 0;
                    args[1] = 0;
                    args[2] = 0;
                    int regs[3];
                    regs[0] = 0;
                    regs[1] = 0;
                    regs[2] = 0;
                    int extra = 0;
                    int value = 0;




                    char* instruct = str;
                    int numParams = validInstruction(instruct, info.format);

                    if(numParams == 1){
                        sscanf(instruct, info.format, &args[0]);
                    }
                    else if(numParams == 2){
                        sscanf(instruct, info.format, &args[0], &args[1]);
                    }
                    else if(numParams == 3){
                        sscanf(instruct, info.format, &args[0], &args[1], &args[2]);
                    }
                    else{

                    }
                    
                    //Args is ready
                    for(int i = 0; i < 3; i ++){
                        if(equals("RS", getSourceString(info.srcs[i]))){
                            regs[0] = args[i];
                        }
                        if(equals("RT", getSourceString(info.srcs[i]))){
                            regs[1] = args[i];
                        }
                        if(equals("RD", getSourceString(info.srcs[i]))){
                            regs[2] = args[i];
                        }
                        if(equals("EXTRA", getSourceString(info.srcs[i]))){
                            extra = args[i];
                        }
                    }


                    //fix the extra value

                    char* instructionString = getOpcodeString(info.opcode);
                    char* instrTypeString = getTypeString(info.type);

                    


                    if(equals("OP_BREAK", instructionString)){
                        //Do nothing to extra.
                    }
                    else if(equals(instrTypeString, "RTYP")){
                        //Do nothing to extra
                    }
                    else if(equals(instrTypeString, "ITYP")){

                        //If extra is negative, extend sign

                        if(extra < 0){
                            extra *= -1;
                        }

                        if(equals(instructionString, "OP_BEQ") || 
                            equals(instructionString, "OP_BGEZ") || 
                            equals(instructionString, "OP_BGEZAL") || 
                            equals(instructionString, "OP_BGTZ") || 
                            equals(instructionString, "OP_BLEZ") || 
                            equals(instructionString, "OP_BLTZAL") || 
                            equals(instructionString, "OP_BLTZ") || 
                            equals(instructionString, "OP_BNE")){

                            extra -= 4;
                            extra -= PC;
                            extra /= 4;
                        }
                    }
                    else if(equals(instrTypeString, "JTYP")){
                        extra -= ((base_addr) & 0xF0000000);
                        extra /= 4;

                    }


                    


                    Instruction in;
                    in.value = value;
                    in.info = &info;
                    in.regs[0] = regs[0];
                    in.regs[1] = regs[1];
                    in.regs[2] = regs[2];
                    in.extra = extra;
                    in.args[0] = args[0];
                    in.args[1] = args[1];
                    in.args[2] = args[2];



                    if(!encode(&in, PC)){
                        return EXIT_FAILURE;
                    }

                    
                    PC+=4;
                }
                
            }
        }
    }
    return EXIT_SUCCESS;
}
            

