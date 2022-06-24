#include "stdafx.h"
#include "graphicsclass.h"
#include "D3dclass.h"
#include "CameraClass.h"
#include "ModelClass.h"
#include "LightShaderClass.h"
#include "LightClass.h"

GraphicsClass::GraphicsClass()
{
}

GraphicsClass::GraphicsClass(const GraphicsClass& other)
{
}

GraphicsClass::~GraphicsClass()
{
}

bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	// Direct3D 객체 생성
	m_D3D = new D3DClass;
	if (!m_D3D)
	{
		return false;
	}

	// Direct3D 객체를 초기화 한다.
	if (!m_D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR))
	{
		MessageBox(hwnd, L"Could not initialize Direct3D.", L"Error", MB_OK);
		return false;
	}


	// m_Camera 객체 생성
	m_Camera = new CameraClass;
	if (!m_Camera)
	{
		return false;
	}

	// 카메라 포지션 설정
	m_Camera->SetPosition(0.0f, 0.0f, -5.0f);

	// m_Model 객체 생성
	m_Model = new ModelClass;
	if (!m_Model)
	{
		return false;
	}

	// m_Model 초기화
	if (!m_Model->Initialize(m_D3D->GetDevice(), L"./Textures/WoodCrate01.dds"))
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
	}

	// 텍스쳐 쉐이더 객체 생성
	//m_TextureShader = new TextureShaderClass;
	//if (!m_TextureShader)
	//{
	//	return false;
	//}

	//// 텍스터 쉐이더 객테 초기화
	//if (!m_TextureShader->Initialize(m_D3D->GetDevice(), hwnd))
	//{
	//	MessageBox(hwnd, L"Could not initialize texture shader object.", L"Error", MB_OK);
	//	return false;
	//}

	// LightShaderClass 객체 생성
	m_LightShader = new LightShaderClass;
	if (!m_LightShader)
	{
		return false;
	}

	// LightShader 객체를 초기화한다.
	if (!m_LightShader->Initialize(m_D3D->GetDevice(), hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
		return false;
	}

	// LightClass 객체 생성
	m_Light = new LightClass;
	if (!m_Light)
	{
		return false;
	}

	// Light 객체 초기화
	m_Light->SetDiffuseColor(1.0f, 0.0f, 1.0f, 1.0f);
	m_Light->SetDirection(0.0f, 0.0f, 1.0f);

	return true;
}

void GraphicsClass::Shutdown()
{
	// light 객체 해제
	if (m_Light)
	{
		delete m_Light;
		m_Light = nullptr;
	}

	// LightShader 객체 해제
	if (m_LightShader)
	{
		m_LightShader->Shutdown();
		delete m_LightShader;
		m_LightShader = nullptr;
	}

	// m_TextureShader 객체 반환
	//if (m_TextureShader)
	//{
	//	m_TextureShader->Shutdown();
	//	delete m_TextureShader;
	//	m_TextureShader = nullptr;
	//}

	// m_Model 객체 반환
	if (m_Model)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = nullptr;
	}

	// m_Camera 객체 반환
	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = nullptr;
	}

	// D3D 객체를 반환합니다.
	if (m_D3D)
	{
		m_D3D->Shutdown();
		delete m_D3D;
		m_D3D = nullptr;
	}
}

bool GraphicsClass::Frame()
{
	static float rotation = 0.0f;

	// 각 프레임 마다 rotation 변수값을 업데이트 한다.
	rotation += XM_PI * 0.01f;
	if (rotation > 360.0f)
	{
		rotation -= 360.0f;
	}
	//그래픽 렌더링을 수행합니다.
	if (!Render(rotation))
	{
		
		return false;
	}
	return true;
}

bool GraphicsClass::Render(float rotation)
{
	// 씬 그리기를 시작하기 위해 버퍼의 내용을 지웁니다.
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// 카메라의 위치에 따라 뷰 행렬을 생성한다.
	m_Camera->Render();

	// 카메라 및 d3d 객체에서 월드, 뷰 및 투영 행렬을 가져온다.
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	m_D3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	// 도형이 회전 할 수 있도록 회전 값으로 월드 행렬을 회전한다.
	worldMatrix = XMMatrixRotationY(rotation);

	// 모델 버텍스와 인덱스 버퍼를 그래픽 파이프 라인에 배치하여 드로잉을 준비한다.
	m_Model->Render(m_D3D->GetDeviceContext());


	// Light 쉐이더를 사용하여 모델을 렌더링 한다.
	if (!m_LightShader->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix,
		viewMatrix, projectionMatrix, m_Model->GetTexture(), m_Light->GetDirection(), m_Light->GetDiffuseColor()))
	{
		return false;
	}


	// 텍스처 쉐이더를 사용하여 모델을 랜더링 한다.
	//if (!m_TextureShader->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
	//	m_Model->GetTexture()))
	//{
	//	return false;
	//}
	

	// 버퍼에 그려진 씬을 화면에 표시합니다.
	m_D3D->EndScene();

	return true;
}
