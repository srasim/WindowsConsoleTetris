#include <iostream>
#include <Windows.h>

int nScreenWidth = 80;
int nScreenHeight = 30;

int nPlayFieldWidth = 12;
int nPlayFieldHeight = 18;

int nTetrominoWidth = 4;
int nTetrominoHeight = 4;

unsigned char* pPlayField = nullptr; //For play field buffer

std::string tetrominos[7]; //For tetrominos (assets)

int GetRotatedIndex(int px, int py, int r) { //Returns the rotated index of element in a tetromino
    //Rotation and indexing logic
    r %= 4;
    //r represents count of "90 degree clockwise rotation"
    //if r = 2 that means func returns the index of the 180 degree clockwise rotated tetromino element
    while (r > 0)
    {
        int temp = py;
        py = nTetrominoWidth - px - 1;
        px = temp;
        r--;
    }

    return py * nTetrominoWidth + px;
}

int main() {

#pragma region Assets

    //Every tetromino is a string that has 4*4 elements
   // 'X' defines the shape
    tetrominos[0].append("..X.");
    tetrominos[0].append("..X.");
    tetrominos[0].append("..X.");
    tetrominos[0].append("..X.");

    tetrominos[1].append("..X.");
    tetrominos[1].append(".XX.");
    tetrominos[1].append(".X..");
    tetrominos[1].append("....");

    tetrominos[2].append(".X..");
    tetrominos[2].append(".XX.");
    tetrominos[2].append("..X.");
    tetrominos[2].append("....");

    tetrominos[3].append("....");
    tetrominos[3].append(".XX.");
    tetrominos[3].append(".XX.");
    tetrominos[3].append("....");

    tetrominos[4].append("..X.");
    tetrominos[4].append(".XX.");
    tetrominos[4].append("..X.");
    tetrominos[4].append("....");

    tetrominos[5].append("....");
    tetrominos[5].append(".XX.");
    tetrominos[5].append("..X.");
    tetrominos[5].append("..X.");

    tetrominos[6].append("..X.");
    tetrominos[6].append("..X.");
    tetrominos[6].append("..X.");
    tetrominos[6].append("..X.");
#pragma endregion

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

    DWORD dwBytesWritten = 0;
    int nPLayFieldOffsetX = nScreenWidth / 2 - (nPlayFieldWidth / 2);
    bool bIsGameOver = false;
    int nCurrentPieceIndex = 1;
    int nCurrentRotation = 0;
    int nCurrentPosX = nScreenWidth / 2;
    int nCurrentPosY = 0;

    while (!bIsGameOver)
    {

        //DRAW PLAY FIELD
        for (int x = 0; x < nPlayFieldWidth; x++)
        {
            for (int y = 0; y < nPlayFieldHeight; y++)
            {
                screen[(y + 5) * nScreenWidth + x + nPLayFieldOffsetX] = " #X="[pPlayField[y * nPlayFieldWidth + x]]; // Set boundary elements as '#"
            }
        }

        //DRAW CURRENT PIECE
        for (int px = 0; px < nTetrominoWidth; px++)
        {
            for (int py = 0; py < nTetrominoHeight; py++)
            {
                if (tetrominos[nCurrentPieceIndex][GetRotatedIndex(px, py, nCurrentRotation)] == 'X')
                {
                    screen[(nCurrentPosY + 5 + py) * nScreenWidth + nCurrentPosX + px] = 'X';
                }
            }
        }


        WriteConsoleOutputCharacterA(hBuffer, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);//Display
    }
  

    return 0;
}
