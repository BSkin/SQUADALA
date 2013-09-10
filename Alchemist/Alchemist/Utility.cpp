#include "Utility.h"
#include "Dxerr.h"
#include <io.h>
#include <algorithm>

// Output a string to Visual Studio's output pane
void Utility::DebugString(const std::string &str)
{
	OutputDebugString(str.c_str());
}

// Check to see if a DirectX call failed. If it did output the error string.
bool Utility::FailedHr(HRESULT hr)
{
	if (FAILED(hr))
	{
		DebugString("DirectX Reported Error: "+ToString(DXGetErrorString(hr))+" - " + 
			ToString(DXGetErrorDescription(hr))+"\n");
		std::string s = ToString(DXGetErrorDescription(hr));
		return true;
	}

	return false;
}

/**
 * \brief makes a new character string from the passed one
 * \param charString - a zero terminated char string
 * \return the new string. Caller is responsible for freeing the memory
 * \author Keith Ditchburn \date 17 July 2005
*/
char* Utility::DuplicateCharString(const char* charString)
{
    if (!charString)
		return 0;

	size_t len=strlen(charString) + 1;
	char *newString = new char[len];
	memcpy(newString, charString, len*sizeof(char));

	return newString;
}

/* 
Try to find a texture file
There is often pain finding the textures for .x files as sometimes the filename
includes the path and other times it does not. It is a cause of a lot of issues. By
saving your .x file as text you can look at it and see what the filenames are set to
which can help spot problems.
*/
bool Utility::FindFile(std::string *filename)
{
	if (!filename)
		return false;

	// Look for it first using the original filename and path
	if (DoesFileExist(*filename))
		return true;

	// Next try with stripping the path from it
	std::string pathOnly;
	std::string filenameOnly;
	SplitPath(*filename,&pathOnly,&filenameOnly);

	if (DoesFileExist(filenameOnly))
	{
		*filename=filenameOnly;
		return true;
	}

	// Could now try by looking in a specific data folder....
	return false;
}

/*
	Uses the basic C run time _access fucntion to determine if the file exists
*/
bool Utility::DoesFileExist(const std::string &filename)
{
	return (_access(filename.c_str(), 0) != -1);
}

/*
	Splits a path and filename string
	pathOnly or filenameOnly can be null if they are not required
*/
void Utility::SplitPath(const std::string& inputPath, std::string* pathOnly, std::string* filenameOnly)
{
	std::string fullPath(inputPath);
	std::replace(fullPath.begin(),fullPath.end(),'\\','/');

	std::string::size_type lastSlashPos=fullPath.find_last_of('/');

	// check for there being no path element in the input
	if (lastSlashPos==std::string::npos)
	{
		*pathOnly="";
		*filenameOnly=fullPath;
	}
	else // do the split
	{
		if (pathOnly)
			*pathOnly=fullPath.substr(0,lastSlashPos);

		if (filenameOnly)
			*filenameOnly=fullPath.substr(lastSlashPos+1,fullPath.size()-lastSlashPos-1);
	}
}

/*
	Retrieves the current directory
	Takes into account buffer size etc.
*/
std::string Utility::GetTheCurrentDirectory()
{
	int bufferSize=GetCurrentDirectory(0,NULL);
	char *buffer=new char[bufferSize];

	GetCurrentDirectory(bufferSize,buffer);
	std::string directory(buffer);
	delete []buffer;

	return directory;
}

D3DXMATRIX Utility::btTransToD3DXMat(btTransform * bulletTransformMatrix)
{
	btVector3 R = bulletTransformMatrix->getBasis().getColumn(0);
	btVector3 U = bulletTransformMatrix->getBasis().getColumn(1);
	btVector3 L = bulletTransformMatrix->getBasis().getColumn(2);
	btVector3 P = bulletTransformMatrix->getOrigin();
				
	D3DXVECTOR3 vR, vU, vL, vP;
	vR.x = R.x();vR.y = R.y();vR.z = R.z();
	vU.x = U.x();vU.y = U.y();vU.z = U.z();
	vL.x = L.x();vL.y = L.y();vL.z = L.z();
	vP.x = P.x();vP.y = P.y();vP.z = P.z();

	D3DXMATRIX matOutput;
	matOutput._11 = vR.x;matOutput._12 = vR.y;matOutput._13 = vR.z;matOutput._14 = 0.f;
	matOutput._21 = vU.x;matOutput._22 = vU.y;matOutput._23 = vU.z;matOutput._24 = 0.f;
	matOutput._31 = vL.x;matOutput._32 = vL.y;matOutput._33 = vL.z;matOutput._34 = 0.f;
	matOutput._41 = vP.x;matOutput._42 = vP.y;matOutput._43 = vP.z;matOutput._44 = 1.f;

	return matOutput;
}

btTransform Utility::D3DXMatTobtTrans(D3DXMATRIX * worldMatrix)
{
	btTransform bulletTransformMatrix;
	btVector3 R,U,L,P;
	R.setX(btScalar(worldMatrix->_11)); R.setY(btScalar(worldMatrix->_12)); R.setZ(btScalar(worldMatrix->_13));
	U.setX(btScalar(worldMatrix->_21)); U.setY(btScalar(worldMatrix->_22)); U.setZ(btScalar(worldMatrix->_23));
	L.setX(btScalar(worldMatrix->_31)); L.setY(btScalar(worldMatrix->_32)); L.setZ(btScalar(worldMatrix->_33));
	P.setX(btScalar(worldMatrix->_41)); P.setY(btScalar(worldMatrix->_42)); P.setZ(btScalar(worldMatrix->_43));

	btMatrix3x3 bMatrix;
	bMatrix = btMatrix3x3(R.x(), R.y(), R.z(), U.z(), U.y(), U.z(), L.x(), L.y(), L.z());
	bulletTransformMatrix.setBasis(bMatrix);
	bulletTransformMatrix.setOrigin(P);

	return bulletTransformMatrix;
}

D3DXVECTOR3 Utility::ConvertToD3D(const btVector3 * vec) { return D3DXVECTOR3(vec->x(), vec->y(), vec->z()); }
btVector3 Utility::ConvertToBt(const D3DXVECTOR3 * vec) { return btVector3(vec->x, vec->y, vec->z); }