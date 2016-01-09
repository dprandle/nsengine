/*! 
	\file nsbufferobject.cpp
	
	\brief Definition file for nsbuffer_object class

	This file contains all of the neccessary definitions for the nsbuffer_object class.

	\author Daniel Randle
	\date November 2 2013
	\copywrite Earth Banana Games 2013
*/


#include <nsbuffer_object.h>
#include <nsgl_context.h>
#include <nsengine.h>

nsbuffer_object::nsbuffer_object(target_buffer pTarget, storage_mode pStorageMode): 
	m_allocated(false),
	m_mapped(false),
	m_storage_mode(pStorageMode),
	m_target(pTarget),
	nsgl_object()
{}

nsbuffer_object::~nsbuffer_object()
{}

	
void nsbuffer_object::bind() const
{
	glBindBuffer(m_target, m_gl_name);
	gl_err_check("nsbuffer_object::bind()");
}

void nsbuffer_object::bind(uint32 pIndex)
{
	glBindBufferBase(m_target, pIndex, m_gl_name);
	gl_err_check("nsbuffer_object::bind(uint32 index)");
}

nsbuffer_object::storage_mode nsbuffer_object::storage() const
{
	return m_storage_mode;
}

nsbuffer_object::target_buffer nsbuffer_object::target() const
{
	return m_target;
}

void nsbuffer_object::init_gl()
{
	glGenBuffers(1, &m_gl_name);
	gl_err_check("nsbuffer_object::initGL()");
}

bool nsbuffer_object::allocated() const
{
	return m_allocated;
}

bool nsbuffer_object::mapped() const
{
	return m_mapped;
}

void nsbuffer_object::allocate(usage_flag pFlag, uint32 pTotalByteSize)
{
	if (m_storage_mode == storage_mutable)
		glBufferData(m_target, pTotalByteSize, NULL, pFlag);
	else
		glBufferStorage(m_target, pTotalByteSize, NULL, pFlag);

	gl_err_check("nsbuffer_object::allocate()");
	m_allocated = true;
}

void nsbuffer_object::release()
{
	if (m_gl_name != 0)
		glDeleteBuffers(1, &m_gl_name);
	m_gl_name = 0;
	m_mapped = false;
	m_allocated = false;
	gl_err_check("nsbuffer_object::release()");
}

bool nsbuffer_object::set_storage(storage_mode pStorageMode)
{
	if (m_allocated)
	{
		dprint("nsbuffer_object::setStorageMode() : Trying to change allocated buffer's storage mode");
		return false;
	}
	m_storage_mode = pStorageMode;
	return true;
}

bool nsbuffer_object::set_target(target_buffer pTarget)
{
	m_target = pTarget;
	return true;
}

void nsbuffer_object::unbind() const
{
	glBindBuffer(m_target, 0);
	gl_err_check("nsbuffer_object::unbind()");
}

void nsbuffer_object::unbind(uint32 pIndex)
{
	glBindBufferBase(m_target, pIndex, 0);
	gl_err_check("nsbuffer_object::unbindBase()");
}

bool nsbuffer_object::unmap()
{
	if (!m_mapped || !m_allocated)
	{
		dprint("nsbuffer_object::unmap() : Trying to unmap buffer that has either not been allocated or mMapped");
		return false;
	}
	m_mapped = !(glUnmapBuffer(m_target) != 0);
	gl_err_check("nsbuffer_object::unmap()");
	return !m_mapped;
}
