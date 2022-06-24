#include "stdafx.h"
#include "ModelClass.h"
#include "TextureClass.h"

#include <fstream>

ModelClass::ModelClass()
{
}

ModelClass::ModelClass(const ModelClass& other)
{
}

ModelClass::~ModelClass()
{
}

bool ModelClass::Initialize(ID3D11Device* device, const char* modelFilename, const WCHAR* textureFilename)
{
	// 모델 데이터를 로드한다.
	if (!LoadModel(modelFilename))
	{
		return false;
	}

	// 정점 및 인덱스 버퍼를 초기화합니다.
	if (!InitializeBuffers(device))
	{
		return false;
	}

	// 이 모델의 텍스처를 로드한다.
	return LoadTexture(device, textureFilename);
}

void ModelClass::Shutdown()
{
	// 모델 텍스처 반환
	ReleaseTexture();

	//버텍스 및 인덱스 버퍼 종료
	ShutdownBuffers();

	// 모델 데이타 반환
	ReleaseModel();
}

void ModelClass::Render(ID3D11DeviceContext* deviceContext)
{
	// 그리기를 준비하기 위해 그래픽 파이프 라인에 꼭지점과 인덱스 버퍼를 놓는다.
	RenderBuffers(deviceContext);
}

int ModelClass::GetIndexCount()
{
	return m_indexCount;
}

ID3D11ShaderResourceView* ModelClass::GetTexture()
{
	return m_texture->GetTexture();
}

bool ModelClass::LoadTexture(ID3D11Device* device, const WCHAR* filename)
{
	// 텍스쳐클래스 객체 생성
	m_texture = new TextureClass;


	if (!m_texture)
	{
		return false;
	}

	// 객체 초기화
	return m_texture->Initialize(device, filename);
}

void ModelClass::ReleaseTexture()
{
	// 텍스쳐 클래스 객체 반환
	if (m_texture)
	{
		m_texture->Shutdown();
		delete m_texture;
		m_texture = nullptr;
	}
}

bool ModelClass::LoadModel(const char* modelFilename)
{
	// 모델 파일 열기
	std::ifstream fin;
	fin.open(modelFilename);

	//파일을 열 수 없으면 종료
	if (fin.fail())
	{
		return false;
	}

	// 버텍스 카운트의 값까지 읽는다.
	char input = 0;
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	// 버텍스 카운트를 읽는다.
	fin >> m_vertexCount;

	// 인덱스의 수를 정점 수와 같게 설정한다.
	m_indexCount = m_vertexCount;

	// 읽어 들인 정점 개수를 사용하여 모델을 만든다.
	m_model = new ModelType[m_vertexCount];
	if (!m_model)
	{
		return false;
	}

	// 데이터의 시작 부분까지 읽는다.
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}
	fin.get(input);
	fin.get(input);

	// 버텍스 데이터를 읽는다.
	for (int i = 0; i < m_vertexCount; i++)
	{
		fin >> m_model[i].x >> m_model[i].y >> m_model[i].z;
		fin >> m_model[i].tu >> m_model[i].tv;
		fin >> m_model[i].nx >> m_model[i].ny >> m_model[i].nz;
	}

	// 모델 파일을 닫는다.
	fin.close();

	return true;
}

void ModelClass::ReleaseModel()
{
	if (m_model)
	{
		delete[] m_model;
		m_model = nullptr;
	}
	
}

bool ModelClass::InitializeBuffers(ID3D11Device* device)
{
	// 정점 배열의 정점 수를 설정합니다.
	//m_vertexCount = 4;

	// 인덱스 배열의 인덱스 수를 설정합니다.
	//m_indexCount = 6;

	// 정점 배열을 만든다.
	VertexType* vertices = new VertexType[m_vertexCount];
	if(!vertices)
	{
		return false;
	}

	// 인덱스 배열을 만듭니다.
	unsigned long* indices = new unsigned long[m_indexCount];
	if(!indices)
	{
		return false;
	}

	// 정점 배열과 인덱스 배열을 더이상 직접 입력하지 않고 데이터로 읽어 온다.
	for (int i = 0; i < m_vertexCount; i++)
	{
		vertices[i].position = XMFLOAT3(m_model[i].x, m_model[i].y, m_model[i].z);
		vertices[i].texture= XMFLOAT2(m_model[i].tu, m_model[i].tv);
		vertices[i].normal= XMFLOAT3(m_model[i].nx, m_model[i].ny, m_model[i].nz);

		indices[i] = i;
	}


	// 삼각형
	//vertices[0].position = XMFLOAT3(-1.0f, -1.0f, 0.0f); // bottom left.
	//vertices[0].color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);

	//vertices[1].position = XMFLOAT3(0.0f, 1.0f, 0.0f); // Top middle
	//vertices[1].color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);

	//vertices[2].position = XMFLOAT3(1.0f, -1.0f, 0.0f); // bottom right.
	//vertices[2].color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);


	// 사각형
	//vertices[0].position = XMFLOAT3(-1.0f, -1.0f, 0.0f); // bottom left.
	//vertices[0].texture = XMFLOAT2(0.0f, 1.0f);
	//vertices[0].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

	//vertices[1].position = XMFLOAT3(-1.0f, 1.0f, 0.0f); // Top left
	//vertices[1].texture = XMFLOAT2(0.0f, 0.0f);
	//vertices[1].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

	//vertices[2].position = XMFLOAT3(1.0f, 1.0f, 0.0f); // top right.
	//vertices[2].texture = XMFLOAT2(1.0f, 0.0f);
	//vertices[2].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

	//vertices[3].position = XMFLOAT3(1.0f, -1.0f, 0.0f); // bottom right.
	//vertices[3].texture = XMFLOAT2(1.0f, 1.0f);
	//vertices[3].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

	// 인덱스 배열의 값을 설정한다.
	// 삼각형
	//indices[0] = 0; // Bottom left;
	//indices[1] = 1; // Top middle;
	//indices[2] = 2; // Bottom right;

	// 사각형
	//indices[0] = 0; // Bottom left;
	//indices[1] = 1; // Top left;
	//indices[2] = 2; // Top right;
	//indices[3] = 2; // Top right;
	//indices[4] = 3; // Bottom right;
	//indices[5] = 0; // Bottom left;


	// 정점 버퍼의 description 작성.
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// 정점 데이터를 가리키는 보조 리소스 구조체를 작성한다.
	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// 이제 정점 버퍼를 만듭니다.
	if(FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer)))
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
	if(FAILED(device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer)))
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

void ModelClass::ShutdownBuffers()
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

void ModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
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
