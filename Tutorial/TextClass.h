#pragma once
#ifndef _TEXTCLASS_H_
#define _TEXTCLASS_H_

class FontClass;
class FontShaderClass;


class TextClass
{
private:
	struct SentenceType
	{
		ID3D11Buffer* vertexBuffer, * indexBuffer;
		int vertexCount, indexCount, maxLength;
		float red, green, blue;
	};

	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
	};

public:
	TextClass();
	TextClass(const TextClass& other);
	~TextClass();

	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, HWND hwnd, int screenWidth, int screenHeight, XMMATRIX baseViewMatrix);
	void Shutdown();
	bool Render(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX orthoMatrix);
	bool SetMousePosition(int mouseX, int mouseY, ID3D11DeviceContext* deviceContext);

private:
	bool InitializeSentence(SentenceType** sentence, int maxLength, ID3D11Device* device);
	bool UpdateSentence(SentenceType* sentence, const char* text, int positionX, int positionY, float red, float green, float blue, ID3D11DeviceContext* deviceContext);
	void ReleaseSentence(SentenceType** sentence);
	bool RenderSentence(ID3D11DeviceContext* deviceContext, SentenceType* sentence, XMMATRIX worldMatrix, XMMATRIX orthoMatrix);

private:
	FontClass* m_Font = nullptr;
	FontShaderClass* m_FontShader = nullptr;
	int m_screenWidth = 0;
	int m_screenHeight = 0;
	XMMATRIX m_baseViewMatrix;
	SentenceType* m_sentence1 = nullptr;
	SentenceType* m_sentence2 = nullptr;

};


#endif // _TEXTCLASS_H_
