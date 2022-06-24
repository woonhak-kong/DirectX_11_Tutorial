#pragma once
#ifndef __MODELCLASS_H_
#define __MODELCLASS_H_


class TextureClass;

class ModelClass
{
private:
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
		XMFLOAT3 normal;
	};

	struct ModelType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

public:
	ModelClass();
	ModelClass(const ModelClass& other);
	~ModelClass();

	bool Initialize(ID3D11Device* device, const char* modelFilename, const WCHAR* textureFilename);
	void Shutdown();
	void Render(ID3D11DeviceContext* deviceContext);

	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture();

private:
	bool InitializeBuffers(ID3D11Device* device);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

	// 텍스쳐 로드, 반환
	bool LoadTexture(ID3D11Device* device, const WCHAR* filename);
	void ReleaseTexture();

	// 모델데이터 로드, 반환
	bool LoadModel(const char* modelFilename);
	void ReleaseModel();


private:
	ID3D11Buffer* m_vertexBuffer = nullptr;
	ID3D11Buffer* m_indexBuffer = nullptr;
	int m_vertexCount = 0;
	int m_indexCount = 0;

	TextureClass* m_texture = nullptr;

	ModelType* m_model = nullptr;
};


#endif //__MODELCLASS_H_

