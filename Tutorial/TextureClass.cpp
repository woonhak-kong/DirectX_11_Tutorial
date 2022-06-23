#include "stdafx.h"
#include "TextureClass.h"

#include "DDSTextureLoader.h"

TextureClass::TextureClass()
{
}

TextureClass::TextureClass(const TextureClass& other)
{
}

TextureClass::~TextureClass()
{
}

bool TextureClass::Initialize(ID3D11Device* device, const WCHAR* filename)
{
	// 텍스쳐 로드

	if (FAILED(CreateDDSTextureFromFile(device, filename, &m_texture, &m_textureView)))
	{
		return false;
	}

	return true;
	
}

void TextureClass::Shutdown()
{
	// 텍스쳐 뷰 리로스 해제
	if (m_textureView)
	{
		m_textureView->Release();
		m_textureView = nullptr;
	}

	if (m_texture)
	{
		m_texture->Release();
		m_texture = nullptr;
	}

}

ID3D11ShaderResourceView* TextureClass::GetTexture()
{
	return m_textureView;
}
