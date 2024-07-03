#include <iostream>
#include <memory>
#include <cstring>

#include "raylib.h"
#include "Cpu.hpp"


#define SCR_WIDTH 1280
#define SCR_HEIGHT 720

#define INPUT_ADDRESS 0x030000
#define TTY_ADDRESS 0x020000

#define IMPOSTOR2_CLOCK_SPEED (1.f/10000000.f)

#define CHAR_SIZE 20

#define CHAR_RES_WIDTH SCR_WIDTH/CHAR_SIZE
#define CHAR_RES_HEIGHT SCR_HEIGHT/CHAR_SIZE

void ReadExeFile(unsigned char* io, char* fileName) {

	FILE* file = fopen(fileName, "r");

    //Get the length of the file
    fseek(file, 0L, SEEK_END);
    size_t fileLength = ftell(file);
    rewind(file);

    //Allocate a string to store the file within
    std::unique_ptr<char[]> fileStr = std::make_unique<char[]>(fileLength);

    //Copy the file into the string
    {
        char c;
        size_t i = 0;
        while ((c = fgetc(file)) != EOF) fileStr[i++] = c;
    }

    //Close the file
    fclose(file);

    //Split the string into lines
    strtok(fileStr.get(), "\n");

    //Skip the first line because it just contains information for logisim
    char* line = strtok(NULL, "\n");

    size_t currentAddress = 0;

    while (line != NULL) {

        //Get the number on the line
        char* endPtr;
        size_t num = strtoul(line, &endPtr, 16);

        //Is there a multiplication after the number?
        if (*endPtr == '*') {
            //If so, skip num lines and fill them with the next number instead.
            
            //Move to the next character
            ++endPtr;

            //Read the number
            unsigned char fillNum = strtoul(line, &endPtr, 16);

            //Perform the fill
            size_t endAddress = currentAddress + num;
            for (; currentAddress < endAddress; currentAddress++) {
                io[currentAddress] = fillNum;
            }
        }
        else {
            //If not, write num to the current address, then move to the next byte
            io[currentAddress] = num;
            ++currentAddress;
        }

        //Now move to the next line
        line = strtok(NULL, "\n");

    }

}

void ShiftCharBufferUp(char* charBuffer) {

    for (size_t y = 0; y < CHAR_RES_HEIGHT; y++) {
        for (size_t x = 0; x < CHAR_RES_WIDTH; x++) {
            if (y != CHAR_RES_HEIGHT-1) charBuffer[CHAR_RES_WIDTH*y+x] = charBuffer[CHAR_RES_WIDTH*(y+1)+x];
            else charBuffer[CHAR_RES_WIDTH*y+x] = 0;
        }
    }

}

int main(int argc, char* argv[]) {

    if (argc < 2) {
        fprintf(stderr, "Error: Need file to execute as argument\n");
        return EXIT_FAILURE;
    }

	InitWindow(SCR_WIDTH, SCR_HEIGHT, "window");
	SetTargetFPS(60);

	Impostor2::Cpu processor;
	for (int i = 0; i < 8; i++) processor.regs[i] = 0;
	processor.pc = 0;

	//16MB of available io
	std::unique_ptr<unsigned char[]> io = std::make_unique<unsigned char[]>(16777216);
	memset(io.get(), 0, 16777216);

    std::unique_ptr<char[]> charBuffer = std::make_unique<char[]>(CHAR_RES_WIDTH*CHAR_RES_HEIGHT);
    memset(charBuffer.get(), 0, CHAR_RES_WIDTH*CHAR_RES_HEIGHT);

    //Text cursor position
    unsigned int cursorX = 0;
    unsigned int cursorY = 0;
	
	ReadExeFile(io.get(), argv[1]);

	while (!WindowShouldClose()) {
		
		float deltaTime = GetFrameTime();

        char key = GetCharPressed();
        if (IsKeyPressed(KEY_ENTER)) key = '\n';
        else if (IsKeyPressed(KEY_BACKSPACE)) key = 8;
        io[INPUT_ADDRESS] = key;

		float totalCycles = 0.f;
		while (totalCycles * IMPOSTOR2_CLOCK_SPEED < deltaTime) {

            bool accessedInput;
			totalCycles += processor.ExecInstr(io.get(), &accessedInput);

            if (accessedInput) io[INPUT_ADDRESS] = 0;

            if (io[TTY_ADDRESS] == '\n') {
                if (cursorY < CHAR_RES_HEIGHT-1) ++cursorY;
                else ShiftCharBufferUp(charBuffer.get());
                cursorX = 0;

                io[TTY_ADDRESS] = 0;
            }
            else if (io[TTY_ADDRESS] == 8) {
                if (cursorX > 0)
                    --cursorX;
                else if (cursorY > 0) {
                    cursorX = CHAR_RES_WIDTH-1;
                    --cursorY;
                }
                charBuffer[CHAR_RES_WIDTH*cursorY+cursorX] = 0;

                io[TTY_ADDRESS] = 0;
            }
            else if (io[TTY_ADDRESS] != 0) {
                charBuffer[CHAR_RES_WIDTH*cursorY+cursorX] = io[TTY_ADDRESS];

                if (cursorX < CHAR_RES_WIDTH-1) {
                    ++cursorX;
                }
                else {
                    cursorX = 0;
                    if (cursorY < CHAR_RES_HEIGHT-1) ++cursorY;
                    else ShiftCharBufferUp(charBuffer.get());
                }

                io[TTY_ADDRESS] = 0;
            }
		}

		BeginDrawing();

        ClearBackground(BLACK);

        for (size_t y = 0; y < CHAR_RES_HEIGHT; y++) {
            for (size_t x = 0; x < CHAR_RES_WIDTH; x++) {
                DrawText(TextFormat("%c", charBuffer[CHAR_RES_WIDTH*y+x]), x * CHAR_SIZE, y * CHAR_SIZE, CHAR_SIZE, WHITE);
            }
        }

		EndDrawing();

	}

	CloseWindow();

	return 0;

}
