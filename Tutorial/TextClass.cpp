#include "stdafx.h"
#include "TextClass.h"

#include "FontClass.h"
#include "FontShaderClass.h"

TextClass::TextClass()
{
}

TextClass::TextClass(const TextClass& other)
{
}

TextClass::~TextClass()
{
}

bool TextClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, HWND hwnd, int screenWidth, int screenHeight, XMMATRIX baseViewMatrix)
{

	// 화면 너비 높이 값 저장.
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	// baseViewMatrix 저장.
	m_baseViewMatrix = baseViewMatrix;

	// Font 객체 생성
	m_Font = new FontClass;
	if (!m_Font)
	{
		return false;
	}

	// Font 객체 초기화
	if (!(m_Font->Initialize(device, "./fontdata.txt", L"./Textures/font.dds")))
	{
		MessageBox(hwnd, L"Could not initialize the font object", L"Error", MB_OK);
		return false;
	}

	// 폰트 쉐이더 객체 생성
	m_FontShader = new FontShaderClass;
	if (!m_FontShader)
	{
		return false;
	}

	// 폰트쉐이더 객체 초기화
	if (!(m_FontShader->Initialize(device, hwnd)))
	{
		MessageBox(hwnd, L"Could not initialize the font shader object", L"Error", MB_OK);
		return false;
	}

	// 첫번째 문장 초기화
	if (!(InitializeSentence(&m_sentence1, 16, device)))
	{
		return false;
	}

	// 이젠 Sentence Vertex Buffer를 문장으로 업데이트 한다.
	if (!(UpdateSentence(m_sentence1, "Hello", 100, 100, 1.0f, 1.0f, 1.0f, deviceContext)))
	{
		return false;
	}

	// 두번째 문장 초기화
	if (!(InitializeSentence(&m_sentence2, 16, device)))
	{
		return false;
	}

	// 이젠 Sentence Vertex Buffer를 문장으로 업데이트 한다.
	if (!(UpdateSentence(m_sentence2, "Goodbye", 100, 200, 1.0f, 1.0f, 0.0f, deviceContext)))
	{
		return false;
	}

	return true;
}

void TextClass::Shutdown()
{
	// 문장 객체 해제
	ReleaseSentence(&m_sentence1);
	ReleaseSentence(&m_sentence2);

	// 폰트쉐이더 객체 해제
	if (m_FontShader)
	{
		m_FontShader->Shutdown();
		delete m_FontShader;
		m_FontShader = nullptr;
	}

	// 폰트 객체 해제
	if (m_Font)
	{
		m_Font->Shutdown();
		delete m_Font;
		m_Font = nullptr;
	}

}

bool TextClass::Render(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX orthoMatrix)
{
	bool result;

	// 첫번째 문자 그리기
	result = RenderSentence(deviceContext, m_sentence1, worldMatrix, orthoMatrix);
	if (!result)
	{
		return false;
	}

	// 두번째 문자 그리기
	result = RenderSentence(deviceContext, m_sentence2, worldMatrix, orthoMatrix);
	if (!result)
	{
		return false;
	}

	return true;
}

bool TextClass::InitializeSentence(SentenceType** sentence, int maxLength, ID3D11Device* device)
{
	// SentenceType 객체 생성
	*sentence = new SentenceType;
	if (!*sentence)
	{
		return false;
	}

	// 센텐스 버퍼를 null로 초기화
	(*sentence)->vertexBuffer = nullptr;
	(*sentence)->indexBuffer = nullptr;

	// 최대 길이 초기화
	(*sentence)->maxLength = maxLength;

	// 정점수 초기화
	(*sentence)->vertexCount = 6 * maxLength; // 한글자에 6정점 필요(삼각형 2개)

	// 인덱스의 수 초기화
	(*sentence)->indexCount = (*sentence)->vertexCount;

	// 버텍스 배열 생성
	VertexType* vertices = new VertexType[(*sentence)->vertexCount];
	if (!vertices)
	{
		return false;
	}

	// 인덱스 배열 생성
	unsigned long* indices = new unsigned long[(*sentence)->indexCount];
	if (!indices)
	{
		return false;
	}

	// 버텍스 어레이 0으로 초기화
	memset(vertices, 0, (sizeof(VertexType) * (*sentence)->vertexCount));

	// 인덱스 어레이 초기화
	for (int i = 0; i < (*sentence)->indexCount; i++)
	{
		indices[i] = i;
	}

	// 동적 정점 버퍼 디스크립션 설정
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * (*sentence)->vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Subresource 구조체에 버텍스 데이터 포인터를 할당.
	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// 버텍스 버퍼 생성
	bool result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &(*sentence)->vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// 정적 인덱스 버퍼의 디스크립션 작성
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * (*sentence)->indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Subresource 구조체에 인덱스 데이타 포인터 할당
	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// 인덱스 버퍼 생성
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &(*sentence)->indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// 더이상 사용하지 않는 버텍스, 인덱스 배열 할당 해제
	delete[] vertices;
	vertices = nullptr;
	delete[] indices;
	indices = nullptr;

	return true;
}

bool TextClass::UpdateSentence(SentenceType* sentence, const char* text, int positionX, int positionY, float red, float green, float blue , ID3D11DeviceContext* deviceContext)
{
	// 문장의 색상 저장
	sentence->red = red;
	sentence->green = green;
	sentence->blue = blue;

	// 문장의 길이 가져오기
	auto numLetters = static_cast<int>(strlen(text));

	// 버퍼의 오버플로우 확인
	if (numLetters > sentence->maxLength)
	{
		return false;
	}

	// 버텍스 어레이 생성
	auto vertices = new VertexType[sentence->vertexCount];
	if (!vertices)
	{
		return false;
	}

	// 버텍스 어레이 0으로 초기화
	memset(vertices, 0, sizeof(VertexType) * sentence->vertexCount);

	// X와 Y의 픽셀위치를 계산한다.
	auto drawX = static_cast<float>(((m_screenWidth / 2) * -1) + positionX);
	auto drawY = static_cast<float>((m_screenHeight / 2) - positionY);

	// 문장 텍스트와 문자 그리는 위치를 사용하여 버텍스 어레이를 빌드하기위해 폰트 클래스를 사용한다.
	m_Font->BuildVertexArray(reinterpret_cast<void*>(vertices), text, drawX, drawY);

	// 버텍스 버퍼를 쓰기 위해 잠근다.
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	bool result = deviceContext->Map(sentence->vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// 버텍스 버퍼의 데이터를 가르키는 포인터를 가져온다.
	auto verticesPtr = reinterpret_cast<VertexType*>(mappedResource.pData);

	// 데이타를 버텍스 버퍼로 복사한다.
	memcpy(verticesPtr, reinterpret_cast<void*>(vertices), sizeof(VertexType) * sentence->vertexCount);

	// 버텍스 버퍼의 잠금해제 한다.
	deviceContext->Unmap(sentence->vertexBuffer, 0);

	// 더이상 사용하지 않는 버텍스 어레이 할당해제
	delete[] vertices;
	vertices = nullptr;

	return true;
}

void TextClass::ReleaseSentence(SentenceType** sentence)
{
	if (*sentence)
	{
		// 문장 버텍스 버퍼 해제
		if ((*sentence)->vertexBuffer)
		{
			(*sentence)->vertexBuffer->Release();
			(*sentence)->vertexBuffer = nullptr;
		}

		// 문장 인덱스 버퍼 해제
		if (((*sentence)->indexBuffer))
		{
			(*sentence)->indexBuffer->Release();
			(*sentence)->indexBuffer = nullptr;
		}

		// sentence 해제
		delete* sentence;
		*sentence = nullptr;
	}
}

bool TextClass::RenderSentence(ID3D11DeviceContext* deviceContext, SentenceType* sentence, XMMATRIX worldMatrix, XMMATRIX orthoMatrix)
{
	// 버텍스 버퍼의 스트라이드와 오프셋을 설정
	unsigned int stride = sizeof(VertexType);
	unsigned int offset = 0;

	// 랜더될수 있도록 인풋 어셈블러에서 버텍스 버퍼가 활성화 되도록 설정한다.
	deviceContext->IASetVertexBuffers(0, 1, &sentence->vertexBuffer, &stride, &offset);

	// 인덱스 버퍼 활성화
	deviceContext->IASetIndexBuffer(sentence->indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// primitive 타입 설정, 여기서는 삼각형
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 픽셀 생상 벡터를 인풋된 문장 색상으로 만든다.
	auto pixelColor = XMFLOAT4(sentence->red, sentence->green, sentence->blue, 1.0f);

	// 폰트쉐이더를 이용하여 텍스트를 랜더한다.
	bool result = m_FontShader->Render(deviceContext, sentence->indexCount, worldMatrix, m_baseViewMatrix, orthoMatrix,
		m_Font->GetTexture(), pixelColor);
	if (!result)
	{
		return false;
	}
	return true;
}
