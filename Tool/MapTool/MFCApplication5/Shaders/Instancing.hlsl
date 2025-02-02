//***************************************************************************************
// Default.hlsl by Frank Luna (C) 2015 All Rights Reserved.
//***************************************************************************************

#include "Common.hlsl"

VertexOut VS(VertexIn vin, uint instanceID : SV_InstanceID)
{
	VertexOut vout = (VertexOut)0.0f;
	
	// Fetch the instance data.
	InstanceData instData = gInstanceData[instanceID];
	float4x4 world = instData.World;
	float4x4 texTransform = instData.TexTransform;
	uint matIndex = instData.MaterialIndex;

	vout.MatIndex = matIndex;
	
	// Fetch the material data.
	MaterialData matData = gMaterialData_Instance[matIndex];
	
    // Transform to world space.
    float4 posW = mul(float4(vin.PosL, 1.0f), world);
    vout.PosW = posW.xyz;

    // Assumes nonuniform scaling; otherwise, need to use inverse-transpose of world matrix.
    vout.NormalW = mul(vin.NormalL, (float3x3)world);

    // Transform to homogeneous clip space.
    vout.PosH = mul(posW, gViewProj);
	
	// Output vertex attributes for interpolation across triangle.
	float4 texC = mul(float4(vin.TexC, 0.0f, 1.0f), texTransform);
	vout.TexC = mul(texC, matData.MatTransform).xy;
	
    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	// Fetch the material data.
	MaterialData matData = gMaterialData_Instance[pin.MatIndex];
	float4 diffuseAlbedo = matData.DiffuseAlbedo;
    float3 fresnelR0 = matData.FresnelR0;
    float  roughness = matData.Roughness;
	uint diffuseTexIndex = matData.DiffuseMapIndex;
	
	// Dynamically look up the texture in the array.
    diffuseAlbedo *= gDiffuseMap_Instancing[diffuseTexIndex].Sample(gsamLinearWrap, pin.TexC);
	
    // Interpolating normal can unnormalize it, so renormalize it.
    pin.NormalW = normalize(pin.NormalW);

    // Vector from point being lit to eye. 
    float3 toEyeW = normalize(gEyePosW - pin.PosW);

    // Light terms.
    float4 ambient = gAmbientLight*diffuseAlbedo;

    const float shininess = 1.0f - roughness;
    Material mat = { diffuseAlbedo, fresnelR0, shininess };
    float3 shadowFactor = 1.0f;
    float4 directLight = ComputeLighting(gLights, mat, pin.PosW,
        pin.NormalW, toEyeW, shadowFactor);


	
    float4 litColor = ambient + directLight;

    // Common convention to take alpha from diffuse albedo.
    litColor.a = diffuseAlbedo.a;
	//litColor.rgb = float3(1.0, 0, 0);
	
	//litColor = ceil(litColor * 5) / 5.0f;


    return litColor;
}


