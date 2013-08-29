extern float4x4 worldViewProj;
extern texture tex;
extern float numSpriteRows;
extern float numSpriteCols;
extern float curSpriteRow;
extern float curSpriteCol;
extern bool flipSprite;

sampler TexS = sampler_state 
{
	Texture = <tex>;
	//Filter = MIN_MAG_MIP_POINT;
	MinFilter = NONE;
	MagFilter = NONE;
	MipFilter = NONE;
	//AddressU = CLAMP;
	//AddressV = CLAMP;
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
	float u = vin.tex0.x;
	if (flipSprite) u = -u + 1;
	float v = vin.tex0.y;
	u /= numSpriteCols;
	v /= numSpriteRows;
	u += curSpriteCol/numSpriteCols;
	v += curSpriteRow/numSpriteRows;
	vin.col = tex2D(TexS, float2(u,v));
	return(vin.col);
}

technique ActorTechnique
{
	pass first {
		vertexshader = compile vs_3_0 ShaderVS();
		pixelshader = compile ps_3_0 ShaderPS();

	}
}