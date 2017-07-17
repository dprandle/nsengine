/*!
\file system.cpp

\brief Definition file for nssystem class

This file contains all of the neccessary definitions for the nssystem class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#include <system/nssystem.h>
#include <nsworld_data.h>

nssystem::nssystem(uint32 hashed_type):
	m_chunk_null_error(false),
	m_active_chunk(nullptr),
	m_hashed_type(hashed_type)
{}

nssystem::~nssystem()
{}

nstform_ent_chunk * nssystem::active_chunk()
{
	return m_active_chunk;
}

bool nssystem::chunk_error_check()
{
	if (m_active_chunk == nullptr)
	{
		if (!m_chunk_null_error)
		{
			dprint(hash_to_guid(m_hashed_type) + "Active chunk is null in system " + hash_to_guid(m_hashed_type));
			m_chunk_null_error = true;
		}
	}
	else
	{
		m_chunk_null_error = false;
	}
	return m_chunk_null_error;
}

void nssystem::set_active_chunk(nstform_ent_chunk * active_chunk)
{
	m_active_chunk = active_chunk;
}

uint32 nssystem::type()
{
	return m_hashed_type;
}
