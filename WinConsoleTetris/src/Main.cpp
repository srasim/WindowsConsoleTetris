#include <iostream>
#include <Windows.h>
#include <thread>
#include "InputHandler.h"
#include <vector>

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

bool DoesTetrominoCollide(int nTetromino, int nRotation, int nCurrentPosX, int nCurrentPosY) //Collision detection
{
    for (int px = 0; px < nTetrominoWidth; px++)
    {
        for (int py = 0; py < nTetrominoHeight; py++)
        {
            int pi = GetRotatedIndex(px, py, nRotation);

            int fi = (nCurrentPosY + py) * nPlayFieldWidth + (nCurrentPosX + px);

            if (tetrominos[nTetromino][pi] == 'X' && pPlayField[fi] != 0)
            {
                return true; // Collision detected
            }
        }
    }
    return false;
}

void LogStartInfo(char* screen, HANDLE hBuffer, DWORD dwBytesWritten)
{
    sprintf_s(&screen[3 * nScreenWidth + ((nScreenWidth - 2) / 2 - nPlayFieldWidth / 2)], 12, "WELCOME !!!");

    sprintf_s(&screen[6 * nScreenWidth + ((nScreenWidth - 2) / 2 - nPlayFieldWidth / 2) ]   , 11,   "INPUT INFO");
    sprintf_s(&screen[8 * nScreenWidth + ((nScreenWidth - 2) / 2 - nPlayFieldWidth / 2) -3] , 15,   "ROTATE     : R");
    sprintf_s(&screen[9 * nScreenWidth + ((nScreenWidth - 2) / 2 - nPlayFieldWidth / 2) -3] , 24,   "MOVE DOWN  : DOWN ARROW");
    sprintf_s(&screen[10 * nScreenWidth + ((nScreenWidth - 2) / 2 - nPlayFieldWidth / 2) -3], 24,   "MOVE LEFT  : LEFT ARROW");
    sprintf_s(&screen[11 * nScreenWidth + ((nScreenWidth - 2) / 2 - nPlayFieldWidth / 2) -3], 25,   "MOVE RIGHT : RIGHT ARROW");

    WriteConsoleOutputCharacterA(hBuffer, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);//Display
}

void ClearScreen(char* screen)
{
    for (int i = 0; i < nScreenHeight * nScreenWidth; i++)
    {
        screen[i] = ' ';
    }
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

    tetrominos[6].append("....");
    tetrominos[6].append(".XX.");
    tetrominos[6].append(".X..");
    tetrominos[6].append(".X..");
#pragma endregion

    wchar_t consoleTitle[7] = L"Tetris";
    HANDLE hBuffer = GetStdHandle(STD_OUTPUT_HANDLE); 
    SMALL_RECT rectWindow = { 0, 0, 1, 1 };
    SetConsoleWindowInfo(hBuffer, TRUE, &rectWindow);

    COORD bufferSize = { (short)nScreenWidth, (short)nScreenHeight };
    SetConsoleScreenBufferSize(hBuffer, bufferSize);

    SetConsoleActiveScreenBuffer(hBuffer);
 
    
    rectWindow = { 0, 0, (short)nScreenWidth - 1, (short)nScreenHeight - 1 };
    SetConsoleWindowInfo(hBuffer, TRUE, &rectWindow);

    char* screen = new char[nScreenWidth * nScreenHeight];

    memset(screen, 0, sizeof(char) * nScreenWidth * nScreenHeight);
    SetConsoleTitle(consoleTitle);
    
    ClearScreen(screen);
  

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
    int nPlayFieldOffsetX = nScreenWidth / 2 - (nPlayFieldWidth / 2);
    int nPlayFieldOffsetY = nScreenHeight /2 - (nPlayFieldHeight / 2);
    bool bIsGameOver = false;
    int nCurrentTetrominoIndex = 1;
    int nCurrentRotation = 0;
    int nCurrentPosX = (nScreenWidth - 2) / 2;
    int nCurrentPosY = 0;

    int bForceDown = false;
    int nForceDownCounter = 0;
    int nForceDownThreshold = 20;

    std::vector<int> vLines;

    int nLockedTetrominoCount = 0;
    int nScore = 0; // Gain 25 points for every locked tetromino

    LogStartInfo(screen, hBuffer, dwBytesWritten); 
    std::this_thread::sleep_for(std::chrono::seconds(3));
    ClearScreen(screen);

    //DRAW PLAY FIELD WITH ANIM
    for (int y = 0; y < nPlayFieldHeight; y++)
    {
        for (int x= 0; x < nPlayFieldWidth; x++)
        {
            screen[(y + nPlayFieldOffsetY) * nScreenWidth + x + nPlayFieldOffsetX] = " #X="[pPlayField[y * nPlayFieldWidth + x]];
            WriteConsoleOutputCharacterA(hBuffer, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);//Display
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
    }

    while (!bIsGameOver)
    {
        //GAME TIMING
        std::this_thread::sleep_for(std::chrono::milliseconds(50)); //Game tick

        nForceDownCounter++;
        bForceDown = (nForceDownCounter == nForceDownThreshold);

#pragma region Input
        if (InputHandler::GetInstance()->GetKeyDown(VK_DOWN) && !DoesTetrominoCollide(nCurrentTetrominoIndex, nCurrentRotation, nCurrentPosX - nPlayFieldOffsetX, nCurrentPosY + 1))
        {
            nCurrentPosY++;
        }

        if (InputHandler::GetInstance()->GetKeyDown(VK_LEFT) && !DoesTetrominoCollide(nCurrentTetrominoIndex, nCurrentRotation, nCurrentPosX - nPlayFieldOffsetX - 1, nCurrentPosY))
        {
            nCurrentPosX--;
        }

        if (InputHandler::GetInstance()->GetKeyDown(VK_RIGHT) && !DoesTetrominoCollide(nCurrentTetrominoIndex, nCurrentRotation, nCurrentPosX - nPlayFieldOffsetX + 1, nCurrentPosY))
        {
            nCurrentPosX++;
        }

        if (InputHandler::GetInstance()->GetKeyDown('R') && !DoesTetrominoCollide(nCurrentTetrominoIndex, nCurrentRotation + 1, nCurrentPosX - nPlayFieldOffsetX, nCurrentPosY))
        {
            nCurrentRotation++;
        }
#pragma endregion

        if (bForceDown)
        {
            if (!DoesTetrominoCollide(nCurrentTetrominoIndex, nCurrentRotation, nCurrentPosX - nPlayFieldOffsetX, nCurrentPosY + 1))
            {
                nCurrentPosY++;
            }

            else //Cant move anymore. Lock tetromino
            {
                for (int px = 0; px < nTetrominoWidth; px++)
                {
                    for (int py = 0; py < nTetrominoHeight; py++)
                    {
                        if (tetrominos[nCurrentTetrominoIndex][GetRotatedIndex(px, py, nCurrentRotation)] == 'X')
                        {
                            pPlayField[(py + nCurrentPosY) * nPlayFieldWidth + nCurrentPosX - nPlayFieldOffsetX + px] = 2;
                        }
                    }
                }

                nLockedTetrominoCount++;
                nScore += 25; //Gain 25 points for every locked tetromino

                //After every 10 locked tetromino increase speed
                if (nLockedTetrominoCount % 10 == 0)
                {
                    if (nForceDownThreshold >= 10)
                    {
                        nForceDownThreshold--;
                    }
                }

                //Check lines
                for (int py = 0; py < nTetrominoHeight; py++) // Check last tetmonio rows
                {
                    if (nCurrentPosY + py < nPlayFieldHeight - 1) // Do not check boundaries Y
                    {
                        bool bHasLine = true;

                        for (int px = 1; px < nPlayFieldWidth - 1; px++) // Do not check boundaries X
                        {
                            //If any space return false       
                            bHasLine &= (pPlayField[(nCurrentPosY + py) * nPlayFieldWidth + px]) != 0;
                        }

                        if (bHasLine) // Line detected
                        {
                            for (int px = 1; px < nPlayFieldWidth - 1; px++)
                            {
                                //Remove line and set 3 to display as '='
                                pPlayField[(nCurrentPosY + py) * nPlayFieldWidth + px] = 3;
                            }

                            //Add detected line to vector
                            vLines.push_back(nCurrentPosY + py);
                        }
                    }
                }

                nScore += vLines.size() * 100; //Gain 100 points for every line

                //Set new tetromino
                nCurrentPosX = (nScreenWidth - 2) / 2;
                nCurrentPosY = 0;
                nCurrentTetrominoIndex = rand() % 7;
                nCurrentRotation = 0;

                //If next piece collides game over
                bIsGameOver = DoesTetrominoCollide(nCurrentTetrominoIndex, nCurrentRotation, nCurrentPosX - nPlayFieldOffsetX, nCurrentPosY);
            }


            nForceDownCounter = 0;
        }

        //DRAW PLAY FIELD
        for (int x = 0; x < nPlayFieldWidth; x++)
        {
            for (int y = 0; y < nPlayFieldHeight; y++)
            {
                screen[(y + nPlayFieldOffsetY) * nScreenWidth + x + nPlayFieldOffsetX] = " #X="[pPlayField[y * nPlayFieldWidth + x]]; // Set boundary elements as '#"
            }
        }

        //DRAW CURRENT PIECE
        for (int px = 0; px < nTetrominoWidth; px++)
        {
            for (int py = 0; py < nTetrominoHeight; py++)
            {
                if (tetrominos[nCurrentTetrominoIndex][GetRotatedIndex(px, py, nCurrentRotation)] == 'X')
                {
                    screen[(nCurrentPosY + nPlayFieldOffsetY + py) * nScreenWidth + nCurrentPosX + px] = 'X';
                }
            }
        }

        if (!vLines.empty())
        {
            //Display frame with delay to show lines
            WriteConsoleOutputCharacterA(hBuffer, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);//Display
            std::this_thread::sleep_for(std::chrono::milliseconds(400)); //400 ms delay

            for (auto& l : vLines)
            {
                for (int x = 1; x < nPlayFieldWidth - 1; x++) //Do not change boundaries X
                {
                    for (int y = l; y > 0; y--)
                    {
                        //Shift rows
                        pPlayField[y * nPlayFieldWidth + x] = pPlayField[(y - 1) * nPlayFieldWidth + x];
                    }

                    pPlayField[x] = 0; //Set first row empty
                }
            }
            //clear lines vector
            vLines.clear();
        }

        //Draw Score
        sprintf_s(&screen[(nPlayFieldOffsetY - 5)  * nScreenWidth + nPlayFieldOffsetX], 13, "SCORE: %5d", nScore);

        WriteConsoleOutputCharacterA(hBuffer, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);//Display
    }
    
 
    int row;
    int index;

    for (int i = 0; i < nPlayFieldHeight; i++)
    {
        row = (nPlayFieldOffsetY + i) * nScreenWidth;

        for (int j = 0; j < nPlayFieldWidth; j++)
        {
            index = row + nPlayFieldOffsetX + j;

            screen[index] = ' ';
            WriteConsoleOutputCharacterA(hBuffer, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);//Display
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    sprintf_s(&screen[5 * nScreenWidth + ((nScreenWidth - 2) / 2 - nPlayFieldWidth / 2) + 1], 14,"GAME OVER !!!");
    WriteConsoleOutputCharacterA(hBuffer, screen, nScreenWidth* nScreenHeight, { 0,0 }, & dwBytesWritten);//Display
        
    std::this_thread::sleep_for(std::chrono::seconds(2));

    CloseHandle(hBuffer);
    return 0;
}
