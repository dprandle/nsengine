/*--------------------------------------------- Noble Steed Engine-------------------------------------*
Authors : Daniel Randle, Alex Bourne
Date Created : Mar 7 2013

File:
	nstexture.cpp

Description:
	This file contains the definition for the NSTexture class functions along with any other helper
	functions
*-----------------------------------------------------------------------------------------------------*/


#include <nslogfile.h>
#include <nstexture.h>
#include <nstexmanager.h>

NSTexture::NSTexture(TexType type) :
	mTexType(type),
	mAllocated(false),
	mLocked(false),
	mFormat(0),
	mInternalFormat(0),
	mPixelDataType(0),
	mMipMapLevel(0),
	mGenMipmaps(true),
	mBorder(0),
	mData(NULL, 0, 0)
{
	setExtension(DEFAULT_TEX_EXTENSION);
}

NSTexture::~NSTexture()
{
	release();
}

void NSTexture::initGL()
{
	glGenTextures(1, &mGLName);
	GLError("NSTexture::initGL");
}

void NSTexture::bind()
{
	glBindTexture(mTexType, mGLName);
	GLError("NSTexture::bind");
}

uint32 NSTexture::border() const
{
	return mBorder;
}

void NSTexture::setAllocated(bool alloc)
{
	mAllocated = alloc;
}

bool NSTexture::allocated()
{
	return mAllocated;
}

/*!
Enable mipmaps for this texture - level of 0 means use the default max mip map level
If you want to disable mipmaps you need to create a new texture with them disabled
If the texture has been allocated then this will call generate mip map function
If it has not, it will call generate mip map function when it is allocated
*/
void NSTexture::enableMipMaps(int32 level)
{
	mMipMapLevel = level;
	mGenMipmaps = true;
	if (mAllocated)
	{
		if (level != 0)
			setParameteri(MaxLevel, mMipMapLevel);
		glGenerateMipmap(mTexType);
		GLError("NSTexture::enableMipMaps");
	}
}

void NSTexture::setBorder(uint32 border)
{
	if (mAllocated)
		return;
	mBorder = border;
}

uint32 NSTexture::channels()
{
	if (mFormat == GL_RGBA || mFormat == GL_RGBA_INTEGER || GL_BGRA || GL_BGRA_INTEGER)
		return 4;
	else if (mFormat == GL_RGB || mFormat == GL_RGB_INTEGER || mFormat == GL_BGR || mFormat == GL_BGR_INTEGER)
		return 3;
	else if (mFormat == GL_RG || mFormat == GL_RG_INTEGER || mFormat == GL_DEPTH_STENCIL)
		return 2;
	else if (mFormat == GL_BLUE || mFormat == GL_GREEN || mFormat == GL_RED ||
		mFormat == GL_BLUE_INTEGER || mFormat == GL_GREEN_INTEGER || mFormat == GL_RED_INTEGER ||
		mFormat == GL_STENCIL_INDEX || mFormat == GL_DEPTH_COMPONENT)
		return 1;
	else
		return 0;
}

uint32 NSTexture::bpp()
{
	char chans; char bpc;

	chans = channels();

	switch (mPixelDataType)
	{
	case(GL_UNSIGNED_BYTE) :
		bpc = 1;
		break;
	case(GL_BYTE) :
		bpc = 1;
		break;
	case(GL_UNSIGNED_SHORT) :
		bpc = 2;
		break;
	case(GL_SHORT) :
		bpc = 2;
		break;
	case(GL_UNSIGNED_INT) :
		bpc = 4;
		break; 
	case(GL_INT) :
		bpc = 4;
		break; 
	case(GL_HALF_FLOAT) :
		bpc = 2;
		break; 
	case(GL_FLOAT) :
		bpc = 4;
		break; 
	case(GL_UNSIGNED_BYTE_3_3_2) :
		bpc = 1;
		break; 
	case(GL_UNSIGNED_BYTE_2_3_3_REV) : 
		bpc = 1;
		break; 
	case(GL_UNSIGNED_SHORT_5_6_5) : 
		bpc = 2;
		break; 
	case(GL_UNSIGNED_SHORT_5_6_5_REV) : 
		bpc = 2;
		break; 
	case(GL_UNSIGNED_SHORT_4_4_4_4) : 
		bpc = 2;
		break;
	case(GL_UNSIGNED_SHORT_4_4_4_4_REV) : 
		bpc = 2;
		break; 
	case(GL_UNSIGNED_SHORT_5_5_5_1) : 
		bpc = 2;
		break; 
	case(GL_UNSIGNED_SHORT_1_5_5_5_REV) : 
		bpc = 2;
		break; 
	case(GL_UNSIGNED_INT_8_8_8_8) :
		bpc = 4;
		break; 
	case(GL_UNSIGNED_INT_8_8_8_8_REV) : 
		bpc = 4;
		break; 
	case(GL_UNSIGNED_INT_10_10_10_2) : 
		bpc = 4;
		break; 
	case(GL_UNSIGNED_INT_2_10_10_10_REV) : 
		bpc = 4;
		break; 
	case(GL_UNSIGNED_INT_24_8) : 
		bpc = 4;
		break; 
	case(GL_UNSIGNED_INT_10F_11F_11F_REV) : 
		bpc = 4;
		break; 
	case(GL_UNSIGNED_INT_5_9_9_9_REV) :
		bpc = 4;
		break; 
	case(GL_FLOAT_32_UNSIGNED_INT_24_8_REV) :
		bpc = 4;
		break;
	default:
		bpc = 0;
	}
	return chans * bpc;
}

float NSTexture::getParameterf(GetTexParam p)
{
	GLfloat f;

	if (mTexType == TexCubeMap)
		glGetTexParameterfv(BASE_CUBEMAP_FACE, p, &f);
	else
		glGetTexParameterfv(mTexType, p, &f);
	
	GLError("NSTexture::getParameterf");
	return f;
}

int32 NSTexture::getParameteri(GetTexParam p)
{
	GLint i;

	if (mTexType == TexCubeMap)
		glGetTexParameteriv(BASE_CUBEMAP_FACE, p, &i);
	else
		glGetTexParameteriv(mTexType, p, &i);

	GLError("NSTexture::getParameteri");
	return i;
}

void NSTexture::disable(uint32 pTexUnit)
{
	glActiveTexture(BASE_TEX_UNIT + pTexUnit);
	GLError("NSTexture::disable");
	unbind();
}

void NSTexture::enable(uint32 pTexUnit)
{
	glActiveTexture(BASE_TEX_UNIT + pTexUnit);
	GLError("NSTexture::enable");
	bind();
}

NSTexture::ImageData NSTexture::data()
{
	return mData;
}

bool NSTexture::locked() const
{
	return mLocked;
}

int32 NSTexture::internalFormat() const
{
	return mInternalFormat;
}

int32 NSTexture::format() const
{
	return mFormat;
}

int32 NSTexture::pixelDataType() const
{
	return mPixelDataType;
}

void NSTexture::release()
{
	glDeleteTextures(1,&mGLName);
	mGLName = 0;
	mAllocated = false;
}

void NSTexture::setGLName(uint32 glid)
{
	if (glid != 0)
	{
		if (mGLName != 0)
			release();
		mGLName = glid;
	}
}

void NSTexture::setInternalFormat(int32 intformat)
{
	if (mAllocated)
		return;

	mInternalFormat = intformat;
}

void NSTexture::pup(NSFilePUPer * p)
{
	if (p->type() == NSFilePUPer::Binary)
	{
		NSBinFilePUPer * bf = static_cast<NSBinFilePUPer *>(p);
		::pup(*bf, *this);
	}
	else
	{
		NSTextFilePUPer * tf = static_cast<NSTextFilePUPer *>(p);
		::pup(*tf, *this);
	}
}

void NSTexture::setFormat(int32 pFormat)
{
	mFormat = pFormat;
}

void NSTexture::setPixelDataType(int32 pDataType)
{
	mPixelDataType = pDataType;
}

void NSTexture::setParameterf(TexParameter pParam, float pValue)
{
	if (pParam == GL_TEXTURE_BUFFER)
		return;
	glTexParameterf(mTexType, pParam, pValue);
	GLError("NSTexture::setParameterf");
}

void NSTexture::setParameteri(TexParameter pParam, int32 pValue)
{
	if (pParam == GL_TEXTURE_BUFFER)
		return;
	glTexParameteri(mTexType, pParam, pValue);
	GLError("NSTexture::setParameteri");
}

void NSTexture::setParameterfv(TexParameter param, const fvec4 & val)
{
	if (param == GL_TEXTURE_BUFFER)
		return;
	glTexParameterfv(mTexType, param, &val[0]);
	GLError("NSTexture::setParameterfv");
}

void NSTexture::setParameteriv(TexParameter param, const ivec4 & val)
{
	if (param == GL_TEXTURE_BUFFER)
		return;
	glTexParameteriv(mTexType, param, &val[0]);
	GLError("NSTexture::setParameteriv");
}

void NSTexture::setParameterIiv(TexParameter param, const ivec4 & val)
{
	if (param == GL_TEXTURE_BUFFER)
		return;
	glTexParameterIiv(mTexType, param, &val[0]);
	GLError("NSTexture::setParameterIiv");
}

void NSTexture::setParameterIiuv(TexParameter param, const uivec4 & val)
{
	if (param == GL_TEXTURE_BUFFER)
		return;
	glTexParameterIuiv(mTexType, param, &val[0]);
	GLError("NSTexture::setParameterIiuv");
}

void NSTexture::unbind()
{
	glBindTexture(mTexType, 0);
}

NSTexture::TexType NSTexture::textureType() const
{
	return mTexType;
}

/*-------------------------------------------------------------------------------------------------------------------------------------------------------------------*/



NSTex1D::NSTex1D():
mWidth(0),
mCompByteSize(0),
mImmutable(false),
NSTexture(Tex1D)
{}

NSTex1D::~NSTex1D()
{
	if (mLocked && mData.data != NULL)
		delete[] mData.data;
}

/*
Using width, internal format, pixel data type, format, and mip map level this function
will allocate new texture data to the currently bound texture name. Be sure to bind the texture
before calling allocate.
*/
bool NSTex1D::allocate(const void * data)
{
	if (mAllocated)
		return false;

	if (mCompByteSize == 0)
	{
		if (mImmutable)
			glTexStorage1D(mTexType, mMipMapLevel, mInternalFormat, mWidth);
		else
			glTexImage1D(mTexType, 0, mInternalFormat, mWidth, mBorder, mFormat, mPixelDataType, data); // specify base mip map level
	}
	else
	{
		glCompressedTexImage1D(mTexType, 0, mInternalFormat, mWidth, mBorder, mCompByteSize, data);
	}

	mAllocated = !GLError("NSTex1D::allocate");
	return mAllocated;
}

/*
Allocate new space for a texture and copy screen pixels from whatever read buffer is currently bound
Read buffer must be bound (glReadBuffer) in order to copy screen pixels
\param lowerLeft x and y screen coordinates
\param dimensions width and height starting from lowerLeft
*/
bool NSTex1D::allocateFromScreen(const uivec2 & lowerLeft, const uivec2 dimensions)
{
	if (mCompByteSize != 0 || mImmutable) // Cannot perform this operation on a compressed textureor immutable texture
		return false;

	glCopyTexImage1D(mTexType, 0, mInternalFormat, lowerLeft.x, lowerLeft.y, mWidth, mBorder);
	mAllocated = !GLError("NSTexture::allocateFromScreen()");
	return mAllocated;
}


bool NSTex1D::compressed() const
{
	return (mCompByteSize != 0);
}

bool NSTex1D::immutable() const
{
	return mImmutable;
}

void NSTex1D::init()
{
	// do nothing
}

void NSTex1D::pup(NSFilePUPer * p)
{
	if (p->type() == NSFilePUPer::Binary)
	{
		NSBinFilePUPer * bf = static_cast<NSBinFilePUPer *>(p);
		::pup(*bf, *this);
	}
	else
	{
		NSTextFilePUPer * tf = static_cast<NSTextFilePUPer *>(p);
		::pup(*tf, *this);
	}
}

bool NSTex1D::lock()
{
	mLocked = true;

	if (!mAllocated)
		return mLocked;

	if (mCompByteSize == 0)
	{
		mData.bpp = bpp();
		mData.size = mData.bpp*mWidth;
		mData.data = new char[mData.size];
		glGetTexImage(mTexType, 0, mFormat, mPixelDataType, static_cast<void*>(mData.data));
	}
	else
	{
		mData.size = mCompByteSize;
		mData.data = new char[mData.size];
		glGetCompressedTexImage(mTexType, 0, static_cast<void*>(mData.data));
	}

	mLocked = !GLError("NSTex1D::lock");
	if (!mLocked)
	{
		mData.bpp = 0;
		mData.size = 0;
		delete[] mData.data;
		mData.data = NULL;
	}
	return mLocked;
}

bool NSTex1D::unlock()
{
	if (!mLocked)
		return mLocked;

	mLocked = false;
	bool ret = setData(mData.data, 0, mWidth);
	mData.bpp = 0;
	mData.size = 0;
	delete[] mData.data;
	mData.data = NULL;
	return ret;
}

/*
Set image data starting from the offset and going until offset + dimensions
If data is bound to PIXEL_UNPACK_BUFFER then data will be used as a byte offset in to
the store data
*/
bool NSTex1D::setData(const void * data, uint32 xoffset, uint32 width)
{
	if (!mAllocated || mImmutable)
		return false;

	if (mCompByteSize == 0)
		glTexSubImage1D(mTexType, 0, xoffset, width, mFormat, mPixelDataType, data);
	else
		glCompressedTexSubImage1D(mTexType, 0, xoffset, width, mFormat, mCompByteSize, data);

	return GLError("NSTex1D::setData");
}

bool NSTex1D::setCompressed(uint32 byteSize)
{
	if (mAllocated)
		return false;
	mCompByteSize = byteSize;
	return true;
}

/*
Copy pixels from currently bound read buffer to existing texture - does not allocate space
*/
bool NSTex1D::setDataFromScreen(uint32 xoffset, const uivec2 & lowerLeft, uint32 width)
{
	if (!mAllocated || mCompByteSize != 0 || mImmutable)
		return false;

	glCopyTexSubImage1D(mTexType, 0, xoffset, lowerLeft.x, lowerLeft.y, width);
	return GLError("NSTex1D::setDataFromScreen");
}

void NSTex1D::setImmutable(bool immutable)
{
	if (mAllocated)
		return;

	mImmutable = immutable;
}

void NSTex1D::resize(uint32 w)
{
	if (!mAllocated)
	{
		mWidth = w;
		return;
	}
	else if (mImmutable)
		return;

	lock();
	uint32 nsize = w*bpp();
	uint8 * cpy = new uint8[nsize];
	for (uint32 i = 0; i < nsize; ++i)
	{
		if (i < mData.size)
			cpy[i] = mData.data[i];
		else
			cpy[i] = 0;
	}
	unlock();

	mAllocated = false;
	mWidth = w;
	allocate(cpy);
	delete[] cpy;
}

void NSTex1D::setWidth(uint32 width)
{
	if (mAllocated)
		return;
	mWidth = width;
}

uint32 NSTex1D::width()
{
	return mWidth;
}


/*------------------------------------------------------------------------------------------------------------------------------------------*/

NSTex1DArray::NSTex1DArray() :NSTex2D() { mTexType = Tex1DArray;}
NSTex1DArray::~NSTex1DArray(){}

/*------------------------------------------------------------------------------------------------------------------------------------------*/

NSTex2D::NSTex2D() :
mDim(),
mCompByteSize(0),
mImmutable(false),
NSTexture(Tex2D)
{}

NSTex2D::~NSTex2D()
{
	if (mLocked && mData.data != NULL)
		delete[] mData.data;
}

/*
Using width, internal format, pixel data type, format, and mip map level this function
will allocate new texture data to the currently bound texture name. Be sure to bind the texture
before calling allocate.
*/
bool NSTex2D::allocate(const void * data)
{
	if (mAllocated)
		return false;

	if (mCompByteSize == 0)
	{
		if (mImmutable)
			glTexStorage2D(mTexType, mMipMapLevel, mInternalFormat, mDim.w, mDim.h);
		else
			glTexImage2D(mTexType, 0, mInternalFormat, mDim.w, mDim.h, mBorder, mFormat, mPixelDataType, data); // specify base mip map level
	}
	else
	{
		glCompressedTexImage2D(mTexType, 0, mInternalFormat, mDim.w, mDim.h, mBorder, mCompByteSize, data);
	}

	mAllocated = !GLError("NSTex2D::allocate");
	return mAllocated;
}

/*
Allocate new space for a texture and copy screen pixels from whatever read buffer is currently bound
Read buffer must be bound (glReadBuffer) in order to copy screen pixels
\param lowerLeft x and y screen coordinates
\param dimensions width and height starting from lowerLeft
*/
bool NSTex2D::allocateFromScreen(const uivec2 & lowerLeft, const uivec2 dimensions)
{
	if (mAllocated || mImmutable)
		return false;

	if (mCompByteSize != 0) // Cannot perform this operation on a compressed texture
		return false;

	glCopyTexImage2D(mTexType, 0, mInternalFormat, lowerLeft.x, lowerLeft.y, mDim.w, mDim.h, mBorder);
	mAllocated = !GLError("NSTex2D::allocateFromScreen()");
	return mAllocated;
}


bool NSTex2D::compressed() const
{
	return (mCompByteSize != 0);
}

const uivec2 & NSTex2D::dim() const
{
	return mDim;
}

bool NSTex2D::immutable() const
{
	return mImmutable;
}

void NSTex2D::init()
{
	// do nothing
}

void NSTex2D::pup(NSFilePUPer * p)
{
	if (p->type() == NSFilePUPer::Binary)
	{
		NSBinFilePUPer * bf = static_cast<NSBinFilePUPer *>(p);
		::pup(*bf, *this);
	}
	else
	{
		NSTextFilePUPer * tf = static_cast<NSTextFilePUPer *>(p);
		::pup(*tf, *this);
	}
}

bool NSTex2D::lock()
{
	mLocked = true;

	if (!mAllocated)
		return mLocked;

	if (mCompByteSize == 0)
	{
		mData.bpp = bpp();
		mData.size = mData.bpp*mDim.w*mDim.h;
		mData.data = new char[mData.size];
		glGetTexImage(mTexType, 0, mFormat, mPixelDataType, static_cast<void*>(mData.data));
	}
	else
	{
		mData.size = mCompByteSize;
		mData.data = new char[mData.size];
		glGetCompressedTexImage(mTexType, 0, static_cast<void*>(mData.data));
	}

	mLocked = !GLError("NSTex2D::lock");
	if (!mLocked)
	{
		mData.bpp = 0;
		mData.size = 0;
		delete[] mData.data;
		mData.data = NULL;
	}
	return mLocked;
}

void NSTex2D::resize(uint32 w, uint32 h)
{
	if (!mAllocated)
	{
		mDim.set(w,h);
		return;
	}
	else if (mImmutable)
		return;

	lock();
	uint32 nsize = w*h*bpp();
	uint8 * cpy = new uint8[nsize];
	for (uint32 i = 0; i < nsize; ++i)
	{
		if (i < mData.size)
			cpy[i] = mData.data[i];
		else
			cpy[i] = 0;
	}
	unlock();

	mAllocated = false;
	mDim.set(w, h);
	allocate(cpy);
	delete[] cpy;
}

bool NSTex2D::unlock()
{
	if (!mLocked) // cannot be locked if not allocated
		return mLocked;

	mLocked = false;
	bool ret = setData(mData.data, 0, mDim);
	mData.bpp = 0;
	mData.size = 0;
	delete[] mData.data;
	mData.data = NULL;
	return ret;
}

/*
Set image data starting from the offset and going until offset + dimensions
If data is bound to PIXEL_UNPACK_BUFFER then data will be used as a byte offset in to
the store data
*/
bool NSTex2D::setData(const void * data, const uivec2 & offset, const uivec2 & dimensions)
{
	if (!mAllocated || mImmutable)
		return false;

	if (mCompByteSize == 0)
		glTexSubImage2D(mTexType, 0, offset.x, offset.y, dimensions.w, dimensions.h, mFormat, mPixelDataType, data);
	else
		glCompressedTexSubImage2D(mTexType, 0, offset.x, offset.y, dimensions.w, dimensions.h, mFormat, mCompByteSize, data);

	return !GLError("NSTex2D::setData");
}

bool NSTex2D::setCompressed(uint32 byteSize)
{
	if (mAllocated)
		return false;
	mCompByteSize = byteSize;
	return true;
}

void NSTex2D::setdim(const uivec2 & dim)
{
	if (mAllocated)
		return;
	mDim = dim;
}

void NSTex2D::setdim(uint32 w, uint32 h)
{
	if (mAllocated)
		return;
	mDim.set(w, h);
}

/*
Copy pixels from currently bound read buffer to existing texture - does not allocate space
*/
bool NSTex2D::setDataFromScreen(const uivec2 & offset, const uivec2 & lowerLeft, const uivec2 & dimensions)
{
	if (!mAllocated || mCompByteSize != 0 || mImmutable)
		return false;

	glCopyTexSubImage2D(mTexType, 0, offset.x, offset.y, lowerLeft.x, lowerLeft.y, dimensions.w, dimensions.h);
	return !GLError("NSTex2D::setDataFromScreen");
}

void NSTex2D::setImmutable(bool immutable)
{
	if (mAllocated)
		return;

	mImmutable = immutable;
}


/*----------------------------------------------------------------------------------------------------------------------------------------*/

NSTex2DArray::NSTex2DArray() : NSTex3D() { mTexType = Tex2DArray; }
NSTex2DArray::~NSTex2DArray(){}

/*----------------------------------------------------------------------------------------------------------------------------------------*/

NSTex3D::NSTex3D() :
mDim(),
mCompByteSize(0),
mImmutable(false),
NSTexture(Tex3D)
{}

NSTex3D::~NSTex3D()
{
	if (mLocked && mData.data != NULL)
		delete[] mData.data;
}

/*
Using width, internal format, pixel data type, format, and mip map level this function
will allocate new texture data to the currently bound texture name. Be sure to bind the texture
before calling allocate.
*/
bool NSTex3D::allocate(const void * data)
{
	if (mAllocated)
		return false;

	if (mCompByteSize == 0)
	{
		if (mImmutable)
			glTexStorage3D(mTexType, mMipMapLevel, mInternalFormat, mDim.x, mDim.y, mDim.z);
		else
			glTexImage3D(mTexType, 0, mInternalFormat, mDim.x, mDim.y, mDim.z, mBorder, mFormat, mPixelDataType, data); // specify base mip map level
	}
	else
	{
		glCompressedTexImage3D(mTexType, 0, mInternalFormat, mDim.x, mDim.y, mDim.z, mBorder, mCompByteSize, data);
	}

	mAllocated = !GLError("NSTex3D::allocate");
	return mAllocated;
}


bool NSTex3D::compressed() const
{
	return (mCompByteSize != 0);
}

const uivec3 & NSTex3D::dim() const
{
	return mDim;
}

void NSTex3D::init()
{
	// do nothing
}

bool NSTex3D::immutable() const
{
	return mImmutable;
}

void NSTex3D::pup(NSFilePUPer * p)
{
	if (p->type() == NSFilePUPer::Binary)
	{
		NSBinFilePUPer * bf = static_cast<NSBinFilePUPer *>(p);
		::pup(*bf, *this);
	}
	else
	{
		NSTextFilePUPer * tf = static_cast<NSTextFilePUPer *>(p);
		::pup(*tf, *this);
	}
}

bool NSTex3D::lock()
{
	mLocked = true;

	if (!mAllocated)
		return mLocked;

	if (mCompByteSize == 0)
	{
		mData.bpp = bpp();
		mData.size = mData.bpp*mDim.x*mDim.y*mDim.z;
		mData.data = new char[mData.size];
		glGetTexImage(mTexType, 0, mFormat, mPixelDataType, static_cast<void*>(mData.data));
	}
	else
	{
		mData.size = mCompByteSize;
		mData.data = new char[mData.size];
		glGetCompressedTexImage(mTexType, 0, static_cast<void*>(mData.data));
	}

	mLocked = !GLError("NSTex3D::lock");
	if (!mLocked)
	{
		mData.bpp = 0;
		mData.size = 0;
		delete[] mData.data;
		mData.data = NULL;
	}
	return mLocked;
}

void NSTex3D::setdim(uint32 w, uint32 h, uint32 layers)
{
	if (mAllocated)
		return;
	mDim.set(w, h, layers);
}

void NSTex3D::resize(uint32 w, uint32 h, uint32 layers)
{
	if (!mAllocated)
	{
		mDim.set(w, h, layers);
		return;
	}
	else if (mImmutable)
		return;

	lock();
	uint32 nsize = w*h*layers*bpp();
	uint8 * cpy = new uint8[nsize];
	for (uint32 i = 0; i < nsize; ++i)
	{
		if (i < mData.size)
			cpy[i] = mData.data[i];
		else
			cpy[i] = 0;
	}
	unlock();

	mAllocated = false;
	mDim.set(w, h, layers);
	allocate(cpy);
	delete[] cpy;
}

bool NSTex3D::unlock()
{
	if (!mLocked) // cannot be locked if not allocated
		return mLocked;

	mLocked = false;
	bool ret = setData(mData.data, 0, mDim);
	mData.bpp = 0;
	mData.size = 0;
	delete[] mData.data;
	mData.data = NULL;
	return ret;
}

/*
Set image data starting from the offset and going until offset + dimensions
If data is bound to PIXEL_UNPACK_BUFFER then data will be used as a byte offset in to
the store data
*/
bool NSTex3D::setData(const void * data, const uivec3 & offset, const uivec3 & dimensions)
{
	if (!mAllocated || mImmutable)
		return false;

	if (mCompByteSize == 0)
		glTexSubImage3D(mTexType, 0, offset.x, offset.y, offset.z, dimensions.x, dimensions.y, dimensions.z, mFormat, mPixelDataType, data);
	else
		glCompressedTexSubImage3D(mTexType, 0, offset.x, offset.y, offset.z, dimensions.x, dimensions.y, dimensions.z, mFormat, mCompByteSize, data);

	return !GLError("NSTex3D::setData");
}

bool NSTex3D::setCompressed(uint32 byteSize)
{
	if (mAllocated)
		return false;
	mCompByteSize = byteSize;
	return true;
}

void NSTex3D::setdim(const uivec3 & dim)
{
	if (mAllocated)
		return;
	mDim = dim;
}

/*
Copy pixels from currently bound read buffer to existing texture - does not allocate space
*/
bool NSTex3D::setDataFromScreen(const uivec3 & offset, const uivec2 & lowerLeft, const uivec2 & dimensions)
{
	if (!mAllocated || mCompByteSize != 0 || mImmutable)
		return false;

	glCopyTexSubImage3D(mTexType, 0, offset.x, offset.y, offset.z, lowerLeft.x, lowerLeft.y, dimensions.w, dimensions.h);
	return !GLError("NSTex3D::setDataFromScreen");
}

void NSTex3D::setImmutable(bool immutable)
{
	if (mAllocated)
		return;

	mImmutable = immutable;
}


/*------------------------------------------------------------------------------------------------------------------------------*/

NSTexCubeMap::NSTexCubeMap() :
mDim(),
mCompByteSize(0),
NSTexture(TexCubeMap)
{}

NSTexCubeMap::~NSTexCubeMap()
{
	if (mLocked && mData.data != NULL)
		delete[] mData.data;
}

/*
Using width, internal format, pixel data type, format, and mip map level this function
will allocate new texture data to the currently bound texture name. Be sure to bind the texture
before calling allocate.
*/
bool NSTexCubeMap::allocate(const void * data)
{
	if (mAllocated)
		return false;

	bool s = true;
	for (uint32 i = PosX; i <= NegZ; i += 1)
	{
		s = s && allocate(CubeFace(i), data);
		mAllocated = false;
	}

	return (mAllocated = s);
}

bool NSTexCubeMap::allocate(CubeFace f, const void * data)
{
	if (mAllocated)
		return false;

	if (mCompByteSize == 0)
		glTexImage2D(f, 0, mInternalFormat, mDim.w, mDim.h, mBorder, mFormat, mPixelDataType, data); // specify base mip map level
	else
		glCompressedTexImage2D(f, 0, mInternalFormat, mDim.w, mDim.h, mBorder, mCompByteSize, data);

	mAllocated = !GLError("NSTexCubeMap::allocateFace");
	return mAllocated;
}

bool NSTexCubeMap::allocateFromScreen(const uivec2 & lowerLeft, const uivec2 dimensions)
{
	if (mAllocated)
		return false;

	if (mCompByteSize != 0) // Cannot perform this operation on a compressed texture
		return false;

	bool s = true;
	for (uint32 i = PosX; i <= NegZ; ++i)
		s = s && allocateFromScreen(CubeFace(i), lowerLeft, dimensions);

	return (mAllocated = s);
}

/*
Allocate new space for a texture and copy screen pixels from whatever read buffer is currently bound
Read buffer must be bound (glReadBuffer) in order to copy screen pixels
\param lowerLeft x and y screen coordinates
\param dimensions width and height starting from lowerLeft
*/
bool NSTexCubeMap::allocateFromScreen(CubeFace f, const uivec2 & lowerLeft, const uivec2 dimensions)
{
	if (mAllocated)
		return false;

	if (mCompByteSize != 0) // Cannot perform this operation on a compressed texture
		return false;

	glCopyTexImage2D(f, 0, mInternalFormat, lowerLeft.x, lowerLeft.y, mDim.w, mDim.h, mBorder);
	mAllocated = !GLError("NSTexture::allocateFromScreen()");
	return mAllocated;
}


bool NSTexCubeMap::compressed() const
{
	return (mCompByteSize != 0);
}

const uivec2 & NSTexCubeMap::dim() const
{
	return mDim;
}

void NSTexCubeMap::init()
{
	// do nothing
}

bool NSTexCubeMap::lock()
{
	mLocked = true;

	if (!mAllocated)
		return mLocked;

	if (mCompByteSize == 0)
	{
		mData.bpp = bpp();
		mData.size = mData.bpp*mDim.w*mDim.h*6;
		mData.data = new char[mData.size];
		for (uint32 i = 0; i < 6; ++i)
			glGetTexImage(PosX + i, 0, mFormat, mPixelDataType, static_cast<void*>(mData.data + i*mData.bpp*mDim.w*mDim.h));
	}
	else
	{
		mData.size = mCompByteSize;
		mData.data = new char[mData.size];
		for (uint32 i = 0; i < 6; ++i)
			glGetCompressedTexImage(PosX + i, 0, static_cast<void*>(mData.data + i*mData.bpp*mDim.w*mDim.h));
	}

	mLocked = !GLError("NSTexCubeMap::lock");
	if (!mLocked)
	{
		mData.bpp = 0;
		mData.size = 0;
		delete[] mData.data;
		mData.data = NULL;
	}
	return mLocked;
}

bool NSTexCubeMap::lock(CubeFace f)
{
	if (mLocked || !mAllocated)
		return mLocked;

	if (mCompByteSize == 0)
	{
		mData.bpp = bpp();
		mData.size = mData.bpp*mDim.w*mDim.h;
		mData.data = new char[mData.size];
		glGetTexImage(f, 0, mFormat, mPixelDataType, static_cast<void*>(mData.data));
	}
	else
	{
		mData.size = mCompByteSize;
		mData.data = new char[mData.size];
		glGetCompressedTexImage(f, 0, static_cast<void*>(mData.data));
	}

	mLocked = !GLError("NSTexCubeMap::lock");
	if (!mLocked)
	{
		mData.bpp = 0;
		mData.size = 0;
		delete[] mData.data;
		mData.data = NULL;
	}
	return mLocked;
}

void NSTexCubeMap::resize(uint32 w, uint32 h)
{
	if (!mAllocated)
	{
		mDim.set(w, h);
		return;
	}

	lock();
	uint32 nsize = w*h*6*bpp();
	uint8 * cpy = new uint8[nsize];
	for (uint32 i = 0; i < nsize; ++i)
	{
		if (i < mData.size)
			cpy[i] = mData.data[i];
		else
			cpy[i] = 0;
	}
	unlock();

	mAllocated = false;
	mDim.set(w, h);
	allocate(cpy);
	delete[] cpy;
}

bool NSTexCubeMap::unlock()
{
	if (!mLocked) // cannot be locked if not allocated
		return mLocked;

	mLocked = false;

	bool s = true;
	for (uint32 i = 0; i < 6; ++i)
		s = s && setData(CubeFace(PosX + i), mData.data + i*(mData.bpp*mDim.x*mDim.y), 0, mDim);

	mData.bpp = 0;
	mData.size = 0;
	delete[] mData.data;
	mData.data = NULL;
	return s;
}

bool NSTexCubeMap::unlock(CubeFace f)
{
	if (!mLocked) // cannot be locked if not allocated
		return mLocked;

	mLocked = false;
	bool ret = setData(f, mData.data, 0, mDim);
	mData.bpp = 0;
	mData.size = 0;
	delete[] mData.data;
	mData.data = NULL;
	return ret;
}

/*
Set image data starting from the offset and going until offset + dimensions
If data is bound to PIXEL_UNPACK_BUFFER then data will be used as a byte offset in to
the store data
*/
bool NSTexCubeMap::setData(const void * data, const uivec2 & offset, const uivec2 & dimensions)
{
	if (!mAllocated)
		return false;

	bool s = true;
	for (uint32 i = PosX; i <= NegZ; ++i)
		s = s && setData(CubeFace(i), data, offset, dimensions);

	return s;
}

bool NSTexCubeMap::setData(CubeFace f, const void * data, const uivec2 & offset, const uivec2 & dimensions)
{
	if (!mAllocated)
		return false;

	if (mCompByteSize == 0)
		glTexSubImage2D(f, 0, offset.x, offset.y, dimensions.w, dimensions.h, mFormat, mPixelDataType, data);
	else
		glCompressedTexSubImage2D(f, 0, offset.x, offset.y, dimensions.w, dimensions.h, mFormat, mCompByteSize, data);

	return !GLError("NSTexCubeMap::setData");
}

bool NSTexCubeMap::setCompressed(uint32 byteSize)
{
	if (mAllocated)
		return false;
	mCompByteSize = byteSize;
	return true;
}

void NSTexCubeMap::setdim(const uivec2 & dim)
{
	if (mAllocated)
		return;
	mDim = dim;
}

void NSTexCubeMap::setdim(uint32 w, uint32 h)
{
	if (mAllocated)
		return;
	mDim.set(w, h);
}

/*
Copy pixels from currently bound read buffer to existing texture - does not allocate space
*/
bool NSTexCubeMap::setDataFromScreen(const uivec2 & offset, const uivec2 & lowerLeft, const uivec2 & dimensions)
{
	if (!mAllocated || mCompByteSize != 0)
		return false;

	bool s = true;
	for (uint32 i = PosX; i <= NegZ; ++i)
		s = s && setDataFromScreen(CubeFace(i), offset, lowerLeft, dimensions);

	return s;
}

bool NSTexCubeMap::setDataFromScreen(CubeFace f, const uivec2 & offset, const uivec2 & lowerLeft, const uivec2 & dimensions)
{
	if (!mAllocated || mCompByteSize != 0)
		return false;

	glCopyTexSubImage2D(f, 0, offset.x, offset.y, lowerLeft.x, lowerLeft.y, dimensions.w, dimensions.h);
	return !GLError("NSTexCubeMap::setDataFromScreen");
}

void NSTexCubeMap::pup(NSFilePUPer * p)
{
	if (p->type() == NSFilePUPer::Binary)
	{
		NSBinFilePUPer * bf = static_cast<NSBinFilePUPer *>(p);
		::pup(*bf, *this);
	}
	else
	{
		NSTextFilePUPer * tf = static_cast<NSTextFilePUPer *>(p);
		::pup(*tf, *this);
	}
}

/*-------------------------------------------------------------------------------------------------------------------------------------------------*/

NSTexRectangle::NSTexRectangle() { mTexType = TexRectangle; }
NSTexRectangle::~NSTexRectangle() {}

NSTexCubeMapArray::NSTexCubeMapArray() { mTexType = TexCubeMapArray; }
NSTexCubeMapArray::~NSTexCubeMapArray() {}

NSTex2DMultisample::NSTex2DMultisample() { mTexType = Tex2DMultisample;}
NSTex2DMultisample::~NSTex2DMultisample() {}

NSTex2DMultisampleArray::NSTex2DMultisampleArray() { mTexType = Tex2DMultisampleArray; }
NSTex2DMultisampleArray::~NSTex2DMultisampleArray() {}

NSTexBuffer::NSTexBuffer():NSTexture(TexBuffer) {}
NSTexBuffer::~NSTexBuffer() {}

bool NSTexBuffer::allocate(const void * data)
{
	return false;
}

void NSTexBuffer::init()
{
	// do nothing
}

bool NSTexBuffer::lock()
{
	return false;
}

bool NSTexBuffer::unlock()
{
	return false;
}
