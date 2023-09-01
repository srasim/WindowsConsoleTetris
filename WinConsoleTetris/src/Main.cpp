#include <iostream>
#include <Windows.h>

int nScreenWidth = 30;
int nScreenHeight = 120;


int main() {


    char* screen = new char[nScreenWidth * nScreenHeight];

    for (int i = 0; i < nScreenHeight * nScreenWidth; i++)
    {
        screen[i] = ' ';
    }

    HANDLE hBuffer = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL); //Create screen buffer
    SetConsoleActiveScreenBuffer(hBuffer); //Make created buffer active buffer

    COORD bufferSize = { nScreenWidth,nScreenHeight };
    SetConsoleScreenBufferSize(hBuffer, bufferSize); // Set screeen buffer size

    DWORD dwBytesWritten = 0;

    WriteConsoleOutputCharacterA(hBuffer, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten); //Display frame

    return 0;
}