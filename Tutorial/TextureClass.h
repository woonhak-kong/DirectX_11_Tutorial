#pragma once
#ifndef _TEXTURECLASS_H_
#define _TEXTURECLASS_H_


class TextureClass
{
public:
	TextureClass();
	TextureClass(const TextureClass& other);
	~TextureClass();

	bool Initialize(ID3D11Device* device, const WCHAR* filename);
	void Shutdown();

	ID3D11ShaderResourceView* GetTexture();

private:

	ID3D11Resource* m_texture = nullptr;
	ID3D11ShaderResourceView* m_textureView = nullptr;

};


#endif //_TEXTURECLASS_H_