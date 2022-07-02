#pragma once

#ifndef _BITMAPCLASS_H_
#define _BITMAPCLASS_H_

class TextureClass;

class BitmapClass
{
public:
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
	};

	BitmapClass();
	BitmapClass(const BitmapClass& other);
	~BitmapClass();


	bool Initialize(ID3D11Device* device, int screenWidth, int screenHeight, const WCHAR* textureFilename, int bitmapWidth, int bitmapHeight);
	void Shutdown();
	bool Render(ID3D11DeviceContext* deviceContext, int positionX, int positionY);

	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture();


private:

	bool InitializeBuffers(ID3D11Device* device);
	void ShutdownBuffers();
	bool UpdateBuffers(ID3D11DeviceContext* deviceContext, int positionX, int positionY);
	void RenderBuffers(ID3D11DeviceContext* deviceContext);

	// 텍스쳐 로드, 반환
	bool LoadTexture(ID3D11Device* device, const WCHAR* filename);
	void ReleaseTexture();
	
private:
	ID3D11Buffer* m_vertexBuffer = nullptr;
	ID3D11Buffer* m_indexBuffer = nullptr;
	int m_vertexCount = 0;
	int m_indexCount = 0;

	TextureClass* m_texture = nullptr;

	int m_screenWidth = 0;
	int m_screenHeight = 0;
	int m_bitmapWidth = 0;
	int m_bitmapHeight = 0;
	int m_previousPosX = 0;
	int m_previousPosY = 0;

};



#endif // _BITMAPCLASS_H_