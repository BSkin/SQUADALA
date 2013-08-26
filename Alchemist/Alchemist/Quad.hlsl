extern float4x4 worldViewProj;
extern texture tex;

sampler TexS = sampler_state 
{
	Texture = <tex>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	//MipFilter = LINEAR;
	AddressU = CLAMP;
	AddressV = CLAMP;
};

struct VertexData
{
	float3 pos			: POSITION0;
    float4 col			: COLOR0;
    float2 tex0			: TEXCOORD0;
};

struct PixelData
{
    float4 pos	: POSITION0;
    float4 col	: COLOR0;
    float2 tex0	: TEXCOORD0;
};

struct PixelData ShaderVS(VertexData vin)
{
	PixelData vout = (PixelData)0;

	vout.tex0 = vin.tex0;
	vout.pos = float4(vin.pos,1.0f);
	
	vout.pos = mul(vout.pos, worldViewProj);


	return vout;
}

float4 ShaderPS(PixelData vin) : COLOR
{
	vin.col = tex2D(TexS, vin.tex0);
	return(vin.col);
}

technique QuadTechnique
{
	pass first {
		vertexshader = compile vs_3_0 ShaderVS();
		pixelshader = compile ps_3_0 ShaderPS();

	}
}