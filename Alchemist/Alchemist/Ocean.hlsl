uniform extern float4x4 worldViewProj;
uniform extern float3 camPos;
uniform extern float time;
uniform extern float direction;
uniform extern int amp;
uniform extern float length;

uniform extern texture rippleNorm;
uniform extern texture rippleDisp;

uniform extern float3 sunDir;

//uniform extern float3 ambientLight;
//uniform extern float3 specularLight;
//uniform extern float3 viewPoint;
//uniform extern float3 specLightPos;
//uniform extern float specLightPower;

bool ambient = true;
bool diffuse = true;
bool specular = true;

float3 Kambient = {0.1, 0.1, 0.1};				// ambient light constant - mimics material (hard coded)
float3 Lambient = {1.0, 1.0, 1.0};				// ambient light constant (hard coded)
float3 Kdiffuse = {1.0, 1.0, 1.0};				// diffuse light constant - mimics material (hard coded)
float3 Ldiffuse = {1.0, 1.0, 1.0};				// diffuse light constant (hard coded)
float3 Kspecular = {1.0, 1.0, 1.0};				// specular light constant - mimics material (hard coded)
float3 Lspecular = {1.0, 1.0, 1.0};				// specular light constant (hard coded)

sampler rippleNormSamp = sampler_state				// sampler for the texture
{
	Texture = <rippleNorm>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = LINEAR;
    AddressU = WRAP;
    AddressV = MIRROR;
};

sampler rippleDispSamp = sampler_state				// sampler for the texture
{
	Texture = <rippleDisp>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = LINEAR;
    AddressU = WRAP;
    AddressV = MIRROR;
};

struct VertexData
{
	float3 norm: NORMAL0;
    float3 pos : POSITION0;
    float4 col : COLOR0;
    float2 tex0: TEXCOORD0;
};

struct PixelData
{
	float3 normal : NORMAL0;
    float4 pos : POSITION0;
    float4 col : COLOR0;
    float2 tex0: TEXCOORD0;
	float3 viewDir: TEXCOORD1;
};

struct PixelData ShaderVS(VertexData vin)
{
	PixelData vout = (PixelData)0;

	//Wave shit here
	float k = 1.0/length;	//Wave Number
	float omega = 0.03;		//Wave Speed

	float dx = -cos(direction);
	float dz = sin(direction);

	float d = (dx*(vin.pos.x+camPos.x) + dz*(vin.pos.z+camPos.z));///sqrt(dx*dx + dz*dz);
	
	dx = -cos(direction + 1.5707963267948966192313216916398);
	dz = sin(direction + 1.5707963267948966192313216916398);

	float d2 = (dx*(vin.pos.x+camPos.x) + dz*(vin.pos.z+camPos.z));///sqrt(dx*dx + dz*dz);

	vin.pos.y = amp*sin(k*d+omega*time) + amp*sin(k*d2);

	float xder = -cos(direction) * k * amp * cos(
					k * (-cos(direction)*(vin.pos.x+camPos.x) + sin(direction)*(vin.pos.z+camPos.z))+omega*time) 
				- cos(direction) * k * amp * sin(
					k * (-cos(direction + 1.5707963267948966192313216916398) * (vin.pos.x+camPos.x) + sin(direction + 1.5707963267948966192313216916398) * (vin.pos.z+camPos.z)));
	float zder = sin(direction+1.5707963267948966192313216916398) * k * amp * cos(
					k * (-cos(direction) * (vin.pos.x+camPos.x) + sin(direction)*(vin.pos.z+camPos.z))+omega*time) 
				+ sin(direction+1.5707963267948966192313216916398) * k * amp * sin(
					k * (-cos(direction + 1.5707963267948966192313216916398) * (vin.pos.x+camPos.x) + sin(direction + 1.5707963267948966192313216916398) * (vin.pos.z+camPos.z)));

	float3 xvec = { 1.0, xder, 0.0 };
	float3 zvec = { 0.0, zder, 1.0 };

	float3 normal = -cross(xvec, zvec);
	normalize(normal);

	//vout.normal = 5.0*d*cos(k*d + omega*time) + 5.0*d2*cos(k*d2);
	vout.normal = float3(0,1,0);
	vout.normal = normal;

	vin.tex0 *= 5;

	float4 rippleMod = {0.0,0.0,0.0,0.0};
	float4 tex4 = {0.0, 0.0, 0.0, 0.0};
	tex4.xy = vin.tex0;
	tex4.y -= time*0.002;
	rippleMod = tex2Dlod(rippleDispSamp, tex4);

	float4 normalMod = tex2Dlod(rippleNormSamp, tex4);
	vout.normal.xyz += normalMod.xyz;
	//vout.normal += float3(0,1,0);
	//yMod += 2*rippleMod.z;
	//float x = rippleMod[0];

	vout.tex0 = vin.tex0;
	vout.pos = float4(vin.pos,1.0f);
	//vout.pos.y += yMod;
	//vout.col = float4(0,0,vin.pos.y/amp/2+1,0.8);

	vout.col = vin.col;

	vout.pos = mul(vout.pos, worldViewProj);
	vout.viewDir = normalize(camPos);

	return vout;
}

float4 ShaderPS(PixelData vout) : COLOR
{
	//vout.tex0.y  += time*0.001;
	//vout.col = tex2D(rippleNormSamp, vout.tex0);
	//float x = vout.pos.y;
	//vout.col = float4(0,0,0.8,0.6);
	//return tex2D(rippleNormSamp, vout.tex0);

	float4 col = {0.0,0.0,0.5,1.0};	
	float4 colOut = {0.0,0.0,0.0,1.0};	
	float4 cola = {0.0,0,0.1,1};	
	float4 cold = {0.0,0.0,0.0,1.0};	
	float4 cols = {0.5,0.5,0.5,1.0};	
	float3 specularLight = {0,0.5,0.0};
	float specLightPower = 0.8;
	float3 dVec;		// the diffuse vector
	float3 viewVec;		// the view vector
	float3 lsVec;		// point light source vector
	float3 rVec;		// reflection vector
	float Id;		// intensity of diffuse light
	float Is;		// intensity of specular light


	vout.normal = normalize(vout.normal);
	//col = tex2D(TexS, vout.tex0);  // extract colour of pixel from texture
	col = float4(0.4,0.4,1.0,0.6);

	// ambient light
	//cola.rgb = ambientLight*Kambient*Lambient;
	cola = col*0.1;

	// pertube normal
	
	// diffuse light
	dVec = normalize(sunDir); // normalize the diffuse vector
	Id = max(0,dot(-vout.normal,dVec));
	//if (Id < 0) Id *= -1;
	cold.rgb = col.rgb*Id*Kdiffuse*Ldiffuse;

	
	// specular light
	lsVec = normalize(sunDir);

	// set and normarlize the view vector

	//compute refracting vector
	rVec = normalize(2.0 * specLightPower * vout.normal - camPos);
	
	//set the color of the specular light for this pixel
	cols = pow(saturate(dot(rVec,vout.viewDir)), specLightPower) * float4(specularLight,1.0f);

	//if (ambient) colOut += cola;
	//if (diffuse) colOut += cold;
	//if (specular) colOut += cols;

	colOut = cola+cold+cols;
	colOut = cola+cold;

	colOut.a = col.a;

	/*
	float r = (1-x)*colOut.r;
	float g = (1-x)*colOut.g;
	float b = (1-x)*colOut.b; 

	colOut.r = r;
	colOut.g = g;
	colOut.b = b;	
	*/
	//saturate(colOut);

	return (colOut);
	//return(vout.col);
}

technique OceanTechnique
{
	pass first {
		vertexshader = compile vs_3_0 ShaderVS();
		pixelshader = compile ps_3_0 ShaderPS();

		//FillMode = WireFrame;
		//FillMode = Solid;
	}
}