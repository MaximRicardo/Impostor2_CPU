//Uses a 256 shade grayscale screen instead of a TTY

#include <iostream>
#include <memory>
#include <cstring>

#include "raylib.h"
#include "Cpu.hpp"
#include "Input.hpp"


#define Screen_Width 320
#define Screen_Height 200

#define Up_Screen_Width 1280
#define Up_Screen_Height 800

#define SCR_ADDRS 0xff0000
#define INPUT_ADDRS 0x030000
#define TTY_ADDRS 0x020000

#define Impostor2_CLOCK_SPEED (1.f/10000000.f)

void ReadExeFile(unsigned char* io, char* fileName) {

	FILE* file = fopen(fileName, "r");

	size_t index = 0;

	//skip the first line
	char c = 0;
	while (c != '\n') c = fgetc(file);

	while (true) {
		char line[2];
		line[0] = fgetc(file);
		if (line[0] == EOF) break;
		line[1] = fgetc(file);
		if (line[1] == EOF) break;

		unsigned char val = strtoul(line, NULL, 16);

        if (fgetc(file) == '*') {

            //Skip val bytes
            index += val;
            
            //Skip the other number
            char c = 0;
            while (c != '\n') c = fgetc(file);
        }
        else {
		    io[index] = val;
		    ++index;
        }

		if (fgetc(file) == EOF) break; //Skip the newline char
	}

}

int main(int argc, char* argv[]) {

    if (argc < 2) {
        fprintf(stderr, "Error: Need file to execute as argument\n");
        return EXIT_FAILURE;
    }

	InitWindow(Up_Screen_Width, Up_Screen_Height, "window");
	SetTargetFPS(60);

	Image img = GenImageColor(Screen_Width, Screen_Height, BLACK);
	ImageFormat(&img, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
	Texture tex = LoadTextureFromImage(img);

	Rectangle sourceRec = { 0, (float)Screen_Height, (float)Screen_Width, (float)Screen_Height };
	Rectangle destRec = { 0, 0, (float)Up_Screen_Width, (float)Up_Screen_Height};

	std::unique_ptr<unsigned char[]> frameBuffer = std::make_unique<unsigned char[]>(Screen_Width * Screen_Height * 4);

	Impostor2::Cpu processor;
	for (int i = 0; i < 8; i++) processor.regs[i] = 0;
	processor.pc = 0;

	//16MB of available io
	std::unique_ptr<unsigned char[]> io = std::make_unique<unsigned char[]>(16777216);
	
	memset(io.get(), 0, 16777216);

	ReadExeFile(io.get(), argv[1]);

	while (!WindowShouldClose()) {
		
		float deltaTime = GetFrameTime();

		char inputs[3];
		Input::GetCharPressed(inputs);
		io[INPUT_ADDRS] = inputs[0];
		io[INPUT_ADDRS + 1] = inputs[1];
		io[INPUT_ADDRS + 2] = inputs[2];

		char key = GetCharPressed();
		io[INPUT_ADDRS] = key;

		float totalCycles = 0.f;
		while (totalCycles * Impostor2_CLOCK_SPEED < deltaTime) {
			totalCycles += processor.ExecInstr(io.get());
		}

		for (int y = 0; y < Screen_Height; y++)
			for (int x = 0; x < Screen_Width; x++) {
				unsigned int i = Screen_Width * y + x;
				unsigned int address = (unsigned int)(io[SCR_ADDRS] << 16) + i;
				frameBuffer[i * 4] = io[address];
				frameBuffer[i * 4 + 1] = io[address];
				frameBuffer[i * 4 + 2] = io[address];
				frameBuffer[i * 4 + 3] = 255;
			}

		BeginDrawing();

		UpdateTexture(tex, frameBuffer.get());
		DrawTexturePro(tex, sourceRec, destRec, { 0.f, 0.f }, 0.f, WHITE);

		EndDrawing();

	}

	UnloadTexture(tex);
	UnloadImage(img);

	CloseWindow();

	return 0;

}
