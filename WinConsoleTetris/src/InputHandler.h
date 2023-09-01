#ifndef INPUTHANDLER
#define INPUTHANDLER

class InputHandler {

public:
	inline static InputHandler* GetInstance()
	{
		return s_Instance = (s_Instance != nullptr) ? s_Instance : new InputHandler();
	}

	bool GetKeyDown(int vKey);

private:

	InputHandler()
	{
		m_RotationHold = false;
	}

	bool m_RotationHold;
	static InputHandler* s_Instance;
};
#endif