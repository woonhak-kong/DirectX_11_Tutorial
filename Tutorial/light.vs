// Globals

cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};


// Typedefs

struct VertexInputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};


// Vertex Shader

PixelInputType LightVertexShader(VertexInputType input)
{
	PixelInputType output;

	// 올바르게 행렬 연산을 하기 위하여 position 벡터를 w까지 있는 4성분이 있는 것으로 사용한다.
	input.position.w = 1.0f;

	// 정점의 위치를 월드, 뷰, 사영의 순으로 계산.
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// 픽셀 쉐이더의 텍스처 좌표를 저장한다.
	output.tex = input.tex;

	// 월드 행렬에 대해서만 법선 벡터를 계산한다.
	output.normal = mul(input.normal, (float3x3)worldMatrix);

	output.normal = normalize(output.normal);

	return output;
}