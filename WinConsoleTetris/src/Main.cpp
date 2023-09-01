#include <iostream>
#include <Windows.h>

int nScreenWidth = 80;
int nScreenHeight = 30;

int nPlayFieldWidth = 12;
int nPlayFieldHeight = 18;

unsigned char* pPlayField = nullptr; //For play field buffer

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


    pPlayField = new unsigned char[nPlayFieldWidth * nPlayFieldHeight]; // Create play field buffer

    for (int x = 0; x < nPlayFieldWidth; x++) // Set play field boundary
    {
        for (int y = 0; y < nPlayFieldHeight; y++)
        {
            if (x == 0 || x == nPlayFieldWidth - 1 || y == nPlayFieldHeight - 1)
            {
                pPlayField[y * nPlayFieldWidth + x] = 1; // Boundary elements are 1
            }
            else
            {
                pPlayField[y * nPlayFieldWidth + x] = 0;
            }
        }
    }

    //Set screen 
    for (int x = 0; x < nPlayFieldWidth; x++)
    {
        for (int y = 0; y < nPlayFieldHeight; y++)
        {
            screen[y * nScreenWidth + x] = " #X="[pPlayField[y * nPlayFieldWidth + x]]; // Set boundary elements as '#"
        }
    }

    DWORD dwBytesWritten = 0;

    WriteConsoleOutputCharacterA(hBuffer, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten); //Display frame
    while (true)
    {
        WriteConsoleOutputCharacterA(hBuffer, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);//Display
    }

    return 0;
}
