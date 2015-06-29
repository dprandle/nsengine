/*! 
	\file nsframebuffer.cpp
	
	\brief Definition file for NSFrameBuffer class

	This file contains all of the neccessary definitions for the NSFrameBuffer class.

	\author Daniel Randle
	\date November 2 2013
	\copywrite Earth Banana Games 2013
*/

#include <nsframebuffer.h>
#include <nsresmanager.h>
#include <nsengine.h>

NSFrameBuffer::NSFrameBuffer() : mTarget(ReadAndDraw),
mDim(),
mDepthStencilAttachment(NULL),
mColorAttachments(),
NSGLObject()
{}

NSFrameBuffer::~NSFrameBuffer()
{}

bool NSFrameBuffer::add(Attachment * pAttachment, bool pOverwrite)
{
	if (pAttachment == NULL)
		return false;
    // Make sure that there isn't anything attached to this texture point already...
    // And also make sure this attachment is valid
	if ((has(pAttachment->mAttPoint) && !pOverwrite) || !pAttachment->valid())
		return false;

	if (has(pAttachment->mAttPoint) && get(pAttachment->mAttPoint)->mTexture == pAttachment->mTexture)
		return false;

	//bind();
	if (pAttachment->mRenderBuffer != NULL)
		// Render buffers always must use the GL_RENDERBUFFER flag.. so I will just pass it explicitly here (no wrapper name)
		glFramebufferRenderbuffer(mTarget, pAttachment->mAttPoint, GL_RENDERBUFFER, pAttachment->mRenderBuffer->glid());
	else
	{
		// I dont see the point in making multiple mipmap levels for a texture right now.. so I am just
		// going to leave this hardcoded as mip map level 0

		// If attaching a cubemap attach the posX face by default - can be changed with change cubemap face function
		if (pAttachment->mTexture->textureType() == NSTexture::TexCubeMap)
			glFramebufferTexture(mTarget, pAttachment->mAttPoint, pAttachment->mTexture->glid(), 0);
		else
			glFramebufferTexture2D(mTarget, pAttachment->mAttPoint, pAttachment->mTexture->textureType(), pAttachment->mTexture->glid(),0);
	}

	GLError("NSFrameBuffer::addAttachment");

	// I do not need to check if I'm overwriting here because it would have failed already
	// If it is a depth attachment of any kind attach it to the depth/stencil attachment.. otherwise it is a color attachment
	if (pAttachment->mAttPoint == Depth || pAttachment->mAttPoint == Stencil || pAttachment->mAttPoint == DepthStencil)
		mDepthStencilAttachment = pAttachment;
	else
		mColorAttachments.push_back(pAttachment);

	GLError("NSFrameBuffer::addAttachment");

#ifdef NSDEBUG_RT
	// Check for buffer completeness - should catch any mipmap errors etc..
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		nsstringstream ss;
		ss << "NSFrameBuffer::add Error in adding attachment - Error Code: " << status;
		dprint(ss.str());
	}
#endif

	return true;
}

void NSFrameBuffer::bind()
{
	// use mTarget - easily set with setTarget
	glBindFramebuffer(mTarget, mGLName);
	GLError("NSFrameBuffer::bind");
}

NSFrameBuffer::Attachment * NSFrameBuffer::create(AttachmentPoint pAttPoint, nsuint pSampleNumber, nsint pInternalFormat, bool overwrite)
{
	NSRenderBuffer * rBuf = new NSRenderBuffer();

	// Set up render buffer
	rBuf->initGL();
	rBuf->setInternalFormat(pInternalFormat);
	rBuf->setMultisample(pSampleNumber);
	rBuf->setdim(mDim.w, mDim.h);
	rBuf->bind();
	// Allocate the necessary space for the render buffer
	rBuf->allocate();

	// Texture is left as NULL to indicate that we are using a texture at this attachment location on not a render buffer
	Attachment * att = new Attachment();
	att->mAttPoint = pAttPoint;
	att->mRenderBuffer = rBuf;
	att->mOwningFB = mGLName;

	if (!add(att, overwrite))
	{
		dprint("NSFrameBuffer::Attachment unable to add attachment at " + att->mAttPoint);
		delete att;
		att = NULL;
	}

	return att;
}

void NSFrameBuffer::disableTextures()
{
	// Remember that disabling first sets the texture unit
	AttachmentArray::iterator iter = mColorAttachments.begin();
	while (iter != mColorAttachments.end())
	{
		if ((*iter)->mTexture != NULL)
			(*iter)->mTexture->disable((*iter)->mTexUnit);
		++iter;
	}
	if (mDepthStencilAttachment != NULL && mDepthStencilAttachment->mTexture != NULL)
		mDepthStencilAttachment->mTexture->disable(mDepthStencilAttachment->mTexUnit);
}


void  NSFrameBuffer::enableTextures()
{
	AttachmentArray::iterator iter = mColorAttachments.begin();
	while (iter != mColorAttachments.end())
	{
		if ((*iter)->mTexture != NULL)
			(*iter)->mTexture->enable((*iter)->mTexUnit);
		++iter;
	}
	if (mDepthStencilAttachment != NULL && mDepthStencilAttachment->mTexture != NULL)
		mDepthStencilAttachment->mTexture->enable(mDepthStencilAttachment->mTexUnit);
}

NSFrameBuffer::Attachment * NSFrameBuffer::get(AttachmentPoint pAttPoint)
{
	if (pAttPoint == Depth || pAttPoint == Stencil || pAttPoint == DepthStencil)
		return mDepthStencilAttachment;

	for (nsuint i = 0; i < mColorAttachments.size(); ++i)
	{
		if (mColorAttachments[i]->mAttPoint == pAttPoint)
			return mColorAttachments[i];
	}

    // Return NULL attachment if nothing was found
	return NULL;
}

const uivec2 & NSFrameBuffer::dim()
{
	return mDim;
}

NSFrameBuffer::Target NSFrameBuffer::target()
{
	return mTarget;
}

bool NSFrameBuffer::has(AttachmentPoint pAttPoint)
{
    // Should return invalid attachment if there is no attachment found
	Attachment * att = get(pAttPoint);
	return (att != NULL);
}

void NSFrameBuffer::initGL()
{
	glGenFramebuffers(1, &mGLName);
	GLError("NSFrameBuffer::initGL");
}

void NSFrameBuffer::release()
{
	while (mColorAttachments.begin() != mColorAttachments.end())
	{
		if (mColorAttachments.back()->mOwningFB == mGLName)
		{
			delete mColorAttachments.back();
			mColorAttachments.pop_back();
		}
	}
	if (mDepthStencilAttachment != NULL && mDepthStencilAttachment->mOwningFB == mGLName)
	{
		delete mDepthStencilAttachment;
		mDepthStencilAttachment = NULL;
	}

	// Attachment destructor deletes the render buffer and texture - which in turn release their
	// open gl resources
	glDeleteFramebuffers(1, &mGLName);
	GLError("NSFrameBuffer::release");
	mGLName = 0;
}

void NSFrameBuffer::resize(nsuint w, nsuint h, nsuint layers)
{
	// Resize the frame buffer by resizing all associated textures
	// This is relatively expensive operation and should only
	// be done when neccessary
	mDim.set(w, h);

	AttachmentArray::iterator iter = mColorAttachments.begin();
	nsuint m = 0;
	while (iter != mColorAttachments.end())
	{
		if ((*iter)->mRenderBuffer != NULL)
			(*iter)->mRenderBuffer->resize(w, h);
		if ((*iter)->mTexture != NULL)
		{
			NSTexture::TexType tt = (*iter)->mTexture->textureType();
			if (tt == NSTexture::Tex1D)
				((NSTex1D*)(*iter)->mTexture)->resize(w);
			else if (tt == NSTexture::Tex2D || tt == NSTexture::Tex1DArray)
				((NSTex2D*)(*iter)->mTexture)->resize(w, h);
			else if (tt == NSTexture::TexCubeMap)
				((NSTexCubeMap*)(*iter)->mTexture)->resize(w, h);
			else if (tt == NSTexture::Tex3D || tt == NSTexture::Tex2DArray)
				((NSTex3D*)(*iter)->mTexture)->resize(w, h, layers);
		}
		++iter;
		++m;
	}

	if (mDepthStencilAttachment != NULL)
	{
		if (mDepthStencilAttachment->mRenderBuffer != NULL)
			mDepthStencilAttachment->mRenderBuffer->resize(w, h);
		if (mDepthStencilAttachment->mTexture != NULL)
		{
			NSTexture::TexType tt = mDepthStencilAttachment->mTexture->textureType();
			if (tt == NSTexture::Tex1D)
				((NSTex1D*)mDepthStencilAttachment->mTexture)->resize(w);
			else if (tt == NSTexture::Tex2D || tt == NSTexture::Tex1DArray)
				((NSTex2D*)mDepthStencilAttachment->mTexture)->resize(w, h);
			else if (tt == NSTexture::TexCubeMap)
				((NSTexCubeMap*)mDepthStencilAttachment->mTexture)->resize(w, h);
			else if (tt == NSTexture::Tex3D || tt == NSTexture::Tex2DArray)
				((NSTex3D*)mDepthStencilAttachment->mTexture)->resize(w, h, layers);
		}
	}
}

bool NSFrameBuffer::setCubeface(AttachmentPoint pAttPoint, NSTexCubeMap::CubeFace pFace)
{
	Attachment * att = get(pAttPoint);
	if (att == NULL || att->mTexture == NULL || att->mTexture->textureType() != NSTexture::TexCubeMap)
		return false;

	glFramebufferTexture2D(mTarget, pAttPoint, pFace, att->mTexture->glid(), 0);
	GLError("NSFrameBuffer::setCubeAttachmentFace");

#ifdef NSDEBUG_RT
	// Check for buffer completeness - should catch any mipmap errors etc..
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		nsstringstream ss;
		ss << "NSFrameBuffer::setCubeAttachmentFace Error in adding attachment - Error Code: " << status;
		dprint(ss.str());
		return false;
	}
#endif
	return true;
}

void NSFrameBuffer::setdim(nsuint w, nsuint h)
{
	resize(w, h);
}

void NSFrameBuffer::setDrawBuffer(AttachmentPoint pAttPoint)
{
	glDrawBuffer(pAttPoint);
	GLError("NSFrameBuffer::setDrawBuffer");
}

void NSFrameBuffer::setDrawBuffers(AttachmentPointArray * pAttArray)
{
	if (pAttArray == NULL)
	{
		dprint("NSFrameBuffer::setDrawBuffers - Error passed NULL attachment point array");
		return;
	}

	if (pAttArray->empty())
	{
		dprint("NSFrameBuffer::setDrawBuffers - Error empty attachment point array");
		return;
	}

	glDrawBuffers(pAttArray->size(), &(*pAttArray)[0]);
	GLError("NSFrameBuffer::setDrawBuffers");
}

void NSFrameBuffer::setReadBuffer(AttachmentPoint pAttPoint)
{
	glReadBuffer(pAttPoint);
	GLError("NSFrameBuffer::setReadBuffer");
}

void NSFrameBuffer::setTarget(Target pTarget)
{
	mTarget = pTarget;
}

void NSFrameBuffer::unbind()
{
	// By unbind I just mean bind the main frame buffer back
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	GLError("NSFrameBuffer::unbind");
}

void NSFrameBuffer::updateDrawBuffers()
{
	AttachmentPointArray bufferAttachments;

    // Go through the color attachments and add their attachment points to the
    // bufferAttachments array - its important to note that the
    // location of each attachment is dependent on the order they were added
	AttachmentArray::iterator iter = mColorAttachments.begin();
	while (iter != mColorAttachments.end())
	{
		bufferAttachments.push_back( GLenum((*iter)->mAttPoint) );
		++iter;
	}

	setDrawBuffers(&bufferAttachments);
}

NSFrameBuffer::NSRenderBuffer::NSRenderBuffer() :
mSampleNumber(0),
mInternalFormat(GL_RGBA8),
mDim(0),
mAllocated(false)
{}

NSFrameBuffer::NSRenderBuffer::~NSRenderBuffer()
{}

void NSFrameBuffer::NSRenderBuffer::allocate()
{
	// Note that the render buffer must be bound for this to work
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, mSampleNumber, mInternalFormat, mDim.w, mDim.h);
	mAllocated = !GLError("NSFrameBuffer::NSRenderBuffer::allocate");
}


void NSFrameBuffer::NSRenderBuffer::bind()
{
	glBindRenderbuffer(GL_RENDERBUFFER, mGLName);
	GLError("NSFrameBuffer::NSRenderBuffer::bind");
}

void NSFrameBuffer::NSRenderBuffer::initGL()
{
	glGenRenderbuffers(1, &mGLName);
	GLError("NSFrameBuffer::NSRenderBuffer::initGL");
}

void NSFrameBuffer::NSRenderBuffer::release()
{
	glDeleteRenderbuffers(1, &mGLName);
	GLError("NSFrameBuffer::NSRenderBuffer::release");
	mAllocated = false;
}

nsint NSFrameBuffer::NSRenderBuffer::internalFormat() const
{
	return mInternalFormat;
}

const uivec2 & NSFrameBuffer::NSRenderBuffer::dim()
{
	return mDim;
}

nsuint NSFrameBuffer::NSRenderBuffer::multisample() const
{
	return mSampleNumber;
}

void NSFrameBuffer::NSRenderBuffer::setMultisample(nsuint numsamples)
{
	mSampleNumber = numsamples;
}

void NSFrameBuffer::NSRenderBuffer::resize(nsint w, nsint h)
{
	// Expensive operation - reallocating memory resources
	// Only do when absolutely necessary
	release();
	mDim.set(w, h);
	initGL();
	bind();
	allocate();
	unbind();
}

void NSFrameBuffer::NSRenderBuffer::setInternalFormat(nsint pInternalFormat)
{
	if (mAllocated)
		return;

	mInternalFormat = pInternalFormat;
}

void NSFrameBuffer::NSRenderBuffer::setdim(nsuint w, nsuint h)
{
	if (mAllocated)
		return;
	mDim.set(w, h);
}

void NSFrameBuffer::NSRenderBuffer::unbind()
{
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	GLError("NSFrameBuffer::unbind");
}

NSFrameBuffer::Attachment::Attachment(): mTexture(NULL),
mRenderBuffer(NULL),
mAttPoint(None),
mOwningFB(0),
mTexUnit(0)
{}

NSFrameBuffer::Attachment::~Attachment()
{
	if (mTexture != NULL)
		mTexture->release();
	if (mRenderBuffer != NULL)
		mRenderBuffer->release();
	delete mTexture;
	delete mRenderBuffer;
}

bool NSFrameBuffer::Attachment::valid()
{
    // there are two main conditions for a valid attachement
    // 1) the attatchment point cannot be None AND
    // 2) the render buffer XOR the texture must not be NULL AND
	return ( mAttPoint != None && ( (mRenderBuffer != NULL) != (mTexture != NULL) ) );
}


NSShadowBuffer::NSShadowBuffer() :
mSpotBuf(),
mPointBuf(),
mDirBuf()
{}

NSShadowBuffer::~NSShadowBuffer()
{}

void NSShadowBuffer::bind(const MapType & pMType)
{
	switch (pMType)
	{
	case (Direction) :
		mDirBuf->bind();
		break;
	case (Spot) :
		mSpotBuf->bind();
		break;
	case (Point) :
		mPointBuf->bind();
		break;
	}
}

void NSShadowBuffer::setPointface(const NSTexCubeMap::CubeFace & pFace)
{
	mSpotBuf->setCubeface(NSFrameBuffer::Depth, pFace);
}

void NSShadowBuffer::disable(const MapType & pMType)
{
	switch (pMType)
	{
	case (Direction) :
		mDirBuf->disableTextures();
		break;
	case (Spot) :
		mSpotBuf->disableTextures();
		break;
	case (Point) :
		mPointBuf->disableTextures();
		break;
	}
}

void NSShadowBuffer::enable(const MapType & pMType)
{
	switch (pMType)
	{
	case (Direction) :
		mDirBuf->enableTextures();
		break;
	case (Spot) :
		mSpotBuf->enableTextures();
		break;
	case (Point) :
		mPointBuf->enableTextures();
		break;
	}
}

void NSShadowBuffer::setfb(NSFrameBuffer * fb, const MapType & map)
{
	switch (map)
	{
	case (Direction) :
		mDirBuf = fb;
		break;
	case (Spot) :
		mSpotBuf = fb;
		break;
	case (Point) :
		mPointBuf = fb;
		break;
	}
}

const uivec2 & NSShadowBuffer::dim(const MapType & pMap)
{
	switch (pMap)
	{
	case (Direction) :
		return mDirBuf->dim();
	case (Spot) :
		return mSpotBuf->dim();
	case (Point) :
		return mPointBuf->dim();
	default:
		throw(std::exception("nonrecognized map type"));
	}
}

NSFrameBuffer * NSShadowBuffer::fb(const MapType & pMap)
{
	switch (pMap)
	{
	case (Direction) :
		return mDirBuf;
	case (Spot) :
		return mSpotBuf;
	case (Point) :
		return mPointBuf;
	default:
		return NULL;
	}
}

void NSShadowBuffer::init()
{
	if (mSpotBuf == NULL || mDirBuf == NULL || mPointBuf == NULL)
		return;

	mSpotBuf->setTarget(NSFrameBuffer::Draw);
	mSpotBuf->bind();
	mSpotBuf->setDrawBuffer(NSFrameBuffer::None);
	NSFrameBuffer::Attachment * att = mSpotBuf->create<NSTex2D>("SpotLightShadow", NSFrameBuffer::Depth, SHADOW_TEX_UNIT, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_FLOAT);
	att->mTexture->setParameteri(NSTexture::MinFilter, GL_LINEAR);
	att->mTexture->setParameteri(NSTexture::MagFilter, GL_LINEAR);
	att->mTexture->setParameteri(NSTexture::CompareMode, GL_COMPARE_REF_TO_TEXTURE);
	att->mTexture->setParameteri(NSTexture::CompareFunc, GL_LESS);
	att->mTexture->setParameteri(NSTexture::WrapS, GL_CLAMP_TO_EDGE);
	att->mTexture->setParameteri(NSTexture::WrapT, GL_CLAMP_TO_EDGE);

	mDirBuf->setTarget(NSFrameBuffer::Draw);
	mDirBuf->bind();
	mDirBuf->setDrawBuffer(NSFrameBuffer::None);
	NSFrameBuffer::Attachment * att2 = mDirBuf->create<NSTex2D>("DirLightShadow", NSFrameBuffer::Depth, SHADOW_TEX_UNIT, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_FLOAT);
	att2->mTexture->setParameteri(NSTexture::MinFilter, GL_LINEAR);
	att2->mTexture->setParameteri(NSTexture::MagFilter, GL_LINEAR);
	att2->mTexture->setParameteri(NSTexture::CompareMode, GL_COMPARE_REF_TO_TEXTURE);
	att2->mTexture->setParameteri(NSTexture::CompareFunc, GL_LEQUAL);
	att2->mTexture->setParameteri(NSTexture::WrapS, GL_CLAMP_TO_EDGE);
	att2->mTexture->setParameteri(NSTexture::WrapT, GL_CLAMP_TO_EDGE);

	mPointBuf->setTarget(NSFrameBuffer::Draw);
	mPointBuf->bind();
	mPointBuf->setDrawBuffer(NSFrameBuffer::None);
	NSFrameBuffer::Attachment * att3 = mPointBuf->create<NSTexCubeMap>("PointLightShadow", NSFrameBuffer::Depth, SHADOW_TEX_UNIT, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_FLOAT);
	att3->mTexture->setParameteri(NSTexture::MinFilter, GL_LINEAR);
	att3->mTexture->setParameteri(NSTexture::MagFilter, GL_LINEAR);
	att3->mTexture->setParameteri(NSTexture::CompareMode, GL_COMPARE_REF_TO_TEXTURE);
	att3->mTexture->setParameteri(NSTexture::CompareFunc, GL_LEQUAL);
	att3->mTexture->setParameteri(NSTexture::WrapS, GL_CLAMP_TO_EDGE);
	att3->mTexture->setParameteri(NSTexture::WrapT, GL_CLAMP_TO_EDGE);
	att3->mTexture->setParameteri(NSTexture::WrapR, GL_CLAMP_TO_EDGE);
}

void NSShadowBuffer::resize(nsuint w, nsuint h)
{
	mSpotBuf->resize(w,h);
	mPointBuf->resize(w, h);
	mDirBuf->resize(w, h);
}

void NSShadowBuffer::resize(const MapType & pMap, nsuint w, nsuint h)
{
	fb(pMap)->resize(w, h);
}

void NSShadowBuffer::setdim(nsuint w, nsuint h)
{
	mSpotBuf->setdim(w, h);
	mPointBuf->setdim(w, h);
	mDirBuf->setdim(w, h);
}

void NSShadowBuffer::setdim(const MapType & pMap, nsuint w, nsuint h)
{
	fb(pMap)->setdim(w, h);
}

NSGBuffer::NSGBuffer() :
mTexFrameBuffer(NULL),
mMultisampleLevel(0),
mScreendim(0)
{}

NSGBuffer::~NSGBuffer()
{}

void NSGBuffer::bind()
{
	mTexFrameBuffer->setTarget(NSFrameBuffer::Draw);
	mTexFrameBuffer->bind();
}

void NSGBuffer::debugBlit(const uivec2 & scrn)
{
	uivec2 hlf = scrn / 2;

	mTexFrameBuffer->setTarget(NSFrameBuffer::Read);
	mTexFrameBuffer->bind();


	glDisable(GL_DEPTH_TEST);
	glDisable(GL_STENCIL_TEST);
	glDisable(GL_BLEND);
	

	mTexFrameBuffer->setReadBuffer(NSFrameBuffer::AttachmentPoint(NSFrameBuffer::Color + Diffuse));
	glBlitFramebuffer(0, 0, scrn.w, scrn.h, 0, 0, hlf.w, hlf.h, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	GLError("NSFrameBuffer::debugBlit");

	mTexFrameBuffer->setReadBuffer(NSFrameBuffer::AttachmentPoint(NSFrameBuffer::Color + Position));
	glBlitFramebuffer(0, 0, scrn.w, scrn.h, 0, hlf.h, hlf.w, scrn.h, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	GLError("NSFrameBuffer::debugBlit2");

	mTexFrameBuffer->setReadBuffer(NSFrameBuffer::AttachmentPoint(NSFrameBuffer::Color + Normal));
	glBlitFramebuffer(0, 0, scrn.w, scrn.h, hlf.w, hlf.h, scrn.w, scrn.h, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	GLError("NSFrameBuffer::debugBlit3");

	mTexFrameBuffer->setReadBuffer(NSFrameBuffer::AttachmentPoint(NSFrameBuffer::Color + Material));
	glBlitFramebuffer(0, 0, scrn.w, scrn.h, hlf.w, 0, scrn.w, hlf.h, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	GLError("NSFrameBuffer::debugBlit4");

	mTexFrameBuffer->setReadBuffer(NSFrameBuffer::None);
	mTexFrameBuffer->unbind();
	mTexFrameBuffer->setTarget(NSFrameBuffer::ReadAndDraw);
}

void NSGBuffer::disableTextures()
{
	mTexFrameBuffer->disableTextures();
}

void NSGBuffer::enableColorWrite(bool pEnable)
{
	if (pEnable)
		mTexFrameBuffer->updateDrawBuffers();
	else
		mTexFrameBuffer->setDrawBuffer(NSFrameBuffer::None);
}

void NSGBuffer::enableTextures()
{
	mTexFrameBuffer->enableTextures();
}

NSFrameBuffer::Attachment * NSGBuffer::color(ColorAttachmentType pType)
{
    // If this check fails it probably means the GBuffer was not initialized
	if (pType >= AttCount)
	{
		dprint("NSGFrameBuffer::getColorAttachment Parameter pType is larger than the color attachments vector");
		return NULL;
	}
	return mTexFrameBuffer->get(NSFrameBuffer::AttachmentPoint(NSFrameBuffer::Color + pType));
}

NSFrameBuffer::Attachment * NSGBuffer::depth()
{
	return mTexFrameBuffer->get(NSFrameBuffer::DepthStencil);
}

NSFrameBuffer * NSGBuffer::fb()
{
	return mTexFrameBuffer;
}

void NSGBuffer::resizefb(nsuint w, nsuint h)
{
	mTexFrameBuffer->resize(w, h);
}

void NSGBuffer::setfb(NSFrameBuffer * fb)
{
	mTexFrameBuffer = fb;
}

void NSGBuffer::resizescreen(nsuint w, nsuint h)
{
	mScreendim.set(w, h);
}

void NSGBuffer::setscreendim(nsuint w, nsuint h)
{
	mScreendim.set(w, h);
}

void NSGBuffer::setscreendim(const uivec2 & dim)
{
	mScreendim = dim;
}

void NSGBuffer::setfbdim(nsuint w, nsuint h)
{
	mTexFrameBuffer->setdim(w, h);
}

void NSGBuffer::setfbdim(const uivec2 & dim)
{
	mTexFrameBuffer->setdim(dim.x,dim.y);
}

void NSGBuffer::init()
{
	if (mTexFrameBuffer == NULL)
		return;

	if (mTexFrameBuffer->dim().x == 0 || mTexFrameBuffer->dim().y == 0)
		return;

	// Bind the resource - only in this subclass will we bind in an init function
	if (!_createTextureAttachments())
		dprint("NSGFrameBuffer::init - Error in adding a color attachment - see previous error message - not updating draw buffers");
}

void NSGBuffer::unbind()
{
	mTexFrameBuffer->unbind();
}

bool NSGBuffer::_createTextureAttachments()
{
	// Attach the different textures for the Gbuffer - doing it explicitly using the enum because
	// its a lot less confusing later than if you use a for loop to attach the colors.. to add more
	// texture attachments to the gbuffer simply add an enum value and two lines to this function
	bool flag = true;
	NSFrameBuffer::Attachment * att = NULL;

	mTexFrameBuffer->bind();
	// Depth attachment
	att = mTexFrameBuffer->create(NSFrameBuffer::DepthStencil, 0, GL_DEPTH32F_STENCIL8);

	// The order these are added here determines the layout in the shader
	// Diffuse color data texture: layout = 0
	att = mTexFrameBuffer->create<NSTex2D>("GBufferDiffuse", NSFrameBuffer::AttachmentPoint(NSFrameBuffer::Color + Diffuse), G_DIFFUSE_TEX_UNIT, GL_RGBA8, GL_RGBA, GL_FLOAT);
	if (att != NULL)
	{
		att->mTexture->setParameterf(NSTexture::MinFilter, GL_LINEAR);
		att->mTexture->setParameterf(NSTexture::MagFilter, GL_LINEAR);
	}

	// Picking data texture: layout = 1
	att = mTexFrameBuffer->create<NSTex2D>("GBufferPicking", NSFrameBuffer::AttachmentPoint(NSFrameBuffer::Color + Picking), G_PICKING_TEX_UNIT, GL_RGB32UI, GL_RGB_INTEGER, GL_UNSIGNED_INT);
	if (att != NULL)
	{
		att->mTexture->setParameteri(NSTexture::MinFilter, GL_NEAREST);
		att->mTexture->setParameteri(NSTexture::MagFilter, GL_NEAREST);
	}

	// Position data texture: layout = 2
	att = mTexFrameBuffer->create<NSTex2D>("GBufferPosition", NSFrameBuffer::AttachmentPoint(NSFrameBuffer::Color + Position), G_WORLD_POS_TEX_UNIT, GL_RGB32F, GL_RGB, GL_FLOAT);
	if (att != NULL)
	{
		att->mTexture->setParameterf(NSTexture::MinFilter, GL_NEAREST);
		att->mTexture->setParameterf(NSTexture::MagFilter, GL_NEAREST);
	}

	// Normal data texture: layout = 3
	att = mTexFrameBuffer->create<NSTex2D>("GBufferNormal", NSFrameBuffer::AttachmentPoint(NSFrameBuffer::Color + Normal), G_NORMAL_TEX_UNIT, GL_RGBA32F, GL_RGBA, GL_FLOAT);
	if (att != NULL)
	{
		att->mTexture->setParameterf(NSTexture::MinFilter, GL_NEAREST);
		att->mTexture->setParameterf(NSTexture::MagFilter, GL_NEAREST);
	}

	// Normal data texture: layout = 4
	att = mTexFrameBuffer->create<NSTex2D>("GBufferMaterial", NSFrameBuffer::AttachmentPoint(NSFrameBuffer::Color + Material), G_MATERIAL_TEX_UNIT, GL_RGBA32F, GL_RGBA, GL_FLOAT);
	if (att != NULL)
	{
		att->mTexture->setParameterf(NSTexture::MinFilter, GL_NEAREST);
		att->mTexture->setParameterf(NSTexture::MagFilter, GL_NEAREST);
	}
	mTexFrameBuffer->updateDrawBuffers();
	return (att != NULL);
}