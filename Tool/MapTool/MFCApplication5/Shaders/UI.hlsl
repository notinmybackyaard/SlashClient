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
	float4 litColor = gDiffuseMap_Default[0].Sample(gsamAnisotropicWrap, pin.TexC);

	//litColor = ceil(litColor * 5) / 5.0f;
	return litColor;

}



