#include "stdafx.h"
#include "LightShaderClass.h"

LightShaderClass::LightShaderClass()
{
}

LightShaderClass::LightShaderClass(const LightShaderClass& other)
{
}

LightShaderClass::~LightShaderClass()
{
}

bool LightShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	// 정점 및 픽셀 쉐이더 초기화
	return InitializeShader(device, hwnd, L"./light.vs", L"./light.ps");
}

void LightShaderClass::Shutdown()
{
	// 버텍스 및 픽셀 쉐이더와 관련되 객체 종료
	ShutdownShader();
}

bool LightShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture,
	XMFLOAT3 lightDirection, XMFLOAT4 ambientColor, XMFLOAT4 diffuseColor, XMFLOAT3 cameraPosition, XMFLOAT4 specularColor, float specularPower)
{
	// 렌더링에 사용할 쉐이더 매개 변수를 설정
	if (!SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture, lightDirection, ambientColor, diffuseColor,
		cameraPosition, specularColor, specularPower))
	{
		return false;
	}

	// 설정된 버퍼를 쉐이더로 랜더링 한다.
	RenderShader(deviceContext, indexCount);
	return true;
}

bool LightShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, const WCHAR* vsFilename, const WCHAR* psFilename)
{
	ID3D10Blob* errorMessage = nullptr;

	// 버텍스 쉐이더 코드를 컴파일 한다.
	ID3D10Blob* vertexShaderBuffer = nullptr;
	if (FAILED(D3DCompileFromFile(vsFilename, nullptr, nullptr, "LightVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&vertexShaderBuffer, &errorMessage)))
	{
		// 쉐이더 컴파일 실패시 오류메시지 출력.
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		// 컴파일 오류가 아니라면 쉐이더 파일을 찾울 수 없는 경우이다.
		else
		{
			MessageBox(hwnd, vsFilename, L"Missing VertexShader Shader File", MB_OK);
		}

		return false;
	}

	// 픽셀 쉐이더 코드를 컴파일 한다.
	ID3D10Blob* pixelShaderBuffer = nullptr;
	if (FAILED(D3DCompileFromFile(psFilename, nullptr, nullptr, "LightPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&pixelShaderBuffer, &errorMessage)))
	{
		// 쉐이더 컴파일 실패시 오류메시지 출력.
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		}
		// 컴파일 오류가 아니라면 쉐이더 파일을 찾울 수 없는 경우이다.
		else
		{
			MessageBox(hwnd, psFilename, L"Missing PixelShade Shader File", MB_OK);
		}

		return false;
	}

	// 버퍼로부터 정점 쉐이더를 생성한다.
	if (FAILED(device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(),
		nullptr, &m_vertexShader)))
	{
		return false;
	}

	// 버퍼에서 픽셀 쉐이더를 생성한다.
	if (FAILED(device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(),
		nullptr, &m_pixelShader)))
	{
		return false;
	}

	// 정점 입력 레이아웃 구조체를 설정합니다.
	// 이 설정은 ModelClass와 쉐이더의 VertexType 구조와 일치해야 한다.
	D3D11_INPUT_ELEMENT_DESC polygonLayout[3];
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	polygonLayout[2].SemanticName = "NORMAL";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;

	// 레이아웃의 요소 수를 가져옵니다.
	unsigned int numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// 정점 입력 레이아웃을 만듭니다.
	if (FAILED(device->CreateInputLayout(polygonLayout, numElements,
		vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_layout)))
	{
		return false;
	}

	// 더 이상 사용되지 않는 정점 쉐이더 버퍼와 픽셀 쉐이더 버퍼를 해제한다.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = nullptr;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = nullptr;

	// 정점 쉐이더에 있는 행렬 상수 버퍼의 구조체를 작성한다.
	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// 상수 버퍼 포인터를 만들어 이 클래스에서 정점 쉐이더 상수 버퍼에 접근할 수 있게 한다.
	if (FAILED(device->CreateBuffer(&matrixBufferDesc, nullptr, &m_matrixBuffer)))
	{
		return false;
	}

	// 텍스처 샘플러 상태 구조체를 생성 및 설정한다.
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// 텍스처 샘플러 상태를 만듭니다.
	if (FAILED(device->CreateSamplerState(&samplerDesc, &m_sampleState)))
	{
		return false;
	}


	// 버텍스 쉐이더에있는 카메라 동적 상수 버퍼의 설명을 설정
	D3D11_BUFFER_DESC cameraBufferDesc;
	cameraBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cameraBufferDesc.ByteWidth = sizeof(CameraBufferType);
	cameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cameraBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cameraBufferDesc.MiscFlags = 0;
	cameraBufferDesc.StructureByteStride = 0;

	// 이 클래스 내에서 정점 쉐이더 상수 버퍼에 엑세스 할 수 있도록 카메라 상수 버퍼 포인터를 만든다.
	if (FAILED(device->CreateBuffer(&cameraBufferDesc, nullptr, &m_cameraBuffer)))
	{
		return false;
	}


	// 픽셀 쉐이더에있는 광원 동적 상수 버퍼의 설명을 설정합니다.
	// D3D11_BIND_CONSTANT_BUFFER를 사용하면 Bytewidth가 항상 16배수 어야하며 그렇지 않으면, CreateBuffer가 실패한다.
	D3D11_BUFFER_DESC lightBufferDesc;
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	// 이 클래스 내에서 정점 쉐이더 상수 버퍼에 엑세스 할 수 있도록 상수 버퍼 포인터를 만듭니다.
	if (FAILED(device->CreateBuffer(&lightBufferDesc, nullptr, &m_lightBuffer)))
	{
		return false;
	}

	return true;

}

void LightShaderClass::ShutdownShader()
{
	// 카메라 상수 버퍼 해제
	if (m_cameraBuffer)
	{
		m_cameraBuffer->Release();
		m_cameraBuffer = nullptr;
	}

	// light constant 버퍼 해제
	if (m_lightBuffer)
	{
		m_lightBuffer->Release();
		m_lightBuffer = nullptr;
	}

	// 샘플러 상태 해제
	if (m_sampleState)
	{
		m_sampleState->Release();
		m_sampleState = nullptr;
	}

	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = nullptr;
	}

	if (m_layout)
	{
		m_layout->Release();
		m_layout = nullptr;
	}

	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = nullptr;
	}

	if (m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = nullptr;
	}
}

void LightShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, const WCHAR* shaderFilename)
{
	// 에러 메시지를 출력창에 표시한다.
	OutputDebugStringA(reinterpret_cast<const char*>(errorMessage->GetBufferPointer()));

	// 에러 메시지를 반환 한다.
	errorMessage->Release();
	errorMessage = nullptr;

	// 컴파일 에러가 있음을 팝업 메세지를 알려준다.
	MessageBox(hwnd, L"Error compiling shader.", shaderFilename, MB_OK);
}

bool LightShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix,
	ID3D11ShaderResourceView* texture, XMFLOAT3 lightDirection, XMFLOAT4 ambientColor, XMFLOAT4 diffuseColor, XMFLOAT3 cameraPosition, XMFLOAT4 specularColor,
	float specularPower)
{
	// 행렬을 transpose하여 쉐이더에서 사용할 수 있게 한다.
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	// 상수 버퍼의 내용을 쓸 수 있도록 잠근다.
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if (FAILED(deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		return false;
	}

	// 상수 버퍼의 데이터에 대한 포인터를 가져온다.
	MatrixBufferType* dataPtr = reinterpret_cast<MatrixBufferType*>(mappedResource.pData);

	// 상수 버퍼에 행렬을 복사한다.
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	// 상수 버퍼의 잠금을 푼다.
	deviceContext->Unmap(m_matrixBuffer, 0);

	// 정점 쉐이더에서의 상수 버퍼의 위치를 설정한다.
	unsigned bufferNumber = 0;

	// 마지막으로 정점 쉐이더의 상수 버퍼를 바뀐 값으로 바꾼다.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	// 쓸 수 있도록 카메라 상수 버퍼를 잠급니다.
	if (FAILED(deviceContext->Map(m_cameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0,&mappedResource)))
	{
		return false;
	}

	CameraBufferType* dataPtr3 = reinterpret_cast<CameraBufferType*>(mappedResource.pData);

	// 카메라 위치를 상수 버퍼에 복사한다.
	dataPtr3->cameraPosition = cameraPosition;
	dataPtr3->padding = 0.0f;

	// 카메라 상수 버퍼를 잠금 해제한다.
	deviceContext->Unmap(m_cameraBuffer, 0);

	// 버텍스 쉐이더에서 카메라 상수 버퍼의 위치를 설정한다.
	bufferNumber = 1;

	// 이제 업데이트 된 값으로 버텍스 쉐이더에서 카메라 상수 버퍼를 설정한다.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_cameraBuffer);

	// 픽셀 쉐이더에서 쉐이더 텍스처 리소스를 설정한다.
	deviceContext->PSSetShaderResources(0, 1, &texture);

	// light constant buffer를 잠글 수 있도록 기록한다.
	if(FAILED(deviceContext->Map(m_lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		return false;
	}

	// 상수 버퍼의 데이터에 대한 포인터를 가져온다.
	LightBufferType* dataPtr2 = reinterpret_cast<LightBufferType*>(mappedResource.pData);

	// 빛 변수들을 상수버퍼에 넣는다.
	dataPtr2->ambientColor = ambientColor;
	dataPtr2->diffuseColor = diffuseColor;
	dataPtr2->lightDirection = lightDirection;
	dataPtr2->specularColor = specularColor;
	dataPtr2->specularPower = specularPower;

	// 상수 버퍼의 잠금을 해제한다.
	deviceContext->Unmap(m_lightBuffer, 0);

	// 픽셀 쉐이더에서 광원 상수 버퍼의 위치를 설정한다.
	bufferNumber = 0;

	// 마지막으로 업데이트 된 값으로 픽셀 쉐이더에서 광원 상수 버퍼를 설정한다.
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_lightBuffer);

	return true;
}

void LightShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// 정점 입력 레이아웃을 설정한다.
	deviceContext->IASetInputLayout(m_layout);

	// 삼각형을 그릴 정점 쉐이더와 픽셀 쉐이더를 설정한다.
	deviceContext->VSSetShader(m_vertexShader, nullptr, 0);
	deviceContext->PSSetShader(m_pixelShader, nullptr, 0);

	// 픽셀 쉐이더에서 샘플러 상태를 설정한ㄷ.ㅏ
	deviceContext->PSSetSamplers(0, 1, &m_sampleState);

	// 삼각형을 그린다.
	deviceContext->DrawIndexed(indexCount, 0, 0);
}
