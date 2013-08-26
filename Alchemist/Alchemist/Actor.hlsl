extern float4x4 worldMatrix;
extern float4x4 worldViewProj;
extern float4x4 finalTransforms[35];
extern texture tex;
extern int NumVertInfluences = 2;
extern bool animated;

extern float3 sunDir;

bool ambient = true;
bool diffuse = true;
bool specular = true;

float3 Kambient = {0.1, 0.1, 0.1};				// ambient light constant - mimics material (hard coded)
float3 Lambient = {1.0, 1.0, 1.0};				// ambient light constant (hard coded)
float3 Kdiffuse = {1.0, 1.0, 1.0};				// diffuse light constant - mimics material (hard coded)
float3 Ldiffuse = {1.0, 1.0, 1.0};				// diffuse light constant (hard coded)
float3 Kspecular = {1.0, 1.0, 1.0};				// specular light constant - mimics material (hard coded)
float3 Lspecular = {1.0, 1.0, 1.0};				// specular light constant (hard coded)

sampler TexS = sampler_state 
{
	Texture = <tex>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	AddressU = WRAP;
	AddressV = MIRROR;
};


struct VertexData
{
	float3 pos			: POSITION0;
	float3 norm			: NORMAL;
    float4 col			: COLOR0;
    float2 tex0			: TEXCOORD0;
	float4 weights		: BLENDWEIGHT0;
	int4 boneIndices	: BLENDINDICES0;
};

struct PixelData
{
	float3 normal : NORMAL0;
    float4 pos : POSITION0;
    float4 col : COLOR0;
    float2 tex0: TEXCOORD0;
	float3 viewDir: TEXCOORD1;
};

struct PixelData VertexBlend(VertexData vin, PixelData vout)
{
	PixelData output = vout;

	float4 p = float4(0.0f, 0.0f, 0.0f, 1.0f);
	float lastWeight = 0.0f; 
	int n = NumVertInfluences-1; 

	for(int i = 0; i < n; ++i) 
	{ 
		lastWeight += vin.weights[i]; 
		p += vin.weights[i]*mul(vin.pos, finalTransforms[vin.boneIndices[i]]); 
	} 

	lastWeight = 1.0f - lastWeight; 
	p += lastWeight * mul(vin.pos, finalTransforms[vin.boneIndices[n]]); 
	p.w = 1.0f; 

	return output;
}


struct PixelData ShaderVS(VertexData vin)
{
	PixelData vout = (PixelData)0;

	vout.tex0 = vin.tex0;
	vout.pos = float4(vin.pos,1.0f);
	vout.col = vin.col;
	vout.normal = mul(vin.norm, worldMatrix);

	if (animated) vout = VertexBlend(vin, vout);
	vout.pos = mul(vout.pos, worldViewProj);

	return vout;
}

float4 ShaderPS(PixelData vout) : COLOR
{
	//vout.col = tex2D(TexS, vout.tex0);

	float4 col = {0.0,0.0,0.0,1.0};	
	float4 colOut = {0.0,0.0,0.0,1.0};	
	float4 cola = {0.0,0.0,0.0,1.0};	
	float4 cold = {0.0,0.0,0.0,1.0};	
	float4 cols = {0.0,0.0,0.0,1.0};	
	float3 specularLight = {0,0.5,0.0};
	float specLightPower = 0.8;
	float3 dVec;		// the diffuse vector
	float3 viewVec;		// the view vector
	float3 lsVec;		// point light source vector
	float3 rVec;		// reflection vector
	float Id;		// intensity of diffuse light
	float Is;		// intensity of specular light


	vout.normal = normalize(vout.normal);
	col = tex2D(TexS, vout.tex0);  // extract colour of pixel from texture
	//col = float4(0,0,1,1);

	// ambient light
	//cola.rgb = ambientLight*Kambient*Lambient;
	cola = col*0.2;
	
	// pertube normal
	
	// diffuse light
	dVec = normalize(sunDir); // normalize the diffuse vector
	Id = max(0,dot(-vout.normal,dVec));
	//if (Id < 0) Id *= -1;
	cold.rgb = col.rgb*Id*Kdiffuse*Ldiffuse;

	/*
	// specular light
	lsVec = normalize(sunDir);

	// set and normarlize the view vector


	//compute refracting vector
	rVec = normalize(2 * specLightPower * (vout.normal) - sunDir);
	
	//set the color of the specular light for this pixel
	cols = pow(saturate(dot(rVec,vout.viewDir)), specLightPower) * float4(specularLight,1.0f);
	*/

	//if (ambient) colOut += cola;
	//if (diffuse) colOut += cold;
	//if (specular) colOut += cols;

	colOut = cola+cold;

	colOut.a = col.a;

	saturate(colOut);

	return (colOut);

	//return(vout.col);
}

technique ActorTechnique
{
	pass first {
		vertexshader = compile vs_3_0 ShaderVS();
		pixelshader = compile ps_3_0 ShaderPS();
		//FillMode = WireFrame;
	}
}