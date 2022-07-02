#include "stdafx.h"
#include "BitmapClass.h"

#include "TextureClass.h"

BitmapClass::BitmapClass()
{
}

BitmapClass::BitmapClass(const BitmapClass& other)
{
}

BitmapClass::~BitmapClass()
{
}

bool BitmapClass::Initialize(ID3D11Device* device, int screenWidth, int screenHeight, const WCHAR* textureFilename, int bitmapWidth, int bitmapHeight)
{
	// 화면 크기를 멤버변수에 저장
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	// 렌더링할 비트맵의 픽셀의 크기를 저장
	m_bitmapWidth = bitmapWidth;
	m_bitmapHeight = bitmapHeight;

	// 이전 렌더링 위치를 음수로 초기화 한다.
	m_previousPosX = -1;
	m_previousPosY = -1;

	// 정점 및 인덱스 버퍼를 초기화 한다.
	if (!InitializeBuffers(device))
	{
		return false;
	}

	// 이 모델의 텍스처를 로드한다.
	return LoadTexture(device, textureFilename);

}

void BitmapClass::Shutdown()
{
	// 모델 텍스쳐를 반환한다.
	ReleaseTexture();

	// 버텍스 및 인덱스 버퍼를 종료한다.
	ShutdownBuffers();
}

bool BitmapClass::Render(ID3D11DeviceContext* deviceContext, int positionX, int positionY)
{
	// 화면의 다른 위치로 렌더링하기 위해 동적 정점 버퍼를 다시 빌드한ㄷ.
	if (!UpdateBuffers(deviceContext, positionX, positionY))
	{
		return false;
	}

	// 그리기를 준비하기 위해 그래픽 파이프 라인에 꼭지점과 인덱스 버퍼를 놓는다.
	RenderBuffers(deviceContext);

	return true;
}

int BitmapClass::GetIndexCount()
{
	return m_indexCount;
}

ID3D11ShaderResourceView* BitmapClass::GetTexture()
{
	return m_texture->GetTexture();
}

bool BitmapClass::InitializeBuffers(ID3D11Device* device)
{
	// 정점 배열의 정점 수와 인덱스 배열의 인덱스 수를 지정한다.
	m_indexCount = m_vertexCount = 6;

	// 정점 배열을 만든다.
	VertexType* vertices = new VertexType[m_vertexCount];
	if (!vertices)
	{
		return false;
	}


	// 정점 배열을 0으로 초기화 한다.
	memset(vertices, 0, (sizeof(VertexType) * m_vertexCount));

	// 인덱스 배열을 만든다.
	unsigned long* indices = new unsigned long[m_indexCount];
	if (!indices)
	{
		return false;
	}

	// 데이터로 인덱스 배열을 로드한다.
	for (int i = 0; i < m_vertexCount; i++)
	{
		indices[i] = i;
	}


	// 정적 정점 버퍼의 구조체를 설정한다.
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// subresource 구조에 정점 데이터에 대한 포인터를 제공한다.
	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// 이제 정점 버퍼를 만듭니다.
	if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer)))
	{
		return false;
	}

	// 인덱스 버퍼 description 생성
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// 인덱스 데이터를 가리키는 보조 리소스 구조체 작성
	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// 인덱스 버퍼를 생성합니다.
	if (FAILED(device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer)))
	{
		return false;
	}

	// 정점버퍼 인덱스 버퍼 해제
	delete[] vertices;
	vertices = nullptr;

	delete[] indices;
	indices = nullptr;

	return true;
}

void BitmapClass::ShutdownBuffers()
{
	// 인덱스 버퍼 해제
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = nullptr;
	}

	// 정점 버퍼를 해제합니다.
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_indexBuffer = nullptr;
	}
}

bool BitmapClass::UpdateBuffers(ID3D11DeviceContext* deviceContext, int positionX, int positionY)
{
	float left, right, top, bottom;
	VertexType* vertices;
	VertexType* verticesPtr;
	D3D11_MAPPED_SUBRESOURCE mappedResource;


	// 이 비트맵을 렌더링 할 위치가 변경되지 않은 경우 정점 버퍼를 업데이트 하지 않는다.
	if ((positionX == m_previousPosX) && (positionY == m_previousPosY))
	{
		return true;
	}

	// 변경된 경우 렌더링 되는 위치를 업데이트 한다.
	m_previousPosX = positionX;
	m_previousPosY = positionY;

	// 비트 맵 왼쪽의 화면 좌표를 계산한다.
	left = static_cast<float>((m_screenWidth / 2) * -1) + static_cast<float>(positionX);

	// 비트맵 오른쪽의 화면 좌표를 계산한다.
	right = left + static_cast<float>(m_bitmapWidth);

	// 비트맵 상단의 화면 좌표를 계산한다.
	top = static_cast<float>(m_screenHeight / 2) - static_cast<float>(positionY);

	// 비트 맵 아래쪽의 화면 좌표를 계산한다.
	bottom = top - static_cast<float>(m_bitmapHeight);


	// 정점 배열을 만든다.
	vertices = new VertexType[m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	// 정점 배열에 데이터를 로드한다.
	// 첫 번째 삼각형
	vertices[0].position = XMFLOAT3(left, top, 0.0f);
	vertices[0].texture = XMFLOAT2(0.0f, 0.0f);

	vertices[1].position = XMFLOAT3(right, bottom, 0.0f);
	vertices[1].texture = XMFLOAT2(1.0f, 1.0f);

	vertices[2].position = XMFLOAT3(left, bottom, 0.0f);
	vertices[2].texture = XMFLOAT2(0.0f, 1.0f);


	// 두번째 삼각형
	vertices[3].position = XMFLOAT3(left, top, 0.0f);
	vertices[3].texture = XMFLOAT2(0.0f, 0.0f);

	vertices[4].position = XMFLOAT3(right, top, 0.0f);
	vertices[4].texture = XMFLOAT2(1.0f, 0.0f);

	vertices[5].position = XMFLOAT3(right, bottom, 0.0f);
	vertices[5].texture = XMFLOAT2(1.0f, 1.0f);

	// 버텍스 버퍼를 쓸 수 있도록 잠근다.
	if (FAILED(deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		return false;
	}

	// 정점 버퍼의 데이터를 가리키는 포인터를 얻는다.
	verticesPtr = reinterpret_cast<VertexType*>(mappedResource.pData);

	// 데이터를 정점 버퍼에 복사한다.
	memcpy(verticesPtr, vertices, (sizeof(VertexType) * m_vertexCount));
	
	// 정점 버퍼의 잠금을 해제한다.
	deviceContext->Unmap(m_vertexBuffer, 0);

	// 더이상 필요하지 않은 꼭지점 배열 해제
	delete[] vertices;
	vertices = 0;

	return true;
}

void BitmapClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	// 정점 버퍼의 단위와 오프셋을 설정한다.
	stride = sizeof(VertexType);
	offset = 0;

	// input assembler에 정점 버퍼를 활성화하여 그려질 수 있게 한다.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// input Assembler에 인덱스 버퍼를 활성화여 그려질 수 있게한다.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// 정점 버퍼로 그릴 기본형을 설정한다.
	// 여기서는 삼각형이다.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

}

bool BitmapClass::LoadTexture(ID3D11Device* device, const WCHAR* filename)
{
	// 텍스쳐 오브젝트를 생성한다.
	if (!m_texture)
	{
		return false;
	}

	// 텍스처 오브젝트를 초기화한다.
	return m_texture->Initialize(device, filename);
}

void BitmapClass::ReleaseTexture()
{
	// 텍스쳐 오브젝트를 릴리즈 한다.
	if (m_texture)
	{
		m_texture->Shutdown();
		delete m_texture;
		m_texture = nullptr;
	}
	
}
