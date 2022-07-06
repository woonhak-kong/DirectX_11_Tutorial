#include "stdafx.h"
#include "FontClass.h"

#include <fstream>

#include "TextureClass.h"

FontClass::FontClass()
{
}

FontClass::FontClass(const FontClass& other)
{
}

FontClass::~FontClass()
{
}

bool FontClass::Initialize(ID3D11Device* device, const char* fontFilename, const WCHAR* textureFilename)
{
	// 폰트 데이터 로드
	if (!LoadFontData(fontFilename))
	{
		return false;
	}

	// 폰트 문자들을 가지고 있는 텍스쳐 로드
	return LoadTexture(device, textureFilename);
}

void FontClass::Shutdown()
{
	// 폰트 텍스쳐 해제
	ReleaseTexture();

	// 폰트 데이타 해제
	ReleaseFontData();
}

ID3D11ShaderResourceView* FontClass::GetTexture()
{
	return m_Texture->GetTexture();
}

void FontClass::BuildVertexArray(void* vertices, const char* sentence, float drawX, float drawY)
{
	VertexType* vertexPtr;

	// vertices 를 VertexType 구조체로 강제 변경
	vertexPtr = reinterpret_cast<VertexType*>(vertices);

	// 문장의 문자수를 가져온다.
	int numLetters = static_cast<int>(strlen(sentence));

	// vertex 배열의 인덱스 초기화
	int index = 0;

	// 각 문자를 quad로 그린다.
	for (int i = 0; i < numLetters; i++)
	{
		// 문자의 번호
		int letter = static_cast<int>(sentence[i]) - 32;

		// 만약 문자가 단지 공간(스페이스)라면 단지 3픽세 이동시킨다.
		if (letter == 0)
		{
			drawX = drawX + 3.0f;
		}
		else
		{
			// 쿼드위의 첫번째 삼각형
			vertexPtr[index].position = XMFLOAT3(drawX, drawY, 0.0f); // Top left
			vertexPtr[index].texture = XMFLOAT2(m_Font[letter].left, 0.0f); // UV
			index++;

			vertexPtr[index].position = XMFLOAT3((drawX + m_Font[letter].size), (drawY - 16), 0.0f); // Bottom right
			vertexPtr[index].texture = XMFLOAT2(m_Font[letter].right, 1.0f); // UV
			index++;

			vertexPtr[index].position = XMFLOAT3(drawX, drawY - 16, 0.0f); // Bottom left
			vertexPtr[index].texture = XMFLOAT2(m_Font[letter].left, 1.0f); // UV
			index++;

			// 쿼드위의 두버째 삼각형
			vertexPtr[index].position = XMFLOAT3(drawX, drawY, 0.0f); // Top left
			vertexPtr[index].texture = XMFLOAT2(m_Font[letter].left, 0.0f); // UV
			index++;

			vertexPtr[index].position = XMFLOAT3((drawX + m_Font[letter].size), drawY, 0.0f); // Top right
			vertexPtr[index].texture = XMFLOAT2(m_Font[letter].right, 0.0f); // UV
			index++;

			vertexPtr[index].position = XMFLOAT3(drawX + m_Font[letter].size, drawY - 16, 0.0f); // Bottom right
			vertexPtr[index].texture = XMFLOAT2(m_Font[letter].right, 1.0f); // UV
			index++;

			// 다음 글자의 x 위치를 위하여 그 글자의 사이즈보다 1 픽셀 더 이동시킨다.
			drawX = drawX + m_Font[letter].size + 1.0f;
		}
	}

}

bool FontClass::LoadFontData(const char* filename)
{
	std::ifstream fin;
	int i;
	char temp;

	// 폰트버퍼 할당
	m_Font = new FontType[95];
	if (!m_Font)
	{
		return false;
	}


	// 폰트데이타 오픈
	fin.open(filename);
	if (fin.fail())
	{
		return false;
	}

	// 텍스트를 위한 95개의 문자 정보를 저장한다.
	for (i = 0; i < 95; i++)
	{
		fin.get(temp);
		while (temp != ' ')
		{
			fin.get(temp);
		}
		fin.get(temp);
		while (temp != ' ')
		{
			fin.get(temp);
		}

		fin >> m_Font[i].left;
		fin >> m_Font[i].right;
		fin >> m_Font[i].size;
	}

	// 파일을 닫는다.
	fin.close();

	return true;
}

void FontClass::ReleaseFontData()
{
	// 폰트 데이타 배열 해제
	if (m_Font)
	{
		delete[] m_Font;
		m_Font = nullptr;
	}
}

bool FontClass::LoadTexture(ID3D11Device* device, const WCHAR* filename)
{
	// 텍스쳐 객체 생성
	m_Texture = new TextureClass;
	if (!m_Texture)
	{
		return false;
	}

	// 텍스쳐 객체 초기화
	return m_Texture->Initialize(device, filename);
}

void FontClass::ReleaseTexture()
{
	// 텍스쳐 객체 해제
	if (m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = nullptr;
	}
}
