/*! 
	\file nsframebuffer.h
	
	\brief Header file for NSFrameBuffer class

	This file contains all of the neccessary declarations for the NSFrameBuffer class.

	\author Daniel Randle
	\date November 2 2013
	\copywrite Earth Banana Games 2013
*/

#ifndef NSFRAMEBUFFER_H
#define NSFRAMEBUFFER_H

#include <nsgl_object.h>
#include <myGL/glew.h>
#include <vector>
#include <nstexture.h>
#include <nsengine.h>

class NSFrameBuffer : public NSGLObject
{
public:

	class NSRenderBuffer : public NSGLObject
	{
	public:

		NSRenderBuffer();

		~NSRenderBuffer();

		void allocate();

		void bind();

		int32 internalFormat() const;

		const uivec2 & dim();

		uint32 multisample() const;

		void initGL();

		void release();

		void resize(int32 w, int32 h);

		void setMultisample(uint32 numsamples);

		void setdim(uint32 w, uint32 h);

		void setInternalFormat(int32 pInternalFormat);

		void unbind();

	private:
		uint32 mSampleNumber;
		int32 mInternalFormat;
		uivec2 mDim;
		bool mAllocated;
	};

	enum AttachmentPoint {  
		None = GL_NONE,
		Color = GL_COLOR_ATTACHMENT0,
		Depth = GL_DEPTH_ATTACHMENT,
		Stencil = GL_STENCIL_ATTACHMENT,
		DepthStencil = GL_DEPTH_STENCIL_ATTACHMENT };

	enum Target {
		Read = GL_READ_FRAMEBUFFER,
		Draw = GL_DRAW_FRAMEBUFFER,
		ReadAndDraw = GL_FRAMEBUFFER };

	struct Attachment
    {
		Attachment();
		~Attachment();
		bool valid();

		NSTexture * mTexture;
		NSRenderBuffer * mRenderBuffer;
		AttachmentPoint mAttPoint;
		uint32 mTexUnit;
		uint32 mOwningFB;
	};

	typedef std::vector<Attachment*> AttachmentArray;
	typedef std::vector<GLenum> AttachmentPointArray;

	NSFrameBuffer();
	~NSFrameBuffer();

	// On attachment the frame buffer is responsible for destroying the texture
	bool add(Attachment * pAttachment, bool pOverwrite=false);

	void bind();

	Attachment * create(AttachmentPoint pAttPoint, uint32 pSampleNumber, int32 pInternalFormat, bool overwrite = true);

	template<class TexType>
	Attachment * create(const nsstring & pName, AttachmentPoint pAttPoint, uint32 pTexUnit, int32 pInternalFormat, int32 pFormat, int32 pPixelDataType)
	{
		TexType * tex = new TexType();

		// Set up the texture according to the format provided in the function arguements
		tex->initGL();
		tex->rename(pName);
		tex->setFormat(pFormat);
		tex->setInternalFormat(pInternalFormat);
		tex->setPixelDataType(pPixelDataType);
		tex->setdim(mDim);
		tex->bind();
		// Fill the texture with NULL data
		tex->allocate(NULL);

		// Render buffer is left as NULL to indicate that we are using a texture at this attachment location on not a render buffer
		Attachment * att = new Attachment();
		att->mAttPoint = pAttPoint;
		att->mTexture = tex;
		att->mOwningFB = mGLName;
		att->mTexUnit = pTexUnit;

		if (!add(att, true))
		{
			dprint("NSFrameBuffer::create Could not add tex attachment " + pName);
			delete att;
			att = NULL;
		}

		return att;
	}

	void disableTextures();

	void enableTextures();

    // Will return attachment with NULL as renderbuffer pointer and texture pointer if
    // there is no attachment at the attachment point
	Attachment * get(AttachmentPoint pAttPoint);

	const uivec2 & dim();

	Target target();

	bool has(AttachmentPoint pAttPoint);

	virtual void initGL();

	virtual void release();

	virtual void resize(uint32 w, uint32 h=0, uint32 layers=0);

	bool setCubeface(AttachmentPoint pAttPoint, NSTexCubeMap::CubeFace pFace);

	void setDrawBuffer(AttachmentPoint pAttPoint);

	void setDrawBuffers(AttachmentPointArray * pAttArray);

	void setReadBuffer(AttachmentPoint pAttPoint);

	void setTarget(Target pTarget);

	void setdim(uint32 w, uint32 h);

	void unbind();

	void updateDrawBuffers();

protected:

	Target mTarget;
	uivec2 mDim;

	Attachment * mDepthStencilAttachment;
	AttachmentArray mColorAttachments;
};

class NSShadowBuffer
{
public:
	enum MapType {
		Direction,
		Point,
		Spot
	};

	NSShadowBuffer();
	~NSShadowBuffer();

	void bind(const MapType & pMType);

	void disable(const MapType & pMType);

	void enable(const MapType & pMType);

	NSTexture * get(const MapType & pMap);

	const uivec2 & dim(const MapType & pMap);

	NSFrameBuffer * fb(const MapType & pMap);

	void setfb(NSFrameBuffer * fb, const MapType & map);

	void init();

	void resize(const MapType & pMap, uint32 w, uint32 h);

	void resize(uint32 w, uint32 h);

	void setdim(uint32 w, uint32 h);

	void setdim(const MapType & pMap, uint32 w, uint32 h);

	void setPointface(const NSTexCubeMap::CubeFace & pFace);

private:
	NSFrameBuffer * mSpotBuf;
	NSFrameBuffer * mPointBuf;
	NSFrameBuffer * mDirBuf;
};


class NSGBuffer
{
public:
	enum ColorAttachmentType {
		Diffuse,
		Picking,
		Position,
		Normal,
		Material,
		AttCount };

	NSGBuffer();
	~NSGBuffer();

	void bind();

	void debugBlit(const ivec2 & scrn);

	void disableTextures();

	void enableTextures();

	void enableColorWrite(bool pEnable);

	NSFrameBuffer::Attachment * color(ColorAttachmentType pType);

	NSFrameBuffer::Attachment * depth();

	const uivec2 & fbdim();

	NSFrameBuffer * fb();

	uivec3 pick(float mousex, float mousey);

	const uivec2 & screendim();

	void init();

	void resizefb(uint32 w, uint32 h);

	void resizescreen(uint32 w, uint32 h);

	void setscreendim(uint32 w, uint32 h);

	void setscreendim(const uivec2 & dim);

	void setfb(NSFrameBuffer * fb);

	void setfbdim(uint32 w, uint32 h);

	void setfbdim(const uivec2 & dim);

	void unbind();

private:
	bool _createTextureAttachments();
	NSFrameBuffer * mTexFrameBuffer;
	uivec2 mScreendim;
	uint32 mMultisampleLevel;
};


#endif
