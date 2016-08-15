/*! 
	\file nsgl_buffer.h
	
	\brief Header file for nsgl_buffer class

	This file contains all of the neccessary declarations for the nsgl_buffer class.

	\author Daniel Randle
	\date November 2 2013
	\copywrite Earth Banana Games 2013
*/

#ifndef NSGL_BUFFER_H
#define NSGL_BUFFER_H

#include <opengl/nsgl_object.h>
#include <opengl/glew.h>

struct nsgl_buffer : public nsgl_obj
{
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

	enum mutable_usage_flag
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
	};

	enum immutable_usage_flag
	{
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

	nsgl_buffer();

	~nsgl_buffer();

	void bind();

	void unbind();

	void init();
	
	void release();
	
	template<class T_>
	void allocate(const std::vector<T_> & data_vec, mutable_usage_flag usage)
	{
		return allocate(data_vec.size(), sizeof(T_), (void*)data_vec.data(), usage);
	}
	
	template<class T_>
	void allocate(uint32 data_size, T_ * data, mutable_usage_flag usage)
	{
		return allocate(data_size, sizeof(T_), (void*)data, usage);
	}

	void allocate(uint32 data_size, uint32 data_type_byte_size, void * data, mutable_usage_flag usage);

	template<class T_>
	void allocate(const std::vector<T_> & data_vec, immutable_usage_flag usage)
	{
		return allocate(data_vec.size(), sizeof(T_), (void*)data_vec.data(), usage);
	}

	template<class T_>
	void allocate(uint32 data_size, T_ * data, immutable_usage_flag usage)
	{
		return allocate(data_size, sizeof(T_), (void*)data, usage);
	}

	void allocate(uint32 data_size, uint32 data_type_byte_size, void * data, immutable_usage_flag usage);

	template<class T_>
	void download(uint32 offset, const std::vector<T_> & data_vec)
	{
		return download(offset, data_vec.size(), sizeof(T_), (void*)data_vec.data());
	}

	template<class T_>
	void download(uint32 offset, uint32 data_size, T_ * data)
	{
		return download(offset, data_size, sizeof(T_), (void*)data);
	}

	void download(uint32 offset_size, uint32 data_size, uint32 data_type_byte_size, void * data);
	
	void clear(int32 internal_format, int32 format, int32 type, const void * data);
	
	template<class T_>
	void upload(uint32 offset, const std::vector<T_> & data_vec)
	{
		return upload(offset, data_vec.size(), sizeof(T_), (void*)data_vec.data());
	}

	template<class T_>
	void upload(uint32 offset, uint32 data_size, T_ * data)
	{
		return upload(offset, data_size, sizeof(T_), (void*)data);
	}

	void upload(uint32 offset, uint32 data_size, uint32 data_type_byte_size, void * data);

	template<class T_>
	T_ * map(access_map access)
	{
		return static_cast<T_*>(map(access));
	}

	void * map(access_map access);

	template<class T_>
	T_ * map_range(uint32 offset, uint32 length, access_map_range access)
	{
		return static_cast<T_*>(map_range(offset * sizeof(T_), length * sizeof(T_), access));
	}
	
	void * map_range(uint32 byte_offset, uint32 byte_length, access_map_range access);

	void unmap();

	int32 target;
	uint32 target_index;
	uint32 indexed_target_offset;
	uint32 indexed_target_size;
};

#endif
