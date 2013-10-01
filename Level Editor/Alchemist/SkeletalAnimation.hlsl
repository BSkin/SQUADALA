extern float4x4 worldViewProj;
extern float4x4 finalTransforms[35];
extern texture tex;
extern int NumVertInfluences = 2;

sampler samp = sampler_state
{
	Texture = <tex>;
	MinFilter = LINEAR; 
	MagFilter = LINEAR; 
	MipFilter = LINEAR; 
	AddressU = WRAP;
	AddressV = MIRROR;
};

struct InputVS
{
	float4 pos			: POSITION0;
	float2 texCoord		: TEXCOORD0;
	float4 weights		: BLENDWEIGHT0;
	int4 boneIndices	: BLENDINDICES0;
};

struct OutputVS
{
	float4 pos		: POSITION0;
	float2 texCoord	: TEXCOORD;
	float4 diffuse	: COLOR0;
};

struct OutputVS VertexBlend(InputVS input)
{
	OutputVS output = (OutputVS)0;

	float4 p = float4(0.0f, 0.0f, 0.0f, 1.0f); 
	float lastWeight = 0.0f; 
	int n = NumVertInfluences-1; 

	for(int i = 0; i < n; ++i) 
	{ 
		lastWeight += input.weights[i]; 
		p += input.weights[i]*mul(input.pos, finalTransforms[input.boneIndices[i]]); 
	} 

	lastWeight = 1.0f - lastWeight; 
	p += lastWeight * mul(input.pos, finalTransforms[input.boneIndices[n]]); 
	p.w = 1.0f; 

	output.pos = mul(p, worldViewProj); 
	output.texCoord = input.texCoord; 
	output.diffuse = float4(1.0f, 1.0f, 1.0f, 1.0f); 

	return(output);
}

float4 PixelBlend(struct OutputVS v) : COLOR
{
	float4 col = tex2D(samp, v.texCoord);
	return col;
}

technique SkeletalAnimationTechnique
{
	pass first
	{
		vertexshader = compile vs_3_0 VertexBlend();
		pixelshader = compile ps_3_0 PixelBlend();

		Lighting = true;
	}
}