#include "stdafx.h"
#include "systemclass.h"


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdShow)
{
	SystemClass* System;
	bool result;

	// System 객체 생성
	System = new SystemClass;
	if(!System)
	{
		return 0;
	}

	// system 객체 초기화하고 run을 호출한다.
	result = System->Initialize();
	if(result)
	{
		System->Run();
	}

	// system 객체를 종료하고 메모리를 반환한다.
	System->Shutdown();
	delete System;
	System = nullptr;

	return 0;
}