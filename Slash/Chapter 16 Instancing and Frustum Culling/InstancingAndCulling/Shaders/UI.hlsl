//=============================================================================
// Sky.fx by Frank Luna (C) 2011 All Rights Reserved.
//=============================================================================

// Include common HLSL code.
#include "Common.hlsl"
//
//
VertexOut_UI VS(VertexIn_UI vin)
{

	VertexOut_UI vout;
	vout.position = float4(vin.PosL, 1.0f);
	vout.TexC = vin.TexC;
	return vout;
}

float4 PS(VertexOut_UI pin) : SV_Target
{
	//float4 litColor = gCubeMap.Sample(gsamLinearWrap, pin.PosL);
	MaterialData matData = gMaterialData_Default[0];
	float4 diffuseAlbedo = matData.DiffuseAlbedo;

	float4 litColor = gDiffuseMap_Default[0].Sample(gsamAnisotropicWrap, pin.TexC);
	diffuseAlbedo *= litColor;
	if (diffuseAlbedo.a < 0.2)
		discard;
	//litColor = ceil(litColor * 5) / 5.0f;
	return diffuseAlbedo;
}

float4 PS_Change(VertexOut_UI pin) : SV_Target
{
	//float4 litColor = gCubeMap.Sample(gsamLinearWrap, pin.PosL);
	//float4 litColor = gDiffuseMap_Default[0].Sample(gsamAnisotropicWrap, pin.TexC);
	MaterialData matData = gMaterialData_Default[0];
	float4 diffuseAlbedo = matData.DiffuseAlbedo;

	float2 texC = pin.TexC;

	texC.x *= matData.MatTransform[0][0];
	texC.y *= matData.MatTransform[1][1];


	//float2 move = mul(float4(pin.TexC, 0.f, 1.f), gTexTransform).xy;
	texC.x += matData.MatTransform[3][0];
	texC.y += matData.MatTransform[3][1];


	pin.TexC = texC;

	diffuseAlbedo *= gDiffuseMap_Default[0].Sample(gsamAnisotropicWrap, pin.TexC);

	if (diffuseAlbedo.a < 0.3)
		discard;
	//litColor = ceil(litColor * 5) / 5.0f;
	return diffuseAlbedo;

}
