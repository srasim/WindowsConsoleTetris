#include "InputHandler.h"
#include <Windows.h>
#include <iostream>

InputHandler* InputHandler::s_Instance = nullptr;

bool InputHandler::GetKeyDown(int vKey)
{
    if (vKey == 'R')
    {
        if (!m_RotationHold && (0x8000 & GetAsyncKeyState(vKey)) != 0)
        {
            m_RotationHold = true;
            return true;
        }
        else if(m_RotationHold)
        {
            m_RotationHold = false;
            return false;
        }
    }

    return (0x8000 & GetAsyncKeyState(vKey));
    
}

void InputHandler::GiveInfo()
{
    std::cout << "______INPUT INFO_____" << std::endl;
    std::cout << "MOVE LEFT  : LEFT ARROW" << std::endl;
    std::cout << "MOVE RIGHT : RIGHT ARROW" << std::endl;
    std::cout << "MOVE DOWN  : DOWN ARROW" << std::endl;
    std::cout << "ROTATE     : R" << std::endl;
    std::cout << "GOOD LUCK !!!" << std::endl;
}