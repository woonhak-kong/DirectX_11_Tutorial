#include "stdafx.h"
#include "systemclass.h"


SystemClass* SystemClass::ApplicationHandle = nullptr;

SystemClass::SystemClass()
{

}

SystemClass::SystemClass(const SystemClass&)
{
}

SystemClass::~SystemClass()
{
}

bool SystemClass::Initialize()
{
	int screenWidth, screenHeight;
	bool result;

	// 함수에 높이와 너비를 전달하기 전에 변수를 0으로 초기화한다.
	screenWidth = 0;
	screenHeight = 0;

	// 윈도우즈 api를 사용하여 초기화한다.
	InitializeWindows(screenWidth, screenHeight);

	// input 객체를 생성한다.
	// 이 객체는 유저로부터 들어오는 키보드 입력을 처리하기위해 사용된다.
	m_Input = new InputClass;
	if (!m_Input)
	{
		return false;
	}

	// Input 객체를 초기화한다.
	m_Input->Initialize();

	// graphics 객체를 생성한다.
	// 이 객체는 이 어플리케이션의 모든 그래픽 요소를 그리는 일을 한다.
	m_Graphics = new GraphicsClass;
	if (!m_Graphics)
	{
		return false;
	}

	// graphics 객체를 초기화 한다.
	result = m_Graphics->Initialize(screenWidth, screenHeight, m_hwnd);
	if (!result)
	{
		return false;
	}

	return true;
}

void SystemClass::Shutdown()
{

	// Graphics 객체를 반환합니다.
	if (m_Graphics)
	{
		m_Graphics->Shutdown();
		delete m_Graphics;
		m_Graphics = nullptr;
	}

	// Input 객체를 반환합니다.
	if (m_Input)
	{
		delete m_Input;
		m_Input = nullptr;
	}

	// 창을 종료시킵니다.
	ShutdownWindows();
	return;
}

void SystemClass::Run()
{
	MSG msg;
	bool done, result;

	// 메세지 구조체를 초기화합니다.
	ZeroMemory(&msg, sizeof(MSG));

	// 유저로부터 종료 메시지를 받을 때까지 루프를 돈다.
	done = false;
	while (!done)
	{
		// 윈도우 메시지를 처리합니다.
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// 윈도우에서 어플리케이션의 종료를 요청하는 경우 빠져나갑니다.
		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			// 그 외에는 Frame 함수를 처리합니다.
			result = Frame();
			if (!result)
			{
				done = true;
			}
		}
	}
}

HWND& SystemClass::GetHWND()
{
	return m_hwnd;
}

LRESULT SystemClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch (umsg)
	{
		// 키보드키가 눌렸는지 확인
		case WM_KEYDOWN:
		{
			m_Input->KeyDown(static_cast<unsigned int>(wparam));
			return 0;
		}

		// 키보드의 눌린 키가 떼어졌는지 확인
		case WM_KEYUP:
		{
			// 키가 떼어졌다면 input 객체에 이 사실을 전달하여 이 키를 해제토록 한다.
			m_Input->KeyUp(static_cast<unsigned int>(wparam));
			return 0;
		}

		// 다른 메세지들은 사용하지 않으므로 기본 메세지 처리기에 전달
		default:
		{
			return DefWindowProc(hwnd, umsg, wparam, lparam);
		}
	}
}

bool SystemClass::Frame()
{

	bool result;

	// 유저가 Esc키를 누를시 종료.
	if (m_Input->IsKeyDown(VK_ESCAPE))
	{
		return false;
	}

	// graphics 객체의 작업을 처리합니다.
	result = m_Graphics->Frame();
	if (!result)
	{
		return false;
	}

	return true;
}

void SystemClass::InitializeWindows(int& screenWidth, int& screenHeight)
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;

	// 외부 포인터를 이 객체로 설정
	ApplicationHandle = this;
	
	// 이 어플리케이션의 인스턴스 가져오기.
	m_hinstance = GetModuleHandle(nullptr);

	// 엎플리케이션의 이름 설정
	m_applicationName = L"Engine";

	// 윈도우 클래스를 기본 설정으로 설정.
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hinstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = m_applicationName;
	wc.cbSize = sizeof(WNDCLASSEX);

	// 윈도우 클래스 등록
	RegisterClassEx(&wc);

	// 모니터 화면의 해상도 가져오기
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	if (FULL_SCREEN)
	{
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = static_cast<unsigned long>(screenWidth);
		dmScreenSettings.dmPelsHeight = static_cast<unsigned long>(screenHeight);
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// 풀스크린에 맞는 디스플레이 설정을 합니다.
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// 윈도우의 위치를 화면의 왼쪽 위로 맞춥니다.
		posX = posY = 0;

	}
	else
	{
		// 윈도우 모드라면 800X600으로 크기 설정
		screenWidth = 800;
		screenHeight = 600;

		// 창을 모니터의 중앙에 오도록 설정
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	// 설정한 것을 가지고 창을 만들고 그 핸들을 가져옵니다.
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
		posX, posY, screenWidth, screenHeight, nullptr, nullptr, m_hinstance, nullptr);

	// 윈도우를 화면에 표시하고 포커스를 줍니다.
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	// 마우스 커서 표시설정
	ShowCursor(true);

	return;

}

void SystemClass::ShutdownWindows()
{
	// 마우스 커서를 표시합니다.
	ShowCursor(true);

	// 풀스크린 모드를 빠져나올 때 디스플레이 설정을 바꿉니다.
	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(nullptr, 0);
	}

	// 창을 제거합니다.
	DestroyWindow(m_hwnd);
	m_hwnd = nullptr;

	// 어플리케이션 인스턴스를 제거한다.
	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = nullptr;

	// 이 클래스에 대한 외부포인터 참조를 제거
	ApplicationHandle = nullptr;

}

LRESULT WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
		// 윈도우가 제거되었는지 확인
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}

		// 윈도우가 닫히는지 확인한다.
		case WM_CLOSE:
		{
			PostQuitMessage(0);
		}

		// 다른 모든 메세지들은 system 클래스의 메세지 처리기에 전달합니다.
		default:
		{
			return SystemClass::ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
		}
	}
}
