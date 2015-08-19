/*! 
	\file nsbuffer_object.h
	
	\brief Header file for nsbuffer_object class

	This file contains all of the neccessary declarations for the nsbuffer_object class.

	\author Daniel Randle
	\date November 2 2013
	\copywrite Earth Banana Games 2013
*/

#ifndef NSBUFFEROBJECT_H
#define NSBUFFEROBJECT_H

#include <nsglobal.h>
#include <myGL/glew.h>
#include <vector>
#include <nsgl_object.h>

class nsbuffer_object : public nsgl_object
{
public:
	enum target_buffer 
	{
		array = GL_ARRAY_BUFFER,
		atomic_counter = GL_ATOMIC_COUNTER_BUFFER,
		copy_read = GL_COPY_READ_BUFFER,
		copy_write = GL_COPY_WRITE_BUFFER,
		draw_indirect = GL_DRAW_INDIRECT_BUFFER,
		dispatch_indirect = GL_DISPATCH_INDIRECT_BUFFER,
		element_array = GL_ELEMENT_ARRAY_BUFFER,
		pixel_pack = GL_PIXEL_PACK_BUFFER,
		pixel_unpack = GL_PIXEL_UNPACK_BUFFER,
		query = GL_QUERY_BUFFER,
		shader_storage = GL_SHADER_STORAGE_BUFFER,
		texture_target = GL_TEXTURE_BUFFER,
		transform_feedback = GL_TRANSFORM_FEEDBACK_BUFFER,
		uniform = GL_UNIFORM_BUFFER 
	};

	enum storage_mode 
	{
		storage_mutable,
		storage_immutable
	};

	enum access_map
	{
		read_only = GL_READ_ONLY,
		write_only = GL_WRITE_ONLY,
		read_write = GL_READ_WRITE
	};

	enum usage_flag
	{
		mutable_stream_read = GL_STREAM_READ,
		mutable_stream_draw = GL_STREAM_DRAW,
		mutable_stream_copy = GL_STREAM_COPY,
		mutable_static_read = GL_STATIC_READ,
		mutable_static_draw = GL_STATIC_DRAW,
		mutable_static_copy = GL_STATIC_COPY,
		mutable_dynamic_read = GL_DYNAMIC_READ,
		mutable_dynamic_draw = GL_DYNAMIC_DRAW,
		mutable_dynamic_copy = GL_DYNAMIC_COPY,
		immutable_map_read = GL_MAP_READ_BIT,
		immutable_map_write = GL_MAP_WRITE_BIT,
		immutable_map_persistant = GL_MAP_PERSISTENT_BIT,
		immutable_map_coherent = GL_MAP_COHERENT_BIT,
		immutable_client_storage = GL_CLIENT_STORAGE_BIT,
		immutable_dynamic_storage = GL_DYNAMIC_STORAGE_BIT
	};

	enum access_map_range
	{
		map_read = GL_MAP_READ_BIT,
		map_write = GL_MAP_WRITE_BIT,
		map_invalidate_range = GL_MAP_INVALIDATE_RANGE_BIT,
		map_invalidate_buffer = GL_MAP_INVALIDATE_BUFFER_BIT,
		map_flush_explicit = GL_MAP_FLUSH_EXPLICIT_BIT,
		map_unsynchronized = GL_MAP_UNSYNCHRONIZED_BIT,
		map_persistent = GL_MAP_PERSISTENT_BIT,
		map_coherent = GL_MAP_COHERENT_BIT
	};

	nsbuffer_object(target_buffer target_, storage_mode storage_mode_);
	~nsbuffer_object();

	template<class data_type>
	void allocate(const std::vector<data_type>& data_, usage_flag flag_, uint32 size_)
	{
		if (data_.empty())
			return;

		if (m_storage_mode == storage_mutable)
			glBufferData(m_target, sizeof(data_type)* size_, &data_[0], flag_);
		else
			glBufferStorage(m_target, sizeof(data_type)* size_, &data_[0], flag_);

		m_allocated = !GLError("nsbuffer_object::allocate");
	}

	template<class data_type>
	void allocate(data_type& data_, usage_flag flag_)
	{
		if (m_storage_mode == storage_mutable)
			glBufferData(m_target, sizeof(data_type), &data_, flag_);
		else
			glBufferStorage(m_target, sizeof(data_type), &data_, flag_);

		m_allocated = !GLError("nsbuffer_object::allocate");
	}

	void allocate(usage_flag flag_, uint32 total_byte_size_);

	template<class data_type>
	void allocate(usage_flag flag_, uint32 size_)
	{
		if (m_storage_mode == storage_mutable)
			glBufferData(m_target, sizeof(data_type)* size_, NULL, flag_);
		else
			glBufferStorage(m_target, sizeof(data_type)* size_, NULL, flag_);

		m_allocated = !GLError("nsbuffer_object::allocate");
	}
	
	void bind();

	/*!
	This calls bind buffer base
	*/
	void bind(uint32 index_);

	template<class data_type>
	void data(std::vector<data_type> & data_, int32 offset_bytes_ = 0)
	{
		if (!m_allocated)
			return;

		glGetBufferSubData(m_target, offset_bytes_*sizeof(data_type), data_.size()*sizeof(data_type), &data_[0]);
		GLError("nsbuffer_object::allocate");
	}

	storage_mode storage() const;

	target_buffer target() const;

	void init_gl();

	bool allocated() const;

	bool mapped() const;

	void release();

	template<class data_type>
	data_type * map(access_map access_)
	{
		if (!m_allocated)
			return NULL;

		m_mapped = true;
		data_type * retVal = (data_type*)glMapBuffer(m_target, access_);
		if (retVal == NULL)
		{
			GLError("nsbuffer_object::map");
			m_mapped = false;
		}
		return retVal;
	}

	template<class data_type>
	data_type * map(uint32 offset_, uint32 length_, access_map_range access_)
	{
		if (!m_allocated)
			return NULL;

		data_type * retVal = (data_type*)glMapBufferRange(m_target,sizeof(data_type)*offset_, sizeof(data_type)*length_, access_);
		m_mapped = true;
		if (retVal == NULL)
		{
			GLError("nsbuffer_object::map");
			m_mapped = false;
		}
		return retVal;
	}

	template<class data_type>
	void set_data(const std::vector<data_type> & data_, uint32 offset_, uint32 size_)
	{
		if (!m_allocated)
			return;

		glBufferSubData(m_target, offset_, sizeof(data_type)* size_, &data_[0]);
		GLError("nsbuffer_object::setData");
	}

	bool set_storage(storage_mode storage_mode_);

	bool set_target(target_buffer target_);

	void unbind();

	/*!
	This calls bind buffer base
	*/
	void unbind(uint32 index_);

	bool unmap();

private:
	bool m_allocated;
	bool m_mapped;
	storage_mode m_storage_mode;
	target_buffer m_target;
};

#endif
