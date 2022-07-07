#pragma once
#ifndef _INPUTCLASS_H_
#define _INPUTCLASS_H_

class InputClass
{
public:
	InputClass();
	InputClass(const InputClass& other);
	~InputClass();

	bool Initialize(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight);
	void Shutdown();
	bool Frame();

	bool IsEscapePressed();
	void GetMouseLocation(int& mouseX, int& mouseY);

private:
	bool ReadKeyboard();
	bool ReadMouse();
	void ProcessInput();

private:
	IDirectInput8* m_directInput = nullptr;
	IDirectInputDevice8* m_keyboard = nullptr;
	IDirectInputDevice8* m_mouse = nullptr;

	unsigned char m_keyboardState[256] = { 0, };
	DIMOUSESTATE m_mouseState;

	int m_screenWidth = 0;
	int m_screenHeight = 0;
	int m_mouseX = 0;
	int m_mouseY = 0;
};

#endif // _INPUTCLASS_H_