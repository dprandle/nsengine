/*! 
	\file nsbufferobject.cpp
	
	\brief Definition file for NSBufferObject class

	This file contains all of the neccessary definitions for the NSBufferObject class.

	\author Daniel Randle
	\date November 2 2013
	\copywrite Earth Banana Games 2013
*/


#include <nsbuffer_object.h>
#include <nsengine.h>

NSBufferObject::NSBufferObject(TargetBuffer pTarget, StorageMode pStorageMode): 
	mBufferAllocated(false),
	mMapped(false),
	mStorageMode(pStorageMode),
	mTarget(pTarget),
	nsgl_object()
{}

NSBufferObject::~NSBufferObject()
{}

	
void NSBufferObject::bind()
{
	glBindBuffer(mTarget, m_gl_name);
	GLError("NSBufferObject::bind()");
}

void NSBufferObject::bind(uint32 pIndex)
{
	glBindBufferBase(mTarget, pIndex, m_gl_name);
	GLError("NSBufferObject::bindBase()");
}

NSBufferObject::StorageMode NSBufferObject::storage() const
{
	return mStorageMode;
}

NSBufferObject::TargetBuffer NSBufferObject::target() const
{
	return mTarget;
}

void NSBufferObject::init_gl()
{
	glGenBuffers(1, &m_gl_name);
	GLError("NSBufferObject::initGL()");
}

bool NSBufferObject::allocated() const
{
	return mBufferAllocated;
}

bool NSBufferObject::mapped() const
{
	return mMapped;
}

void NSBufferObject::allocate(UsageFlag pFlag, uint32 pTotalByteSize)
{
	if (mStorageMode == Mutable)
		glBufferData(mTarget, pTotalByteSize, NULL, pFlag);
	else
		glBufferStorage(mTarget, pTotalByteSize, NULL, pFlag);

	GLError("NSBufferObject::allocate()");
	mBufferAllocated = true;
}

void NSBufferObject::release()
{
	if (m_gl_name != 0)
		glDeleteBuffers(1, &m_gl_name);
	m_gl_name = 0;
	mMapped = false;
	mBufferAllocated = false;
	GLError("NSBufferObject::release()");
}

bool NSBufferObject::setStorage(StorageMode pStorageMode)
{
	if (mBufferAllocated)
	{
		dprint("NSBufferObject::setStorageMode() : Trying to change allocated buffer's storage mode");
		return false;
	}
	mStorageMode = pStorageMode;
	return true;
}

bool NSBufferObject::setTarget(TargetBuffer pTarget)
{
	mTarget = pTarget;
	return true;
}

void NSBufferObject::unbind()
{
	glBindBuffer(mTarget, 0);
	GLError("NSBufferObject::unbind()");
}

void NSBufferObject::unbind(uint32 pIndex)
{
	glBindBufferBase(mTarget, pIndex, 0);
	GLError("NSBufferObject::unbindBase()");
}

bool NSBufferObject::unmap()
{
	if (!mMapped || !mBufferAllocated)
	{
		dprint("NSBufferObject::unmap() : Trying to unmap buffer that has either not been allocated or mMapped");
		return false;
	}
	mMapped = !(glUnmapBuffer(mTarget) != 0);
	GLError("NSBufferObject::unmap()");
	return !mMapped;
}
