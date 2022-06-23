#pragma once
#ifndef _SYSTEMCLASS_H_
#define _SYSTEMCLASS_H_


//#include <windows.h>

#include "inputclass.h"
#include "graphicsclass.h"

class SystemClass
{
public:
	SystemClass();
	SystemClass(const SystemClass&);
	~SystemClass();

	bool Initialize();
	void Shutdown();
	void Run();
	HWND& GetHWND();

	LRESULT CALLBACK MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam);

	// todo 수정 필요, 좋은 구조 아님.
	// SystemClass 객체 하나만 만들어지기 때문에 메시지박스 용도로 사용중.
	static SystemClass* ApplicationHandle;

private:
	bool Frame();
	void InitializeWindows(int&, int&);
	void ShutdownWindows();


private:
	LPCWSTR m_applicationName;
	HINSTANCE m_hinstance;
	HWND m_hwnd;

	InputClass* m_Input = nullptr;
	GraphicsClass* m_Graphics = nullptr;
	
};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//static SystemClass* ApplicationHandle = nullptr;




#endif
