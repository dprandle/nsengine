/*! 
	\file nstexture.h
	
	\brief Header file for NSTexture class

	This file contains all of the neccessary declarations for the NSTexture class.

	\author Daniel Randle
	\date November 23 2013
	\copywrite Earth Banana Games 2013
*/

// Internal Format : Bytes per pixel(ie 3 or 4)
// Format : enum specifying order of those byes - ie RGBA or RBGA etc
// Pixel Data Type : the data type assigned to each component of the pixe(ie unsigned byte)


#ifndef NSTEXTURE_H
#define NSTEXTURE_H

// Includes
#include <myGL\glew.h>
#include <vector>
#include <nsresource.h>
#include <nsglobject.h>
#include <nsmath.h>
#include <nsglobal.h>
#include <nspupper.h>

class NSTexture : public NSResource, public NSGLObject
{
public:
	struct ImageData
	{
		ImageData(nschar * data_, nsuint size_, nsuint bpp_) :data(data_), size(size_), bpp(bpp_){}

		nschar * data;
		nsuint size;
		nsuint bpp;
	};

	enum TexType {
		Tex1D = GL_TEXTURE_1D,
		Tex1DArray = GL_TEXTURE_1D_ARRAY,
		Tex2D = GL_TEXTURE_2D,
		Tex2DArray = GL_TEXTURE_2D_ARRAY,
		Tex3D = GL_TEXTURE_3D,
		TexRectangle = GL_TEXTURE_RECTANGLE,
		TexBuffer = GL_TEXTURE_BUFFER,
		TexCubeMap = GL_TEXTURE_CUBE_MAP,
		TexCubeMapArray = GL_TEXTURE_CUBE_MAP_ARRAY,
		Tex2DMultisample = GL_TEXTURE_2D_MULTISAMPLE,
		Tex2DMultisampleArray = GL_TEXTURE_2D_MULTISAMPLE_ARRAY
	};

	enum TexParameter {
		DepthStencilTextureMode = GL_DEPTH_STENCIL_TEXTURE_MODE,
		BaseLevel = GL_TEXTURE_BASE_LEVEL,
		MaxLevel = GL_TEXTURE_MAX_LEVEL,
		CompareFunc = GL_TEXTURE_COMPARE_FUNC,
		CompareMode = GL_TEXTURE_COMPARE_MODE,
		LODBias = GL_TEXTURE_LOD_BIAS,
		MinFilter = GL_TEXTURE_MIN_FILTER,
		MagFilter = GL_TEXTURE_MAG_FILTER,
		MinLOD = GL_TEXTURE_MIN_LOD,
		MaxLOD = GL_TEXTURE_MAX_LOD,
		SwizzleR = GL_TEXTURE_SWIZZLE_R,
		SwizzleG = GL_TEXTURE_SWIZZLE_G,
		SwizzleB = GL_TEXTURE_SWIZZLE_B,
		SwizzleA = GL_TEXTURE_SWIZZLE_A,
		WrapS = GL_TEXTURE_WRAP_S,
		WrapT = GL_TEXTURE_WRAP_T,
		WrapR = GL_TEXTURE_WRAP_R };

	enum GetTexParam 
	{
		Width = GL_TEXTURE_WIDTH, 
		Height = GL_TEXTURE_HEIGHT,
		Depth = GL_TEXTURE_DEPTH,
		InternalFormat = GL_TEXTURE_INTERNAL_FORMAT, 
		Border = GL_TEXTURE_BORDER, 
		RedSize = GL_TEXTURE_RED_SIZE, 
		GreenSize = GL_TEXTURE_GREEN_SIZE, 
		BlueSize = GL_TEXTURE_BLUE_SIZE, 
		AlphaSize = GL_TEXTURE_ALPHA_SIZE, 
		LuminanceSize = GL_TEXTURE_LUMINANCE_SIZE,
		IntensitySize = GL_TEXTURE_INTENSITY_SIZE, 
		DepthSize = GL_TEXTURE_DEPTH_SIZE, 
		Compressed = GL_TEXTURE_COMPRESSED,
		CompressedSize = GL_TEXTURE_COMPRESSED_IMAGE_SIZE
	};

	template <class PUPer>
	friend void pup(PUPer & p, NSTexture & sm);

	NSTexture(TexType type);
	~NSTexture();

	void initGL();

	void bind();

	/*
	Get the number of bytes per pixel using opengl format and type parameters
	*/
	nsuint bpp();

	nsuint channels();

	void disable(nsuint pTexUnit);

	virtual bool allocate(const nsvoid * data) = 0;

	void setAllocated(bool alloc);

	bool allocated();

	virtual ImageData data();

	nsint format() const;

	nsint pixelDataType() const;

	virtual nsbool lock() = 0;

	nsbool locked() const;

	virtual nsbool unlock() = 0;

	nsint internalFormat() const;

	void enable(nsuint pTexUnit);

	nsuint border() const;

	nsfloat getParameterf(GetTexParam p);

	nsint getParameteri(GetTexParam p);

	/*!
	Enable mipmaps for this texture - level of 0 means use the default max mip map level
	If you want to disable mipmaps you need to create a new texture with them disabled
	If the texture has been allocated then this will call generate mip map function
	If it has not, it will call generate mip map function when it is allocated
	\param level how many mip map levels to include
	*/
	void enableMipMaps(nsint level = 0);

	virtual void pup(NSFilePUPer * p);

	/*
	Release the opengl image name
	*/
	void release();

	void setBorder(nsuint border);

	TexType textureType() const;

	/*
	Set the format - see opengl specs for which formats are allowable - this determines how pixel data is read in
	when we allocate or how it is returned when we get the texture data from the GPU

	\param OpenGL format used when uploading or downloading pixel data from a stored image on the GPU
	*/
	void setFormat(nsint format);

	/*
	Set the texture internal format - it is not gaurenteed however that the image will be stored in this format - but it
	has a good inlfuence on what the driver will do.

	As per the specs, image data can be stored internally as unsigned or signed integers, floats, or unsigned/signed normalized integers.
	Floats resolve in the shader to a vector of floats, matching exactly what they are internally. Unsigned integers will resolve in to a vector of unsigned integers
	in the shader, and signed integers will resolve in to a vector of signed integers. Normalized unsigned integers will resolve in to a vector of floats
	between 0.0 and 1.0 in the shader. Each integer value is divided by the max possible integer value for that component's bitdepth. For signed normalized,
	it will resolve to a vector of floats between -1.0 and 1.0 with each positive value being divided my the max signed integer value, and each negative value
	being divided by the most negative integer value for that component's bitdepth. Usually the ditdepth is the same for all components (where r g b and a are considered
	components).

	You dont have to store all of the components of the texture internally - if a texture missing certain components is sampled in a shader, the missing components
	will resolve to 0 except for alpha which will resolve to 1. The sampled color value will always be a 4 element vector (rgba).

	GL_DEPTH_COMPONENT_16, _24, _32, and _32F are an example of where only one component is stored internally - the cool thing about these texture formats is that
	they can be sampled with shadow samplers. The integer formats (all without the F) are all normalized.

	This function will do nothing if the texture data has already been allocated

	\param internalFormat The opengl acceptable image format
	*/
	void setInternalFormat(nsint intformat);

	void setParameterf(TexParameter param, nsfloat pValue);

	void setParameteri(TexParameter param, nsint pValue);

	void setParameterfv(TexParameter param, const fvec4 & val);

	void setParameteriv(TexParameter param, const ivec4 & val);

	void setParameterIiv(TexParameter param, const ivec4 & val);

	void setParameterIiuv(TexParameter param, const uivec4 & val);

	void setGLName(nsuint glid);

	/*
	Set the pixel data type - see opengl doc for what types are acceptable
	The is the type for each component given in format - if a single type such as GL_UNSIGNED_BYTE is specified then all of the components
	given in the format are of that type. Sometimes images have different bit sizes for each component however, such as 16 bit RGB where the
	red and blue channels are each 5 bits and the green channel is 6 bits. In this case the format would be GL_RGB and the type would be
	GL_UNSIGNED_SHORT_5_6_5

	The specification is GL_[base type]_[size1]_[size2]_[size3]_[size4](_REV)

	The _REV reversed the order the components are put in the format.. ie if the previous example was GL_UNSIGNED_SHORT_5_5_6_REV with format GL_RGB, this would
	be equivalent to GL_UNSIGNED_SHORT_5_5_6 with format as GL_BGR

	The base type must be some type that is large enough to store all of the 4 components
	*/
	void setPixelDataType(nsint pType);

	void unbind();

protected:
	/*
	Returns the pixel size in bytes using the format
	*/
	void _pixelSize() const;

	TexType mTexType;
	bool mAllocated;
	bool mLocked;
	GLint mFormat;
	GLint mInternalFormat;
	GLint mPixelDataType;
	nsint mMipMapLevel;
	nsbool mGenMipmaps;
	nsuint mBorder;
	ImageData mData;
};

template <class PUPer>
void pup(PUPer & p, NSTexture::ImageData & dat, const nsstring & varName)
{
	pup(p, dat.bpp, varName + ".bpp");
	pup(p, dat.size, varName + ".size");
	for (nsuint i = 0; i < dat.size; ++i)
		pup(p, dat.data[i], ".data[" + std::to_string(i) + "]");
}

template<class PUPer>
void pup(PUPer & p, NSTexture::TexType & en, const nsstring & pString)
{
	nsuint in = static_cast<nsuint>(en);
	pup(p, in, pString);
	en = static_cast<NSTexture::TexType>(in);
}

template <class PUPer>
void pup(PUPer & p, NSTexture & sm)
{
	pup(p, sm.mTexType,"texType");
	pup(p, sm.mFormat,"format");
	pup(p, sm.mInternalFormat,"internalFormat");
	pup(p, sm.mPixelDataType,"pixelDataType");
	pup(p, sm.mMipMapLevel,"mipMapLevel");
	pup(p, sm.mGenMipmaps,"genMipmaps");
	pup(p, sm.mBorder,"border");
	if (p.mode() == PUP_IN)
	{
		if (sm.mGLName == 0)
			sm.initGL();
		sm.bind();
		sm.allocate(NULL);
		sm.unbind();
	}
	sm.lock();
	pup(p, sm.mData,"data");
	sm.unlock();
}

template <class PUPer>
void pup(PUPer & p, NSTexture * sm)
{
	pup(p, *sm);
}

// Save using raw data
class NSTex1D : public NSTexture
{
public:
	template <class PUPer>
	friend void pup(PUPer & p, NSTex1D & sm);

	NSTex1D();
	~NSTex1D();

	/*
	Using width, internal format, pixel data type, format, and mip map level this function
	will allocate new texture data to the currently bound texture name. Be sure to bind the texture
	before calling allocate.
	*/
	bool allocate(const nsvoid * data);

	/*
	Allocate new space for a texture and copy screen pixels from whatever read buffer is currently bound
	Read buffer must be bound (glReadBuffer) in order to copy screen pixels
	\param lowerLeft x and y screen coordinates
	\param dim width and height starting from lowerLeft
	*/
	bool allocateFromScreen(const uivec2 & lowerLeft, const uivec2 dimensions);

	nsbool compressed() const;

	void init();

	nsbool immutable() const;

	/*!
	Lock the texture from other operations and download the pixel data for editing...
	Returns true if the data is downloaded and false otherwise, or if the texture is not allocated
	Returns true if the texture is already locked
	*/
	nsbool lock();

	virtual void pup(NSFilePUPer * p);

	virtual void resize(nsuint w);

	/*!
	Unlock the texture and upload the pixel data back to the GPU. Returns true if the data is uploaded without error,
	or else it returns false. It also returns 
	*/
	nsbool unlock();

	nsbool setCompressed(nsuint byteSize);

	/*
	Set image data starting from the offset and going until offset + dimensions
	If data is bound to PIXEL_UNPACK_BUFFER then data will be used as a byte offset in to
	the store data
	\param offset x and y offset in to the image
	\param dimensions width and height of the sub image
	\data Pixel data for the image
	*/
	nsbool setData(const nsvoid * data, nsuint xoffset, nsuint width);

	/*
	Copy pixels from currently bound read buffer to existing texture - does not allocate space
	\param offset the offset in elements 
	*/
	nsbool setDataFromScreen(nsuint xoffset, const uivec2 & lowerLeft, nsuint width);

	void setImmutable(nsbool immutable);

	void setWidth(nsuint width);

	nsuint width();

private:
	nsuint mWidth;
	nsuint mCompByteSize;
	nsbool mImmutable;
};

template <class PUPer>
void pup(PUPer & p, NSTex1D & sm)
{
	pup(p, sm.mWidth,"width");
	pup(p, sm.mCompByteSize, "compByteSize");
	pup(p, sm.mImmutable, "immutable");
	pup(p, static_cast<NSTexture*>(&sm));
}

template <class PUPer>
void pup(PUPer & p, NSTex1D * sm)
{
	pup(p, *sm);
}

class NSTex2D : public NSTexture
{
public:
	template <class PUPer>
	friend void pup(PUPer & p, NSTex2D & sm);

	NSTex2D();
	~NSTex2D();
	/*
	Using width, internal format, pixel data type, format, and mip map level this function
	will allocate new texture data to the currently bound texture name. Be sure to bind the texture
	before calling allocate.
	*/
	bool allocate(const nsvoid * data);

	/*
	Allocate new space for a texture and copy screen pixels from whatever read buffer is currently bound
	Read buffer must be bound (glReadBuffer) in order to copy screen pixels
	\param lowerLeft x and y screen coordinates
	\param dimensions width and height starting from lowerLeft
	*/
	bool allocateFromScreen(const uivec2 & lowerLeft, const uivec2 dimensions);

	nsbool compressed() const;

	const uivec2 & dim() const;

	nsbool immutable() const;

	void init();

	/*!
	Lock the texture from other operations and download the pixel data for editing...
	Returns true if the data is downloaded and false otherwise, or if the texture is not allocated
	Returns true if the texture is already locked
	*/
	nsbool lock();

	virtual void pup(NSFilePUPer * p);

	virtual void resize(nsuint w, nsuint h);

	/*!
	Unlock the texture and upload the pixel data back to the GPU. Returns true if the data is uploaded without error,
	or else it returns false. It also returns
	*/
	nsbool unlock();

	nsbool setCompressed(nsuint byteSize);

	/*
	Set image data starting from the offset and going until offset + dimensions
	If data is bound to PIXEL_UNPACK_BUFFER then data will be used as a byte offset in to
	the store data
	\param offset x and y offset in to the image
	\param dimensions width and height of the sub image
	\data Pixel data for the image
	*/
	nsbool setData(const nsvoid * data, const uivec2 & offset, const uivec2 & dimensions);

	void setdim(const uivec2 & dim);

	void setdim(nsuint w, nsuint h);

	void setImmutable(nsbool immutable);

	/*
	Copy pixels from currently bound read buffer to existing texture - does not allocate space
	\param offset the offset in elements
	*/
	nsbool setDataFromScreen(const uivec2 & offset, const uivec2 & lowerLeft, const uivec2 & dimensions);

private:
	uivec2 mDim;
	nsuint mCompByteSize;
	nsbool mImmutable;
};

template <class PUPer>
void pup(PUPer & p, NSTex2D & sm)
{
	pup(p, sm.mDim, "dim");
	pup(p, sm.mCompByteSize, "compByteSize");
	pup(p, sm.mImmutable, "immutable");
	pup(p, static_cast<NSTexture*>(&sm));
}

template <class PUPer>
void pup(PUPer & p, NSTex2D * sm)
{
	pup(p, *sm);
}

class NSTex1DArray : public NSTex2D
{
public:
	NSTex1DArray();
	~NSTex1DArray();
};

class NSTex3D : public NSTexture
{
public:
	template <class PUPer>
	friend void pup(PUPer & p, NSTex3D & sm);

	NSTex3D();
	~NSTex3D();
	/*
	Using width, internal format, pixel data type, format, and mip map level this function
	will allocate new texture data to the currently bound texture name. Be sure to bind the texture
	before calling allocate.
	*/
	bool allocate(const nsvoid * data);

	nsbool compressed() const;

	const uivec3 & dim() const;

	nsbool immutable() const;

	void init();

	/*!
	Lock the texture from other operations and download the pixel data for editing...
	Returns true if the data is downloaded and false otherwise, or if the texture is not allocated
	Returns true if the texture is already locked
	*/
	nsbool lock();

	virtual void pup(NSFilePUPer * p);

	/*!
	Unlock the texture and upload the pixel data back to the GPU. Returns true if the data is uploaded without error,
	or else it returns false. It also returns
	*/
	nsbool unlock();

	virtual void resize(nsuint w, nsuint h, nsuint layers);

	nsbool setCompressed(nsuint byteSize);

	/*
	Set image data starting from the offset and going until offset + dimensions
	If data is bound to PIXEL_UNPACK_BUFFER then data will be used as a byte offset in to
	the store data
	\param offset x and y offset in to the image
	\param dimensions width and height of the sub image
	\data Pixel data for the image
	*/
	nsbool setData(const nsvoid * data, const uivec3 & offset, const uivec3 & dimensions);

	void setdim(const uivec3 & dim);

	void setdim(nsuint w, nsuint h, nsuint layers);

	/*
	Copy pixels from currently bound read buffer to existing texture - does not allocate space
	\param offset the offset in elements
	*/
	nsbool setDataFromScreen(const uivec3 & offset, const uivec2 & lowerLeft, const uivec2 & dimensions);

	void setImmutable(nsbool immutable);

private:
	uivec3 mDim;
	nsuint mCompByteSize;
	nsbool mImmutable;
};

template <class PUPer>
void pup(PUPer & p, NSTex3D & sm)
{
	pup(p, sm.mDim, "dim");
	pup(p, sm.mCompByteSize, "compByteSize");
	pup(p, sm.mImmutable, "immutable");
	pup(p, static_cast<NSTexture*>(&sm));
}

template <class PUPer>
void pup(PUPer & p, NSTex3D * sm)
{
	pup(p, *sm);
}

class NSTex2DArray : public NSTex3D
{
public:
	NSTex2DArray();
	~NSTex2DArray();
};

class NSTexCubeMap : public NSTexture
{
public:

	enum CubeFace {
		PosX = GL_TEXTURE_CUBE_MAP_POSITIVE_X,
		NegX = GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
		PosY = GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
		NegY = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
		PosZ = GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
		NegZ = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
	};

	template <class PUPer>
	friend void pup(PUPer & p, NSTexCubeMap & sm);

	NSTexCubeMap();
	~NSTexCubeMap();

	/*
	Using width, internal format, pixel data type, format, and mip map level this function
	will allocate new texture data to the currently bound texture name. Be sure to bind the texture
	before calling allocate.
	*/
	bool allocate(const nsvoid * data);

	bool allocate(CubeFace f, const nsvoid * data);


	bool allocateFromScreen(const uivec2 & lowerLeft, const uivec2 dim);
	/*
	Allocate new space for a texture and copy screen pixels from whatever read buffer is currently bound
	Read buffer must be bound (glReadBuffer) in order to copy screen pixels
	\param lowerLeft x and y screen coordinates
	\param dimensions width and height starting from lowerLeft
	*/
	bool allocateFromScreen(CubeFace f, const uivec2 & lowerLeft, const uivec2 dimensions);

	nsbool compressed() const;

	const uivec2 & dim() const;

	void init();

	nsbool lock();

	/*!
	Lock the texture from other operations and download the pixel data for editing...
	Returns true if the data is downloaded and false otherwise, or if the texture is not allocated
	Returns true if the texture is already locked
	*/
	nsbool lock(CubeFace f);

	virtual void pup(NSFilePUPer * p);

	virtual void resize(nsuint w, nsuint h);


	nsbool unlock();

	/*!
	Unlock the texture and upload the pixel data back to the GPU. Returns true if the data is uploaded without error,
	or else it returns false. It also returns
	*/
	nsbool unlock(CubeFace f);

	nsbool setCompressed(nsuint byteSize);

	/*
	Set image data starting from the offset and going until offset + dimensions
	If data is bound to PIXEL_UNPACK_BUFFER then data will be used as a byte offset in to
	the store data
	\param offset x and y offset in to the image
	\param dimensions width and height of the sub image
	\data Pixel data for the image
	*/
	nsbool setData(const nsvoid * data, const uivec2 & offset, const uivec2 & dimensions);

	nsbool setData(CubeFace f, const nsvoid * data, const uivec2 & offset, const uivec2 & dimensions);

	void setdim(const uivec2 & dim);

	void setdim(nsuint w, nsuint h);

	/*
	Copy pixels from currently bound read buffer to existing texture - does not allocate space
	\param offset the offset in elements
	*/
	nsbool setDataFromScreen(const uivec2 & offset, const uivec2 & lowerLeft, const uivec2 & dimensions);

	nsbool setDataFromScreen(CubeFace f, const uivec2 & offset, const uivec2 & lowerLeft, const uivec2 & dimensions);

private:
	uivec2 mDim;
	nsuint mCompByteSize;
};

template <class PUPer>
void pup(PUPer & p, NSTexCubeMap & sm)
{
	pup(p, sm.mDim, "dim");
	pup(p, sm.mCompByteSize, "compByteSize");
	pup(p, static_cast<NSTexture*>(&sm));
}


template <class PUPer>
void pup(PUPer & p, NSTexCubeMap * sm)
{
	pup(p, *sm);
}

class NSTexRectangle : public NSTex2D
{
public:
	NSTexRectangle();
	~NSTexRectangle();
};

class NSTexCubeMapArray : public NSTex3D
{
public:
	NSTexCubeMapArray();
	~NSTexCubeMapArray();
};

class NSTex2DMultisample : public NSTex2D
{
public:
	NSTex2DMultisample();
	~NSTex2DMultisample();
};

class NSTex2DMultisampleArray : public NSTex3D
{
public:
	NSTex2DMultisampleArray();
	~NSTex2DMultisampleArray();
};

class NSTexBuffer : public NSTexture
{
public:
	NSTexBuffer();
	~NSTexBuffer();

	bool allocate(const nsvoid * data);

	void init();

	nsbool lock();

	nsbool unlock();
};

#endif
