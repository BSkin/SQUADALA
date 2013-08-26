uniform extern float4x4 worldViewProj;
uniform extern texture tex;
uniform extern float3 normal;

sampler TexS = sampler_state 
{
	Texture = <tex>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	AddressU = WRAP;
	AddressV = MIRROR;
};


struct inputVS 
{
	float3 pos	: POSITION0;
	float3 norm	: NORMAL;
    float4 col	: COLOR0;
    float2 tex0	: TEXCOORD0;
};

struct OutputVS 
{
    float4 pos	: POSITION0;
    float4 col	: COLOR0;
	float3 norm : NORMAL;
    float2 tex0	: TEXCOORD0;
};

struct OutputVS PhantomVS(inputVS vin)
{
	// initalize the output
	struct OutputVS vout = {{0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0}};	
	vout.tex0 = vin.tex0;
	//vout.col = float4(0,0,0,0);
	vout.col = vin.col;
	// transform the vertex
	vout.pos = float4(vin.pos,1.0f);
	vout.pos = mul(vout.pos,worldViewProj);

	vout.norm = vin.norm;
	//saturate(vout.col);

	vout.col = vin.col;

	return(vout);
}

/**************************************************************************************/

// Pixel Shader
//float4 TransformFunPS(float4 col : COLOR0) : COLOR

float4 PhantomPS(struct OutputVS v) : COLOR
{
	float3 n = normal;
	v.col = tex2D(TexS, v.tex0);

	float angle = acos((v.norm.x*n.x + v.norm.y*n.y + v.norm.z*n.z) / ( sqrt(pow(v.norm.x,2) + pow(v.norm.y,2) + pow(v.norm.z,2)) * (sqrt(pow(n.x,2) + pow(n.y,2) + pow(n.z,2))) ));
	
	v.col += ((angle/(3.1415f)))*float4(0.8,0.1,0.1,1.0);
	v.col.r += 0.25f;
	v.col.g += 0.25f;
	v.col.b += 0.25f;
	v.col.r = pow(v.col.r,8);
	v.col.g = pow(v.col.g,8);
	v.col.b = pow(v.col.b,8);
	

	return(v.col);
}

technique PhantomTechnique
{
	pass first {
		vertexshader = compile vs_3_0 PhantomVS();
		pixelshader = compile ps_3_0 PhantomPS();
	}
}