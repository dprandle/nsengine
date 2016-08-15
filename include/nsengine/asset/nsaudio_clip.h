/*! 
	\file nsaudio_source.h
	
	\brief Header file for nsaudio_source class

	This file contains all of the neccessary declarations for the nsaudio_source class.

	\author Daniel Randle
	\date November 23 2013
	\copywrite Earth Banana Games 2013
*/
#ifndef NSAUDIO_CLIP_H
#define NSAUDIO_CLIP_H

#include <asset/nsasset.h>
#include <memory>

struct SNDFILE_tag;

struct audio_stream_obj
{
	audio_stream_obj();
	~audio_stream_obj();
	SNDFILE_tag * file;
};

class nsaudio_clip : public nsasset
{
public:
	friend class nsaudio_manager;

	nsaudio_clip();
	nsaudio_clip(const nsaudio_clip & copy);
	
	~nsaudio_clip();

	int16 * pcm_data();

	uint32 al_id();

	int32 sample_freq();

	int32 frame_count();

	int8 channel_count();

	int32 pcm_data_byte_size();

	bool streamable();

	void init();

	void release();

	void finalize();

	/*
	  Returns the offset from the start of the audio data
	 */
	int32 stream_data(uint32 frame_offset, uint32 frame_count, int16 * data);

	void read_data();

	uivec3_vector resources();

	void name_change(const uivec2 & oldid, const uivec2 newid);

	void pup(nsfile_pupper * p);

	nsaudio_clip & operator=(nsaudio_clip rhs);
	
protected:

	int16 * m_raw_data;
	std::shared_ptr<audio_stream_obj> m_stream_obj;
	uint32 m_al_buffer_name;
	int32 m_sample_freq;
	int8 m_channel_cnt;
	int32 m_frame_cnt;
	bool m_seekable;
};

#endif
