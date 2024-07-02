#include "Input.hpp"

void Input::GetCharPressed(char* out) {

    char inputArray[4] = { 0 };

    for (int i = 1; i < 4; i++) {
        if (IsKeyDown(KEY_ENTER) && inputArray[i-1] != '\n') {
            inputArray[i] = '\n';
        }
        else if (IsKeyDown(KEY_SPACE) && inputArray[i - 1] != 32) {
            inputArray[i] = 32;              //ASCII for space
        }
        else if (IsKeyDown(KEY_ZERO) && inputArray[i - 1] != '0') {
            inputArray[i] = '0';
        }
        else if (IsKeyDown(KEY_ONE) && inputArray[i - 1] != '1') {
            inputArray[i] = '1';
        }
        else if (IsKeyDown(KEY_TWO) && inputArray[i - 1] != '2') {
            inputArray[i] = '2';
        }
        else if (IsKeyDown(KEY_THREE) && inputArray[i - 1] != '3') {
            inputArray[i] = '3';
        }
        else if (IsKeyDown(KEY_FOUR) && inputArray[i - 1] != '4') {
            inputArray[i] = '4';
        }
        else if (IsKeyDown(KEY_FIVE) && inputArray[i - 1] != '5') {
            inputArray[i] = '5';
        }
        else if (IsKeyDown(KEY_SIX) && inputArray[i - 1] != '6') {
            inputArray[i] = '6';
        }
        else if (IsKeyDown(KEY_SEVEN) && inputArray[i - 1] != '7') {
            inputArray[i] = '7';
        }
        else if (IsKeyDown(KEY_EIGHT) && inputArray[i - 1] != '8') {
            inputArray[i] = '8';
        }
        else if (IsKeyDown(KEY_NINE) && inputArray[i - 1] != '9') {
            inputArray[i] = '9';
        }
        else if (IsKeyDown(KEY_APOSTROPHE) && inputArray[i - 1] != 39) {
            inputArray[i] = 39;
        }
        else if (IsKeyDown(KEY_COMMA) && inputArray[i - 1] != ',') {
            inputArray[i] = ',';
        }
        else if (IsKeyDown(KEY_MINUS) && inputArray[i - 1] != '-') {
            inputArray[i] = '-';
        }
        else if (IsKeyDown(KEY_PERIOD) && inputArray[i - 1] != '.') {
            inputArray[i] = '.';
        }
        else if (IsKeyDown(KEY_SLASH) && inputArray[i - 1] != '/') {
            inputArray[i] = '/';
        }
        else if (IsKeyDown(KEY_SEMICOLON) && inputArray[i - 1] != ';') {
            inputArray[i] = ';';
        }
        else if (IsKeyDown(KEY_EQUAL) && inputArray[i - 1] != '=') {
            inputArray[i] = '=';
        }
        else if (IsKeyDown(KEY_Q) && inputArray[i - 1] != 'Q') {
            inputArray[i] = 'Q';
        }
        else if (IsKeyDown(KEY_W) && inputArray[i - 1] != 'W') {
            inputArray[i] = 'W';
        }
        else if (IsKeyDown(KEY_E) && inputArray[i - 1] != 'E') {
            inputArray[i] = 'E';
        }
        else if (IsKeyDown(KEY_R) && inputArray[i - 1] != 'R') {
            inputArray[i] = 'R';
        }
        else if (IsKeyDown(KEY_T) && inputArray[i - 1] != 'T') {
            inputArray[i] = 'T';
        }
        else if (IsKeyDown(KEY_Y) && inputArray[i - 1] != 'Y') {
            inputArray[i] = 'Y';
        }
        else if (IsKeyDown(KEY_U) && inputArray[i - 1] != 'U') {
            inputArray[i] = 'U';
        }
        else if (IsKeyDown(KEY_I) && inputArray[i - 1] != 'I') {
            inputArray[i] = 'I';
        }
        else if (IsKeyDown(KEY_O) && inputArray[i - 1] != 'O') {
            inputArray[i] = 'O';
        }
        else if (IsKeyDown(KEY_P) && inputArray[i - 1] != 'P') {
            inputArray[i] = 'P';
        }
        else if (IsKeyDown(KEY_A) && inputArray[i - 1] != 'A') {
            inputArray[i] = 'A';
        }
        else if (IsKeyDown(KEY_S) && inputArray[i - 1] != 'S') {
            inputArray[i] = 'S';
        }
        else if (IsKeyDown(KEY_D) && inputArray[i - 1] != 'D') {
            inputArray[i] = 'D';
        }
        else if (IsKeyDown(KEY_F) && inputArray[i - 1] != 'F') {
            inputArray[i] = 'F';
        }
        else if (IsKeyDown(KEY_G) && inputArray[i - 1] != 'G') {
            inputArray[i] = 'G';
        }
        else if (IsKeyDown(KEY_H) && inputArray[i - 1] != 'H') {
            inputArray[i] = 'H';
        }
        else if (IsKeyDown(KEY_J) && inputArray[i - 1] != 'J') {
            inputArray[i] = 'J';
        }
        else if (IsKeyDown(KEY_K) && inputArray[i - 1] != 'K') {
            inputArray[i] = 'K';
        }
        else if (IsKeyDown(KEY_L) && inputArray[i - 1] != 'L') {
            inputArray[i] = 'L';
        }
        else if (IsKeyDown(KEY_Z) && inputArray[i - 1] != 'Z') {
            inputArray[i] = 'Z';
        }
        else if (IsKeyDown(KEY_X) && inputArray[i - 1] != 'X') {
            inputArray[i] = 'X';
        }
        else if (IsKeyDown(KEY_C) && inputArray[i - 1] != 'C') {
            inputArray[i] = 'C';
        }
        else if (IsKeyDown(KEY_V) && inputArray[i - 1] != 'V') {
            inputArray[i] = 'V';
        }
        else if (IsKeyDown(KEY_B) && inputArray[i - 1] != 'B') {
            inputArray[i] = 'B';
        }
        else if (IsKeyDown(KEY_N) && inputArray[i - 1] != 'N') {
            inputArray[i] = 'N';
        }
        else if (IsKeyDown(KEY_M) && inputArray[i - 1] != 'M') {
            inputArray[i] = 'M';
        }
        else if (IsKeyDown(KEY_UP) && inputArray[i - 1] != 1) {
            inputArray[i] = 1;
        }
        else if (IsKeyDown(KEY_LEFT) && inputArray[i - 1] != 2) {
            inputArray[i] = 2;
        }
        else if (IsKeyDown(KEY_DOWN) && inputArray[i - 1] != 3) {
            inputArray[i] = 3;
        }
        else if (IsKeyDown(KEY_RIGHT) && inputArray[i - 1] != 4) {
            inputArray[i] = 4;
        }
        else if (IsKeyDown(KEY_TAB) && inputArray[i - 1] != 5) {
            inputArray[i] = 5;
        }
        else if (IsKeyDown(KEY_BACKSPACE) && inputArray[i - 1] != 8) {
            inputArray[i] = 8;
        }
        else if (IsKeyDown(KEY_CAPS_LOCK) && inputArray[i - 1] != 6) {
            inputArray[i] = 6;
        }
        else if (IsKeyDown(KEY_LEFT_SHIFT) && inputArray[i - 1] != 7) {
            inputArray[i] = 7;
        }
        else if (IsKeyDown(KEY_RIGHT_SHIFT) && inputArray[i - 1] != 9) {
            inputArray[i] = 9;
        }
        else if (IsKeyDown(KEY_LEFT_CONTROL) && inputArray[i - 1] != 10) {
            inputArray[i] = 10;
        }
        else if (IsKeyDown(KEY_RIGHT_CONTROL) && inputArray[i - 1] != 11) {
            inputArray[i] = 11;
        }
        else if (IsKeyDown(KEY_LEFT_ALT) && inputArray[i - 1] != 12) {
            inputArray[i] = 12;
        }
        else if (IsKeyDown(KEY_RIGHT_ALT) && inputArray[i - 1] != 13) {
            inputArray[i] = 13;
        }
        else
            inputArray[i] = 0;
    }

    for (int i = 0; i < 3; i++)
        out[i] = inputArray[i + 1];
}