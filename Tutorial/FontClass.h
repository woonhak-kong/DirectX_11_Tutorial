#pragma once
#ifndef _FONTCLASS_H_
#define _FONTCLASS_H_

class TextureClass;

class FontClass
{
private:
	struct FontType
	{
		float left, right;
		int size;
	};

	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
	};

public:
	FontClass();
	FontClass(const FontClass& other);
	~FontClass();

	bool Initialize(ID3D11Device* device, const char* fontFilename, const WCHAR* textureFilename);
	void Shutdown();

	ID3D11ShaderResourceView* GetTexture();

	void BuildVertexArray(void* vertices, const char* sentence, float drawX, float drawY);

private:
	bool LoadFontData(const char* filename);
	void ReleaseFontData();
	bool LoadTexture(ID3D11Device* device, const WCHAR* filename);
	void ReleaseTexture();

private:
	FontType* m_Font = nullptr;
	TextureClass* m_Texture = nullptr;

};



#endif // _FONTCLASS_H_

