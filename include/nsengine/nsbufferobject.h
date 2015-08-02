/*! 
	\file nsbufferobject.h
	
	\brief Header file for NSBufferObject class

	This file contains all of the neccessary declarations for the NSBufferObject class.

	\author Daniel Randle
	\date November 2 2013
	\copywrite Earth Banana Games 2013
*/

#ifndef NSBUFFEROBJECT_H
#define NSBUFFEROBJECT_H

#include <nsglobal.h>
#include <myGL/glew.h>
#include <vector>
#include <nsglobject.h>

class NSBufferObject : public NSGLObject
{
public:
	enum TargetBuffer 
	{
		Array = GL_ARRAY_BUFFER,
		AtomicCounter = GL_ATOMIC_COUNTER_BUFFER,
		CopyRead = GL_COPY_READ_BUFFER,
		CopyWrite = GL_COPY_WRITE_BUFFER,
		DrawIndirect = GL_DRAW_INDIRECT_BUFFER,
		DispatchIndirect = GL_DISPATCH_INDIRECT_BUFFER,
		ElementArray = GL_ELEMENT_ARRAY_BUFFER,
		PixelPack = GL_PIXEL_PACK_BUFFER,
		PixelUnpack = GL_PIXEL_UNPACK_BUFFER,
		Query = GL_QUERY_BUFFER,
		ShaderStorage = GL_SHADER_STORAGE_BUFFER,
		Texture = GL_TEXTURE_BUFFER,
		TransformFeedback = GL_TRANSFORM_FEEDBACK_BUFFER,
		Uniform = GL_UNIFORM_BUFFER 
	};

	enum StorageMode 
	{
		Mutable,
		Immutable
	};

	enum AccessMap
	{
		ReadOnly = GL_READ_ONLY,
		WriteOnly = GL_WRITE_ONLY,
		ReadWrite = GL_READ_WRITE
	};

	enum UsageFlag
	{
		MutableStreamRead = GL_STREAM_READ,
		MutableStreamDraw = GL_STREAM_DRAW,
		MutableStreamCopy = GL_STREAM_COPY,
		MutableStaticRead = GL_STATIC_READ,
		MutableStaticDraw = GL_STATIC_DRAW,
		MutableStaticCopy = GL_STATIC_COPY,
		MutableDynamicRead = GL_DYNAMIC_READ,
		MutableDynamicDraw = GL_DYNAMIC_DRAW,
		MutableDynamicCopy = GL_DYNAMIC_COPY,
		ImmutableMapRead = GL_MAP_READ_BIT,
		ImmutableMapWrite = GL_MAP_WRITE_BIT,
		ImmutableMapPersisant = GL_MAP_PERSISTENT_BIT,
		ImmutableMapCoherent = GL_MAP_COHERENT_BIT,
		ImmutableClientStorage = GL_CLIENT_STORAGE_BIT,
		ImmutableDynamicStorage = GL_DYNAMIC_STORAGE_BIT
	};

	enum AccessMapRange
	{
		MapRead = GL_MAP_READ_BIT,
		MapWrite = GL_MAP_WRITE_BIT,
		MapInvalidateRange = GL_MAP_INVALIDATE_RANGE_BIT,
		MapInvalidateBuffer = GL_MAP_INVALIDATE_BUFFER_BIT,
		MapFlushExplicit = GL_MAP_FLUSH_EXPLICIT_BIT,
		MapUnsynchronized = GL_MAP_UNSYNCHRONIZED_BIT,
		MapPersisant = GL_MAP_PERSISTENT_BIT,
		MapCoherent = GL_MAP_COHERENT_BIT
	};

	NSBufferObject(TargetBuffer pTarget, StorageMode pStorageMode);
	~NSBufferObject();

	template<class DataType>
	void allocate(const std::vector<DataType>& data, UsageFlag pFlag, nsuint pSize)
	{
		if (data.empty())
			return;

		if (mStorageMode == Mutable)
			glBufferData(mTarget, sizeof(DataType)* pSize, &data[0], pFlag);
		else
			glBufferStorage(mTarget, sizeof(DataType)* pSize, &data[0], pFlag);

		mBufferAllocated = !GLError("NSBufferObject::allocate");
	}

	template<class DataType>
	void allocate(DataType& data, UsageFlag pFlag)
	{
		if (mStorageMode == Mutable)
			glBufferData(mTarget, sizeof(DataType), &data, pFlag);
		else
			glBufferStorage(mTarget, sizeof(DataType), &data, pFlag);

		mBufferAllocated = !GLError("NSBufferObject::allocate");
	}

	void allocate(UsageFlag pFlag, nsuint pTotalByteSize);

	template<class DataType>
	void allocate(UsageFlag pFlag, nsuint pSize)
	{
		if (mStorageMode == Mutable)
			glBufferData(mTarget, sizeof(DataType)* pSize, NULL, pFlag);
		else
			glBufferStorage(mTarget, sizeof(DataType)* pSize, NULL, pFlag);

		mBufferAllocated = !GLError("NSBufferObject::allocate");
	}
	
	void bind();

	/*!
	This calls bind buffer base
	*/
	void bind(nsuint pIndex);

	template<class DataType>
	void data(std::vector<DataType> & pData, nsint pOffsetBytes = 0)
	{
		if (!mBufferAllocated)
			return;

		glGetBufferSubData(mTarget, pOffsetBytes*sizeof(DataType), pData.size()*sizeof(DataType), &pData[0]);
		GLError("NSBufferObject::allocate");
	}

	StorageMode storage() const;

	TargetBuffer target() const;

	void initGL();

	bool allocated() const;

	bool mapped() const;

	void release();

	template<class DataType>
	DataType * map(AccessMap pAccess)
	{
		if (!mBufferAllocated)
			return NULL;

		mMapped = true;
		DataType * retVal = (DataType*)glMapBuffer(mTarget, pAccess);
		if (retVal == NULL)
		{
			GLError("NSBufferObject::map");
			mMapped = false;
		}
		return retVal;
	}

	template<class DataType>
	DataType * map(nsuint pOffset, nsuint pLength, AccessMapRange pAccess)
	{
		if (!mBufferAllocated)
			return NULL;

		DataType * retVal = (DataType*)glMapBufferRange(mTarget,sizeof(DataType)*pOffset, sizeof(DataType)*pLength, pAccess);
		mMapped = true;
		if (retVal == NULL)
		{
			GLError("NSBufferObject::map");
			mMapped = false;
		}
		return retVal;
	}

	template<class DataType>
	void setData(const std::vector<DataType> & pData, nsuint pOffset, nsuint pSize)
	{
		if (!mBufferAllocated)
			return;

		glBufferSubData(mTarget, pOffset, sizeof(DataType)* pSize, &pData[0]);
		GLError("NSBufferObject::setData");
	}

	bool setStorage(StorageMode pStorageMode);

	bool setTarget(TargetBuffer pTarget);

	void unbind();

	/*!
	This calls bind buffer base
	*/
	void unbind(nsuint pIndex);

	bool unmap();

private:
	bool mBufferAllocated;
	bool mMapped;
	StorageMode mStorageMode;
	TargetBuffer mTarget;
};

#endif
