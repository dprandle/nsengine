
#ifndef NSAUDIO_SOURCE_COMP_H
#define NSAUDIO_SOURCE_COMP_H

#define DEFAULT_STREAM_BUFFER_SAMPLE_SIZE 1024

#include <component/nscomponent.h>

class nsaudio_clip;

struct al_streaming_buffer
{
	al_streaming_buffer();
	~al_streaming_buffer();

	void init();
	void release();
	
	uint32 al_buf[2];
	int16 * buf[2];
	uint8 cur_ind;
	uint32 stream_offset;
	uint32 buf_size;
	bool streaming;
};

struct source_obj
{
	source_obj();
	~source_obj();

	void init();
	void release();
	void allocate_stream_buffers();
	
	nsaudio_clip * buffer;
	uint32 al_source;
	al_streaming_buffer stream_obj;
	
	bool update;
	bool loop;
	float pitch_multiplier;
	float gain;
	float max_distance;
	float rolloff;
	float reference_distance;
	float min_gain;
	float max_gain;
	float cone_outer_gain;
	fvec2 cone_inner_outer_angle; // degrees
};

class nsaudio_source_comp : public nscomponent
{
  public:
	
	nsaudio_source_comp();

	nsaudio_source_comp(const nsaudio_source_comp & copy);

	nsaudio_source_comp & operator=(nsaudio_source_comp rhs_);

	~nsaudio_source_comp();

	void init();

	void release();

	void finalize();

	void name_change(const uivec2 &, const uivec2);

	uivec3_vector resources();

	void pup(nsfile_pupper * p);

	std::vector<source_obj*> sources;
};

#endif
