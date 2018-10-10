//#include "Common.hlsl"
//
//VertexOut VS(VertexIn vin)
//{
//	VertexOut vout = (VertexOut)0.0f;
//
//	MaterialData matData = gMaterialData_Default[0];
//
//	// Transform to world space.
//	float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);
//	vout.PosW = posW.xyz;
//
//	// Assumes nonuniform scaling; otherwise, need to use inverse-transpose of world matrix.
//	vout.NormalW = mul(vin.NormalL, (float3x3)gWorld);
//
//	// Transform to homogeneous clip space.
//	vout.PosH = mul(posW, gViewProj);
//
//	float4 texC = mul(float4(vin.TexC, 0.0f, 1.0f), gTexTransform);
//	vout.TexC = mul(texC, matData.MatTransform).xy;
//
//	return vout;
//}
//
#include "Common.hlsl"




VertexOut VS(VertexIn vin)
{
	VertexOut vout = (VertexOut)0.0f;

	MaterialData matData = gMaterialData_Default[0];

	// Transform to world space.
	float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);
	vout.PosW = posW.xyz;

	// Assumes nonuniform scaling; otherwise, need to use inverse-transpose of world matrix.
	vout.NormalW = mul(vin.NormalL, (float3x3)gWorld);

	// Transform to homogeneous clip space.
	vout.PosH = mul(posW, gViewProj);

	//float4 texC = mul(float4(vin.TexC, 0.0f, 1.0f), gTexTransform);

	//float2 texC = vin.TexC;
	//texC.x *= matData.MatTransform[3][0];
	//texC.y *= matData.MatTransform[3][1];

	//vout.TexC = mul(float4(vin.TexC, 0.f, 1.f), gTexTransform).xy;
	//vout.TexC += texC;
	vout.TexC = vin.TexC;

	return vout;

}

float4 PS_SPRITE(VertexOut pin) : SV_Target
{
	MaterialData matData = gMaterialData_Default[0];
	float4 diffuseAlbedo = matData.DiffuseAlbedo;
	float3 fresnelR0 = matData.FresnelR0;
	float  roughness = matData.Roughness;
	uint diffuseTexIndex = matData.DiffuseMapIndex;

	//float4 texC = mul(float4(pin.TexC, 0.0f, 1.0f), matData.MatTransform);
	//pin.TexC = texC.xy;
	//pin.TexC.x /= 2.f;
	//pin.TexC.y /= 2.f;
	float2 texC = pin.TexC;
	
	texC.x *= matData.MatTransform[3][0];
	texC.y *= matData.MatTransform[3][1];


	//float2 move = mul(float4(pin.TexC, 0.f, 1.f), gTexTransform).xy;
	texC.x += gTexTransform[3][0];
	texC.y += gTexTransform[3][1];


	pin.TexC = texC;

	//pin.TexC = texC;
	diffuseAlbedo *= gDiffuseMap_Default[0].Sample(gsamAnisotropicWrap, pin.TexC);

	if (diffuseAlbedo.a < 0.1)
		discard;

	return diffuseAlbedo;
}

float4 PS(VertexOut pin) : SV_Target
{
	MaterialData matData = gMaterialData_Default[0];
	float4 diffuseAlbedo = matData.DiffuseAlbedo;
	float3 fresnelR0 = matData.FresnelR0;
	float  roughness = matData.Roughness;
	uint diffuseTexIndex = matData.DiffuseMapIndex;

	diffuseAlbedo *= gDiffuseMap_Default[0].Sample(gsamAnisotropicWrap, pin.TexC);

	if (diffuseAlbedo.a < 0.1)
		discard;

	return diffuseAlbedo;
}

float2 RotateUV(float2 Uv, float2 Center, float Theta)
{
	float2 sc;

	sincos((Theta / 180 * 3.14159), sc.x, sc.y);

	float2 uv = Uv - Center;
	float2 RotUv;

	RotUv.x = dot(uv, float2(sc.y, -sc.x));
	RotUv.y = dot(uv, sc.xy);
	RotUv += Center;

	return RotUv;
}


VertexOut VS_Smog(VertexIn vin)
{
	VertexOut vout = (VertexOut)0.0f;

	MaterialData matData = gMaterialData_Default[0];

	// Transform to world space.
	float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);
	vout.PosW = posW.xyz;

	// Assumes nonuniform scaling; otherwise, need to use inverse-transpose of world matrix.
	vout.NormalW = mul(vin.NormalL, (float3x3)gWorld);

	// Transform to homogeneous clip space.
	vout.PosH = mul(posW, gViewProj);

	vout.TexC = float2(vin.TexC.x, -vin.TexC.y);

	return vout;
}

float4 PS_Smog(VertexOut pin) : SV_Target
{
	MaterialData matData = gMaterialData_Default[0];
float4 diffuseAlbedo = matData.DiffuseAlbedo;
float3 fresnelR0 = matData.FresnelR0;
float  roughness = matData.Roughness;
uint diffuseTexIndex = matData.DiffuseMapIndex;

diffuseAlbedo *= gDiffuseMap_Default[0].Sample(gsamAnisotropicWrap, pin.TexC);

if (diffuseAlbedo.a < 0.1)
	discard;

return diffuseAlbedo;
}