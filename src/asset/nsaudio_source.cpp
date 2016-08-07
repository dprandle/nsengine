/*! 
	\file nsaudio_source.cpp
	
	\brief Definition file for nsaudio_source class

	This file contains all of the neccessary definitions for the nsaudio_source class.

	\author Daniel Randle
	\date November 23 2013
	\copywrite Earth Banana Games 2013
*/

#include <AL/al.h>
#include <sndfile.h>

#include <nsaudio_system.h>
#include <nsaudio_source.h>
#include <nsengine.h>

audio_stream_obj::audio_stream_obj()
{
	
}

audio_stream_obj::~audio_stream_obj()
{
	sf_close(file);
}

nsaudio_clip::nsaudio_clip():
	nsasset(type_to_hash(nsaudio_clip)),
	m_raw_data(nullptr),
	m_stream_obj(new audio_stream_obj),
	m_al_buffer_name(0),
	m_sample_freq(0),
	m_channel_cnt(0),
	m_frame_cnt(0),
	m_seekable(true)
{
}

nsaudio_clip::nsaudio_clip(const nsaudio_clip & copy):
	nsasset(copy.type()),
	m_raw_data(nullptr),
	m_stream_obj(copy.m_stream_obj),
	m_al_buffer_name(0),
	m_sample_freq(copy.m_sample_freq),
	m_channel_cnt(copy.m_channel_cnt),
	m_frame_cnt(copy.m_frame_cnt),
	m_seekable(copy.m_seekable)
{
	if (copy.m_raw_data != nullptr)
	{
		m_raw_data = new int16[m_frame_cnt];
		for (uint32 i = 0; i < m_frame_cnt; ++i)
			m_raw_data[i] = copy.m_raw_data[i];
	}
}
	
nsaudio_clip::~nsaudio_clip()
{}

int16 * nsaudio_clip::pcm_data()
{
	return m_raw_data;
}

uint32 nsaudio_clip::al_id()
{
	return m_al_buffer_name;
}

int32 nsaudio_clip::sample_freq()
{
	return m_sample_freq;
}

int32 nsaudio_clip::pcm_data_byte_size()
{
	return sizeof(int16) * m_frame_cnt * m_channel_cnt;
}

bool nsaudio_clip::streamable()
{
	return m_seekable;
}

int32 nsaudio_clip::stream_data(uint32 frame_offset, uint32 frame_count, int16 * data)
{
	if (!m_seekable)
		return 0;
	
	if (m_stream_obj->file == nullptr)
	{
		dprint("nsaudio_clip::stream_data Error streaming data - file was not correctly loaded for streaming");
		return 0;
	}

	// If we are trying to get more than is available - set it to whatever is available
	if (frame_offset + frame_count >= m_frame_cnt)
		frame_count = m_frame_cnt - frame_offset;

	if (sf_seek(m_stream_obj->file, frame_offset, SEEK_SET) == -1)
	{
		dprint("nsaudio_clip::stream_data Error seeking in file - check bounds");
		return 0;
	}
	
	return sf_readf_short(m_stream_obj->file, data, frame_count);
}

void nsaudio_clip::read_data()
{
	if (m_stream_obj->file == nullptr)
	{
		dprint("nsaudio_clip::read_data Error reading data - file was not correctly loaded for reading");
		return;
	}

	if (m_seekable)
	{
		if (sf_seek(m_stream_obj->file, 0, SEEK_SET) == -1)
		{
			dprint("nsaudio_clip::read_data Error seeking in file - check bounds");
			return;
		}
	}

	if (m_raw_data != nullptr)
		free(m_raw_data);
	m_raw_data = (int16*)malloc(pcm_data_byte_size());

	if (m_raw_data == nullptr)
	{
		dprint("nsaudio_clip::read_data OUT OF MEMORY");
		return;
	}
	
	int32 fc = sf_readf_short(m_stream_obj->file, m_raw_data, m_frame_cnt);
	if (fc != m_frame_cnt)
	{
		dprint("nsaudio_clip::read_data Warning - read less than frame count - setting frame count to new size");
		m_frame_cnt = fc;
	}
	
	int fmt;
        (m_channel_cnt == 1) ? fmt = AL_FORMAT_MONO16 : fmt = AL_FORMAT_STEREO16;
	
	alBufferData(
		m_al_buffer_name,
		fmt,
		m_raw_data,
		m_channel_cnt*m_frame_cnt*sizeof(int16),
		m_sample_freq);
	al_error_check("nsaudio_clip::read_data");
}

void nsaudio_clip::init()
{
	if (m_al_buffer_name != 0)
	{
		dprint("nsaudio_clip::init - Buffer already initialized");
		return;
	}
	alGenBuffers(1, &m_al_buffer_name);
	al_error_check("nsaudio_clip::init");
}

void nsaudio_clip::release()
{
	if (m_raw_data != nullptr)
		free(m_raw_data);
	m_raw_data = nullptr;
	alDeleteBuffers(1, &m_al_buffer_name);
	al_error_check("nsaudio_clip::release");
}

void nsaudio_clip::finalize()
{
	
}

int8 nsaudio_clip::channel_count()
{
	return m_channel_cnt;
}

uivec3_vector nsaudio_clip::resources()
{
	return uivec3_vector();
}

void nsaudio_clip::name_change(const uivec2 & oldid, const uivec2 newid)
{
		
}

void nsaudio_clip::pup(nsfile_pupper * p)
{
		
}

nsaudio_clip & nsaudio_clip::operator=(nsaudio_clip rhs)
{
	nsasset::operator=(rhs);
	return *this;
}
