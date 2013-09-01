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

/*
sampler TexS = sampler_state	// sampler for the texture
{
	Texture = <tex>;
	AddressU = Clamp;
    AddressV = Clamp;
    //MinFilter = Linear;
    //MagFilter = Linear;
    //MipFilter = Linear;
};
*/

struct VertexData
{
	float3 pos:	POSITION;
    float2 tex0: TEXCOORD0;
};

struct PixelData
{
	float4 pos:	SV_POSITION;
    float2 tex0: TEXCOORD0;
};

float getGray( float4 c )
{
    return dot( c.rgb, ( (0.33333).xxx) );
}

float4 getEdge( float2 textureCoord )
{
	float viewportWidth = 1280.0f;
	float viewportHeight = 720.0f;

	// Outline properties (thickness and threshold)
	float thickness = 1.0f;
	float threshold = 0.7f;

	float2 quadScreenSize	= float2( viewportWidth, viewportHeight );

	float2 xOffset	= float2( thickness / quadScreenSize.x, 0.0f );
    float2 yOffset	= float2( 0.0f, thickness / quadScreenSize.y );
    float2 uv		= textureCoord.xy;
    float2 pp		= uv - yOffset;
    
	float4 cc;
	
	cc = tex2D( TexS, pp - xOffset );	float g00 = getGray( cc );
	cc = tex2D( TexS, pp );				float g01 = getGray( cc );
	cc = tex2D( TexS, pp + xOffset );	float g02 = getGray( cc );
    
	pp = uv;
    
	cc = tex2D( TexS, pp - xOffset );	float g10 = getGray( cc );
    cc = tex2D( TexS, pp );				float g11 = getGray( cc );
    cc = tex2D( TexS, pp + xOffset );	float g12 = getGray( cc );
    
	pp = uv + yOffset;

    cc = tex2D( TexS, pp - xOffset );	float g20 = getGray( cc );
	cc = tex2D( TexS, pp );				float g21 = getGray( cc );
	cc = tex2D( TexS, pp + xOffset );	float g22 = getGray( cc );
    
	// Sobel filter kernel
	float K00	= -3;
    float K01	= -10;
    float K02	= -3;
    float K10	= 0;
    float K11	= 0;
    float K12	= 0;
    float K20	= 3;
    float K21	= 10;
    float K22	= 3;

    float Gx	= 0;
    float Gy	= 0;
    
	// Gx
	Gx += g00 * K00;
    Gx += g01 * K01;
    Gx += g02 * K02;
    Gx += g10 * K10;
    Gx += g11 * K11;
    Gx += g12 * K12;
    Gx += g20 * K20;
    Gx += g21 * K21;
    Gx += g22 * K22;
	 
	// Gy
    Gy += g00 * K00;
    Gy += g01 * K10;
    Gy += g02 * K20;
    Gy += g10 * K01;
    Gy += g11 * K11;
    Gy += g12 * K21;
    Gy += g20 * K02;
    Gy += g21 * K12;
    Gy += g22 * K22; 
    
	float norm = sqrt( Gx * Gx + Gy * Gy );
    
	//float4 n = float4(normalize(float3(Gx*Gx, 1.0/norm, Gy*Gy), 1.0f);

	//if (norm < threshold)
	//	return float4( 0.0f, 0.0f, 0.0f, 1.0f );

	if (norm < threshold)
		return float4( 0.0f, 0.0f, 0.0f, 1.0f );

	else if (norm > threshold)
		return float4( 1.0f, 1.0f, 1.0f, 1.0f );
	
	return float4( 1.0f, 1.0f, 1.0f, 1.0f );
}

float4 blur(float2 texCoord)
{
	float4 color = {0,0,0,1};

	float range = 10.0f;

	for (float x = -range/1280.0; x < (range+1)/1280.0; x+=1.0/1280.0)
	{
		for (float y = -range/720.0; y < (range+1)/720.0; y+=1.0/720.0)
		{
			color += tex2D( TexS, float2(texCoord.x + x, texCoord.y + y));
		}
	}

	color/=range*range*4;

	//return tex2D(TexS, float2(texCoord.x-200, texCoord.y));

	return color;
}

struct PixelData ShaderVS(VertexData input)
{
	PixelData output = (PixelData)0;
	output.pos = float4(input.pos,1.0f);
	output.pos = mul(output.pos, worldViewProj);
	output.tex0 = input.tex0;
	return output;
}

float4 ShaderPS(PixelData input) : SV_TARGET
{
	float4 col = tex2D(TexS, input.tex0);
	
	//float x = (col.r + col.b + col.g) / 3;
	//col = float4(x,x,x,1);

	//col = getEdge( input.tex0 );

	//col = blur(input.tex0);
	
	return col;
}

technique FullScreenTechnique
{
	pass p0 
	{
		//ZEnable = false;
		vertexshader = compile vs_3_0 ShaderVS();
		pixelshader = compile ps_3_0 ShaderPS();
	}
}