//Single header file assembler for Impostor2 CPU Architecture

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <inttypes.h>
#include <stdbool.h>

#define NOP 0x0
#define ADD 0x1
#define SUB 0x2
#define SHL 0x3
#define SHR 0x4
#define XOR 0x5
#define BOR 0x6
#define AND 0x7
#define CMP 0x8
#define MOV 0x9
#define LDW 0xa
#define STW 0xb
#define LIW 0xc
#define SIW 0xd
#define PSH 0xe
#define POP 0xf
#define JMP 0x10
#define JIE 0x11
#define JNE 0x12
#define JIC 0x13
#define JNC 0x14
#define JIN 0x15
#define JNN 0x16
#define CMI 0x17
#define JAB 0x18
#define JMS 0x19
#define RET 0x1a
#define ADI 0x1b
#define SBI 0x1c
#define SLI 0x1d
#define SRI 0x1e
#define XRI 0x1f
#define ORI 0x20
#define ANI 0x21
#define MVI 0x22

#define N_OPCODES 0x23
#define N_REGISTERS 8

#define UNSIGNED_INT_16_LIMIT 65535

struct Label {

    uint16_t address;
    char* str;

};

char opStrings[N_OPCODES][4] = {"nop", "add", "sub", "shl", "shr", "xor", "bor", "and", "cmp", "mov", "ldw", "stw", "liw", "siw", "psh", "pop", "jmp", "jie", "jne", "jic", "jnc", "jin", "jnn", "cmi", "jab", "jms", "ret", "adi", "sbi", "sli", "sri", "xri", "ori", "ani", "mvi"};

size_t opNRegArgs[N_OPCODES] = {0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1};

size_t opNValArgs[N_OPCODES] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1};

char regStrings[N_REGISTERS][3] = {"r0", "r1", "r2", "r3", "sp", "bp", "pb", "db"};


const size_t LABELS_SIZE_INC = 100;
const size_t LABEL_LENGTH_INC = 100;
const size_t LINES_SIZE_INC = 100;


//Command line Arguments for the assembler
bool writeAllBytes = false;


void* SafeMalloc(size_t size) {
    void* ptr = malloc(size);
    if (ptr == NULL && size != 0) {
        perror("Malloc failed");
        exit(EXIT_FAILURE);
    }
    return ptr;
}

void* SafeCalloc(size_t elemSize, size_t nElem) {

    void* ptr = calloc(elemSize, nElem);
    if (ptr == NULL && elemSize*nElem != 0) {
        perror("Calloc failed");
        exit(EXIT_FAILURE);
    }
    return ptr;

}

void* SafeRealloc(void* oldPtr, size_t newSize) {

    void* ptr = realloc(oldPtr, newSize);
    if (ptr == NULL && newSize != 0) {
        perror("Realloc failed");
        exit(EXIT_FAILURE);
    }
    return ptr;
}


long long int Max(long long int x, long long int y) {

    if (x > y) return x;
    else return y;

}

long long int Min(long long int x, long long int y) {

    if (x < y) return x;
    else return y;

}



//Second pass. Replaces all the labels with their values, and converts opcode mnemonics to machine code
void Assemble_AssembleFile(FILE* outFile, char* str, struct Label* labels, size_t nLabels, uint16_t* lineAddresses) {

    size_t outStrLength = (UNSIGNED_INT_16_LIMIT+1) * 3;
    char* outStr = SafeMalloc(outStrLength * sizeof(char));

    for (size_t i = 0; i < outStrLength; i++) {
        if ((i+1)%3==0) outStr[i] = '\n';
        else outStr[i] = '0';
    }
    outStr[outStrLength-1] = '\0';

    size_t lineNumber = 1;

    uint16_t currentAddress = lineAddresses[lineNumber-1];
    uint16_t largestWrittenToAddress = 0x0000;
    uint16_t smallestWrittenToAddress = 0xffff;

    size_t strI = 0;
    while (str[strI] != '\0') {

        if (isspace(str[strI])) {
            //Skip past white space
            if (str[strI] == '\n') {
                ++lineNumber;
                currentAddress = lineAddresses[lineNumber-1];
            }
            ++strI;
        }
        else if (str[strI] == ';' || str[strI] == '#') {
            //Skip past comments and special instructions
            while (str[strI] != '\n' && str[strI] != '\0') ++strI;
        }
        else if (isdigit(str[strI])) {
            //Convert the string to a number
            char* endPtr;
            uint16_t num = strtoul(&str[strI], &endPtr, 0);
            
            //Skip to the white space after the number
            strI = endPtr - str;

            //Print the number to outStr
            {
                char tempStr[6];    //A temporary string to prevent a null terminator in outStr
                snprintf(tempStr, 3, "%02"PRIx8, (uint8_t)num & 0xff); //Print the low byte
                tempStr[2] = '\n';
                snprintf(tempStr+3, 3, "%02"PRIx8, (uint8_t)(num >> 8) & 0xff);  //Print the high byte
                tempStr[5] = '\0';

                //Then just copy over the temp string to the correct place in outStr
                strncpy(outStr+currentAddress*3, tempStr, 5);

                //Update largestWrittenToAddress                        //+1 is used because the high byte is also written to
                largestWrittenToAddress = Max(largestWrittenToAddress, currentAddress+1);

                //Update smallestWrittenToAddress
                smallestWrittenToAddress = Min(smallestWrittenToAddress, currentAddress);
            }

            currentAddress += 2;    //Increment the current address by 2 bytes            
        }
        else if (str[strI] == '\'') {
            //Convert the character in the single quotes to a number
            uint8_t num = str[strI+1];

            //Print the number to outStr
            {
                char tempStr[6];    //A temporary string to prevent a null terminator in outStr
                snprintf(tempStr, 3, "%02"PRIx8, num); //Print the low byte
                tempStr[2] = '\n';
                snprintf(tempStr+3, 3, "00");  //Print the high byte
                tempStr[5] = '\0';

                //Then just copy over the temp string to the correct place in outStr
                strncpy(outStr+currentAddress*3, tempStr, 5);

                //Update largestWrittenToAddress                        //+1 is used because the high byte is also written to
                largestWrittenToAddress = Max(largestWrittenToAddress, currentAddress+1);

                //Update smallestWrittenToAddress
                smallestWrittenToAddress = Min(smallestWrittenToAddress, currentAddress);
            }

            currentAddress += 2;    //Increment the current address by 2 bytes
            strI += 3;  //Skip past the character inside the quotes, and the closing single quote
        }
        else if (isalpha(str[strI])) {

            size_t oldLineNumber = lineNumber;
            size_t oldAddress = currentAddress;

            //Is it an opcode, if so, write the machine code equivalent into outStr, and increment the current address accordingly

            bool isOpCode = false;
            char opCodeStr[4];
            for (size_t i = 0; i < 3; i++) {
                opCodeStr[i] = str[strI+i];
                if (str[strI+i] == '\0') goto DoneAssembling;
                if (str[strI+i] == '\n') {
                    ++lineNumber;
                    currentAddress = lineAddresses[lineNumber-1];
                }
            }
            opCodeStr[3] = '\0';

            size_t opcodeVal; //If it is an opcode, then this holds the value of that opcode
            for (opcodeVal = 0; opcodeVal < N_OPCODES; opcodeVal++) {
                if (!strcmp(opCodeStr, opStrings[opcodeVal])) {
                    isOpCode = true;
                    break;
                }
            }

            if (isOpCode) {

                uint16_t finalOpcodeVal = (uint16_t)opcodeVal;                

                //First, get the register arguments if there are any
                
                for (size_t regArgCount = 0; regArgCount < opNRegArgs[opcodeVal]; regArgCount++) {
                    //First, skip the current word
                    while (isalnum(str[strI])) ++strI;
                    
                    //Then skip the white space after the word
                    bool encounteredNewLine = false;
                    while (isspace(str[strI])) {
                        if (str[strI] == '\n') {
                            ++lineNumber;
                            currentAddress = lineAddresses[lineNumber-1];
                            encounteredNewLine = true;
                        }
                        ++strI;
                    }

                    //Now get the word

                    if (str[strI] == '\0' || str[strI+1] == '\0') {  //Make sure there is space for a register string
                        fprintf(stderr, "Error: Missing register argument after opcode. Opcode \"%s\" needs %zu register arguments. Line %zu\n", opStrings[opcodeVal], opNRegArgs[opcodeVal], oldLineNumber);
                        exit(EXIT_FAILURE);
                    }

                    char regStr[3];
                    regStr[0] = str[strI]; regStr[1] = str[strI+1]; regStr[2] = '\0';

                    //Now check if the string is actually a register, and if it is, which register it is
                    size_t reg = SIZE_MAX;
                    for (size_t i = 0; i < N_REGISTERS; i++) {
                        if (!strcmp(regStr, regStrings[i])) {
                            reg = i;
                            break;
                        }
                    }

                    if (reg == SIZE_MAX) {
                        //If the next word wasn't a register, print an error and exit
                        fprintf(stderr, "Error: Invalid register name \"");
                        size_t wordStrI = 0;
                        while (!isspace(str[strI+wordStrI])) {
                            fprintf(stderr, "%c", str[strI+wordStrI]);
                            ++wordStrI;
                        }
                        fprintf(stderr, "\". Opcode \"%s\" needs %zu register arguments. Line %zu\n", opStrings[opcodeVal], opNRegArgs[opcodeVal], oldLineNumber);
                        exit(EXIT_FAILURE);
                    }

                    if (encounteredNewLine) {
                        //Cannot be a new line between opcode and register arguments
                        fprintf(stderr, "Error: Opcode and register arguments must be on same line. Register Argument #%zu. Line %zu\n", regArgCount+1, oldLineNumber);
                        exit(EXIT_FAILURE);
                    }

                    //Now modify the value to be written to outStr based on the register
                    finalOpcodeVal |= reg << (6 + regArgCount*3);
                }

                //Print out the final value of the opcode to outStr at the correct location
                {
                    char tempStr[6];    //A temporary string to prevent a null terminator in outStr
                    snprintf(tempStr, 3, "%02"PRIx8, (uint8_t)finalOpcodeVal & 0xff); //Print the low byte
                    tempStr[2] = '\n';
                    snprintf(tempStr+3, 3, "%02"PRIx8, (uint8_t)(finalOpcodeVal >> 8) & 0xff);  //Print the high byte
                    tempStr[5] = '\0';

                    //Then just copy over the temp string to the correct place in outStr
                    strncpy(outStr+currentAddress*3, tempStr, 5);

                    //Update largestWrittenToAddress                        //+1 is used because the high byte is also written to
                    largestWrittenToAddress = Max(largestWrittenToAddress, currentAddress+1);

                    //Update smallestWrittenToAddress
                    smallestWrittenToAddress = Min(smallestWrittenToAddress, currentAddress);
                }

                //Increment the current address by 2 to account for the opcode
                currentAddress += 2;

                while (isalnum(str[strI])) ++strI;

                //Finally continue reading the string
                continue;

            }

            currentAddress = oldAddress;
            lineNumber = oldLineNumber;

            //If it was not an opcode it must be a label, in which case, check if this is a label declaration or not
            //If the word ends with ':', then it is a declaration. This can be checked for by looping til the character after the label identifier, and checking if it is a ':'.
            //While we are at it, this can also be used to create the label string

            size_t labelLength = LABEL_LENGTH_INC;
            char* label = SafeMalloc(labelLength * sizeof(char));
            size_t labelStrI = 0;
            while (isalnum(str[strI]) || str[strI] == '_') {
                if (labelStrI >= labelLength) {
                    labelLength += LABEL_LENGTH_INC;
                    label = SafeRealloc(label, labelLength * sizeof(char));
                }
                label[labelStrI] = str[strI];
                ++strI;
                ++labelStrI;
            }
            bool isDeclaration = str[strI] == ':';

            //Make sure the label string isn't longer than it needs to be, and is null terminated
            labelLength = labelStrI + 1;
            label = SafeRealloc(label, labelLength * sizeof(char));
            label[labelLength-1] = '\0';

            //If it is a declaration, we do not care about it and can skip it
            if (isDeclaration) {
                free(label);    //Prevent a memory leak

                //Skip past the white space the ':'
                ++strI;
                bool encounteredNewLine = false;
                while (isspace(str[strI])) {
                    if (str[strI] == '\n') {
                        ++lineNumber;
                        currentAddress = lineAddresses[lineNumber-1];
                        encounteredNewLine = true;
                    }
                    ++strI;
                }

                //If there is not an equals, just continue reading the string
                if (str[strI] != '=') continue;

                //Else, skip the equals and the number after it
                ++strI;    //Skips the equals
                while (isspace(str[strI])) {    //Skips the white space after
                    if (str[strI] == '\n') {
                        ++lineNumber;
                        currentAddress = lineAddresses[lineNumber-1];
                        encounteredNewLine = true;
                    }
                    ++strI;
                }
                if (encounteredNewLine) {   //Cannot be a new line between the label declaration and it's initial value, if it is given one
                    fprintf(stderr, "Error: Label declaration and initial value must be on the same line. Label name \"%s\". Line %zu\n", label, lineNumber);
                    exit(EXIT_FAILURE);
                }
                //Skips the number
                while (isalnum(str[strI])) ++strI;

                //Now the rest of the string can be read
                continue;
            }

            //If it is a label, but not it's declaration, find the label in the list of labels

            size_t labelIndex = SIZE_MAX;
            for (size_t i = 0; i < nLabels; i++) {
                if (!strcmp(label, labels[i].str)) {
                    labelIndex = i;
                    break;
                }
            }

            if (labelIndex == SIZE_MAX) {
                //If the label couldn't be found, print an error and exit

                fprintf(stderr, "Error: No label or opcode with name \"%s\". Line %zu\n", label, lineNumber);
                exit(EXIT_FAILURE);
            }

            //Now just print out it's value to outStr at the correct place
            {
                char tempStr[6];    //A temporary string to prevent a null terminator in outStr
                snprintf(tempStr, 3, "%02"PRIx8, (uint8_t)labels[labelIndex].address & 0xff); //Print the low byte
                tempStr[2] = '\n';
                snprintf(tempStr+3, 3, "%02"PRIx8, (uint8_t)(labels[labelIndex].address >> 8) & 0xff);  //Print the high byte
                tempStr[5] = '\0';

                //Then just copy over the temp string to the correct place in outStr
                strncpy(outStr+currentAddress*3, tempStr, 5);

                //Update largestWrittenToAddress                        //+1 is used because the high byte is also written to
                largestWrittenToAddress = Max(largestWrittenToAddress, currentAddress+1);

                //Update smallestWrittenToAddress
                smallestWrittenToAddress = Min(smallestWrittenToAddress, currentAddress);
            }

            //Finally increment the current address by 2 to account for writing 2 bytes
            currentAddress += 2;

        }

    }
    
    DoneAssembling:

    //Print the format for logisim
    fprintf(outFile, "v2.0 raw\n");

    if (writeAllBytes) {
        //Print all the pow(2, 16) bytes to the output file
        fputs(outStr, outFile);
    }
    else {
        //Print only bytes from 0 to largestWrittenToAddress, but using a shortcut for bytes 0 to smallestWrittenToAddress to save on file size
        if (smallestWrittenToAddress > 0) fprintf(outFile, "%"PRIu16"*00\n", smallestWrittenToAddress);
        for (size_t i = smallestWrittenToAddress; i <= largestWrittenToAddress; i++) {
            fprintf(outFile, "%c%c\n", outStr[i*3], outStr[(i*3)+1]);
        }
    }

    free(outStr);

}

//Assumes the label string is dynamically allocated
void Label_InsertLabel(struct Label** labelsPtr, size_t* nLabelsPtr, struct Label label, size_t index) {

    //Allocate space for the label pointer
    while (index >= *nLabelsPtr) {
        *nLabelsPtr += LABELS_SIZE_INC;
        *labelsPtr = SafeRealloc(*labelsPtr, *nLabelsPtr * sizeof(struct Label));
    }

    (*labelsPtr)[index].str = label.str;
    (*labelsPtr)[index].address = label.address;

}

void Label_InsertLine(uint16_t** lineAddressesPtr, size_t* nLinesPtr, uint16_t lineAddress, size_t lineNumber) {

    while (lineNumber >= *nLinesPtr) {
        *nLinesPtr += LINES_SIZE_INC;
        *lineAddressesPtr = SafeRealloc(*lineAddressesPtr, *nLinesPtr*sizeof(uint16_t));
    }

    (*lineAddressesPtr)[lineNumber-1] = lineAddress;

}

//First pass. Gets all the labels and their values
void Label_GetLabels(char* str, struct Label** labelsPtr, size_t* nLabelsPtr, uint16_t** lineAddressesPtr, size_t* nLinesPtr /*Doesn't hold the line number*/) {

    *nLabelsPtr = LABELS_SIZE_INC;
    *labelsPtr = SafeMalloc(*nLabelsPtr * sizeof(struct Label));

    size_t labelIndex = 0;

    *nLinesPtr = LINES_SIZE_INC;
    *lineAddressesPtr = SafeMalloc(*nLinesPtr * sizeof(uint16_t));

    size_t lineNumber = 1;

    uint16_t currentAddress = 0x0000;

    //Go through the entire file and evaluate all the label identifiers and addresses
    size_t strI = 0;
    while (str[strI] != '\0') {

        if (isspace(str[strI])) {
            //Skip past white space
            if (str[strI] == '\n') {
                ++lineNumber;
                Label_InsertLine(lineAddressesPtr, nLinesPtr, currentAddress, lineNumber);
            }
            ++strI;
        }
        else if (str[strI] == ';') {
            //Skip past comments
            while (str[strI] != '\n' && str[strI] != '\0') ++strI;
        }
        else if (isalpha(str[strI])) {

            //Is it an op code, if so, skip past and increment the current address accordingly

            bool isOpCode = false;
            char opCodeStr[4];
            for (size_t i = 0; i < 3; i++) {
                opCodeStr[i] = str[strI+i];
                if (str[strI+i] == '\0') goto DoneGettingLabels;
                if (str[strI] == '\n') {
                    ++lineNumber;
                    Label_InsertLine(lineAddressesPtr, nLinesPtr, currentAddress, lineNumber);
                }
            }
            opCodeStr[3] = '\0';

            for (size_t i = 0; i < N_OPCODES; i++) {
                if (!strcmp(opCodeStr, opStrings[i])) {
                    isOpCode = true;
                    break;
                }
            }
            if (isOpCode) {
                strI += 3;
                currentAddress += 2;
                continue;
            }

            //Is it a register, if so, skip past it
            
            bool isRegister = false;
            char regStr[3];
            for (size_t i = 0; i < 2; i++) {
                regStr[i] = str[strI+i];
                if (str[strI+i] == '\0') goto DoneGettingLabels;
                if (str[strI] == '\n') {
                    ++lineNumber;
                    Label_InsertLine(lineAddressesPtr, nLinesPtr, currentAddress, lineNumber);
                }
            }
            regStr[2] = '\0';

            for (size_t i = 0; i < N_REGISTERS; i++) {
                if (!strcmp(regStr, regStrings[i])) {
                    isRegister = true;
                    break;
                }
            }
            if (isRegister) {
                strI += 2;
                continue;
            }

            //Else it must be a label, in which case, check if this is a label declaration or not
            //If the word ends with ':', then it is a declaration. This can be checked for by looping til the character after the label identifier, and checking if it is a ':'.
            //While we are at it, this can also be used to create the label string

            size_t labelLength = LABEL_LENGTH_INC;
            char* label = SafeMalloc(labelLength * sizeof(char));
            size_t labelStrI = 0;
            while (isalnum(str[strI]) || str[strI] == '_') {
                if (labelStrI >= labelLength) {
                    labelLength += LABEL_LENGTH_INC;
                    label = SafeRealloc(label, labelLength * sizeof(char));
                }
                label[labelStrI] = str[strI];
                ++strI;
                ++labelStrI;
            }
            bool isDeclaration = str[strI] == ':';

            //If it is not a declaration, then skip past it, and increment the current address accordingly
            if (!isDeclaration) {
                currentAddress += 2;
                free(label);    //Prevents a memory leak
                continue;
            }

            //Skip past the ':' for later
            ++strI;

            //Make sure the label string isn't longer than it needs to be, and is null terminated
            labelLength = labelStrI + 1;
            label = SafeRealloc(label, labelLength * sizeof(char));
            label[labelLength-1] = '\0';

            //Move to the next non whitespace character
            while (isspace(str[strI])) {
                if (str[strI] == '\n') {
                    ++lineNumber;
                    Label_InsertLine(lineAddressesPtr, nLinesPtr, currentAddress, lineNumber);
                }
                ++strI;
            }

            struct Label newLabel;
            newLabel.str = label;
            newLabel.address = currentAddress;

            if (str[strI] == '=') {
                //If a value is being asigned to the label
                
                //Skip to the beginning of that value
                ++strI;
                while (isspace(str[strI])) {
                    if (str[strI] == '\n') {
                        ++lineNumber;
                        Label_InsertLine(lineAddressesPtr, nLinesPtr, currentAddress, lineNumber);
                    }
                    ++strI;
                }

                //Read the value into the address of the label
                char* endPtr;
                newLabel.address = strtoul(&str[strI], &endPtr, 0);

                //Skip to the white space after that value
                strI = endPtr - str;
            }

            Label_InsertLabel(labelsPtr, nLabelsPtr, newLabel, labelIndex);

            ++labelIndex;

        }
        else if (isdigit(str[strI])) {
            //Skip past the number and increment the address by 2 bytes
            currentAddress += 2;
            while (!isspace(str[strI])) ++strI;
        }
        else if (str[strI] == '\'') {
            //Skip past the character and increment the address by 2 bytes
            strI += 3;
            currentAddress += 2;
        }
        else if (str[strI] == '#') {

            //If it is a special instruction

            //Get the special instruction as a null terminated string
            char instructionStr[100];
            size_t instructionStrI = 0;
            while (!isspace(str[strI])) {
                instructionStr[instructionStrI] = str[strI];
                ++strI;
                ++instructionStrI;
            }
            instructionStr[instructionStrI] = '\0';

            if (!strcmp(instructionStr, "#org")) {
                //Move to the beginning of the new address
                while (isspace(str[strI])) {
                    if (str[strI] == '\n') {
                        fprintf(stderr, "Error: #org Arguments must be on the same line as the statement. Line %zu\n", lineNumber);
                        exit(EXIT_FAILURE);
                    }
                    ++strI;
                }
                
                //Read the new address
                char* endPtr;
                currentAddress = strtoul(&str[strI], &endPtr, 0);

                //Skip to the white space after that address
                strI = endPtr - str;
            }

        }

    }

    DoneGettingLabels:

    //Make sure the list of labels isn't longer than it needs to be
    *nLabelsPtr = labelIndex;
    *labelsPtr = SafeRealloc(*labelsPtr, *nLabelsPtr * sizeof(struct Label));

    //Make sure the list of line addresses is the correct size
    *nLinesPtr = lineNumber;
    *lineAddressesPtr = SafeRealloc(*lineAddressesPtr, *nLinesPtr * sizeof(uint16_t));

}

char* CopyFileToStr(FILE* file) {

    fseek(file, 0L, SEEK_END);
    size_t fileLength = ftell(file);
    rewind(file);

    char* str = SafeMalloc(fileLength);

    size_t i = 0;
    char c;
    while ((c = fgetc(file)) != EOF) str[i++] = c;

    str[i-1] = '\0';

    return str;

}

int main(int argc, char* argv[]) {

    if (argc < 3) {
        fprintf(stderr, "Error: Path to source and output files required\n");
        exit(EXIT_FAILURE);
    }

    //Get the other arguments aswell
    for (size_t i = 3; i < argc; i++) {
        writeAllBytes = !strcmp(argv[i], "-print_all");
    }
    

    FILE* srcFile = fopen(argv[1], "r");
    if (srcFile == NULL) {
        fprintf(stderr, "Error: Couldn't read source file \"%s\": ", argv[1]);
        perror(NULL);
        exit(EXIT_FAILURE);
    }

    //Copy the source file into a string
    char* srcStr = CopyFileToStr(srcFile);

    //No more need for the source file itself
    fclose(srcFile);

    struct Label* labels = NULL;
    size_t nLabels = 0;
    uint16_t* lineAddresses = NULL; //Keeps track of what address each line in the file starts at
    size_t nLines = 0;
    Label_GetLabels(srcStr, &labels, &nLabels, &lineAddresses, &nLines);


    FILE* outFile = fopen(argv[2], "w");
    if (outFile == NULL) {
        fprintf(stderr, "Error: Couldn't write to output file \"%s\": ", argv[2]);
        perror(NULL);
        exit(EXIT_FAILURE);
    }    

    Assemble_AssembleFile(outFile, srcStr, labels, nLabels, lineAddresses);


    fclose(outFile);    //Done with writing, so file can now be closed

    //Clean everything up and prepare to exit
    free(labels);
    free(lineAddresses);

    free(srcStr);

    return 0;

}
















