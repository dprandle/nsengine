#include <component/nsaudio_source_comp.h>
#include <nsengine.h>
#include <AL/al.h>
#include <float.h>
#include <asset/nsaudio_clip.h>

al_streaming_buffer::al_streaming_buffer():
	al_buf(),
	buf(),
	cur_ind(0),
	stream_offset(0),
	streaming(false),
	buf_size(DEFAULT_STREAM_BUFFER_SAMPLE_SIZE)
{}

al_streaming_buffer::~al_streaming_buffer()
{
	release();
	free(buf[0]);
	free(buf[1]);
}

void al_streaming_buffer::init()
{
	if (al_buf[0] == 0 && al_buf[1] == 0)
		alGenBuffers(2, al_buf);
	else
	{
		dprint("al_streaming_buffer::init Error buffers not deleted before initializing");
	}
}

void al_streaming_buffer::release()
{
	alDeleteBuffers(2, al_buf);
	al_buf[0] = 0; al_buf[1] = 0;
}

source_obj::source_obj():
	buffer(nullptr),
	al_source(0),
	update(true),
	loop(false),
	pitch_multiplier(1.0f),
	gain(1.0f),
	max_distance(FLT_MAX),
	rolloff(1.0f),
	reference_distance(1.0f),
	min_gain(0.0f),
	max_gain(1.0f),
	cone_outer_gain(0.0f),
	cone_inner_outer_angle(fvec2(360.0f,360.0f))
{}

source_obj::~source_obj()
{
	release();
}

void source_obj::allocate_stream_buffers()
{
	if (buffer == nullptr)
	{
		dprint("source_obj::allocate_stream_buffers Need to assign audio clip before allocating stream buffers");
	}
	if (stream_obj.buf[0] != nullptr)
		free(stream_obj.buf[0]);
	if (stream_obj.buf[1] != nullptr)
		free(stream_obj.buf[1]);

	uint32 byte_size = buffer->channel_count() * sizeof(int16) * stream_obj.buf_size;

	stream_obj.buf[0] = (int16*)malloc(byte_size);
	stream_obj.buf[1] = (int16*)malloc(byte_size);
	stream_obj.cur_ind = 0;
	stream_obj.stream_offset = 0;
}

void source_obj::init()
{
	if (al_source != 0)
	{
		dprint("source_obj::init Source buffer is non zero - forget to call release before init?");
		return;
	}
	alGenSources(1, &al_source);
	stream_obj.init();
}

void source_obj::release()
{
	alDeleteSources(1, &al_source);
	al_source = 0;
	stream_obj.release();
}

nsaudio_source_comp::nsaudio_source_comp():
	nscomponent(type_to_hash(nsaudio_source_comp))
{}

nsaudio_source_comp::nsaudio_source_comp(const nsaudio_source_comp & copy):
	nscomponent(copy.m_hashed_type),
	sources()
{}

nsaudio_source_comp::~nsaudio_source_comp()
{
	while (sources.begin() != sources.end())
	{
		delete sources.back();
		sources.pop_back();
	}
}

void nsaudio_source_comp::init()
{
	for (uint32 i = 0; i < sources.size(); ++i)
		sources[i]->init();
}

void nsaudio_source_comp::release()
{
	for (uint32 i = 0; i < sources.size(); ++i)
		sources[i]->release();
}

void nsaudio_source_comp::finalize()
{
		
}

void nsaudio_source_comp::name_change(const uivec2 &, const uivec2)
{
		
}

uivec3_vector nsaudio_source_comp::resources()
{
	return uivec3_vector();
}

void nsaudio_source_comp::pup(nsfile_pupper * p)
{
		
}
