#include <system/nsaudio_system.h>
#include <AL/al.h>
#include <AL/alc.h>
#include <component/nsaudio_source_comp.h>
#include <nsentity.h>
#include <asset/nsaudio_clip.h>
#include <component/nstform_comp.h>
#include <nsworld_data.h>


ALCcontext * alc_context;

void al_error_check(const nsstring & func)
{
#ifdef NSDEBUG
	ALenum err = alGetError();
	if (err != AL_NO_ERROR)
	{
		nsstring msg;
		switch (err)
		{
		  case(AL_INVALID_NAME):
			  msg = "A bad name (ID) was passed to an OpenAL function";
		  case(AL_INVALID_ENUM):
			  msg = "An invalid enum value was passed to an OpenAL function";
		  case(AL_INVALID_VALUE):
			  msg = "An invalid value was passed to an OpenAL function";
		  case(AL_INVALID_OPERATION):
			  msg = "The requested operation is not valid";
		  case(AL_OUT_OF_MEMORY):
			  msg = "The requested operation resulted in OpenAL running out of memory";
		  default:
			  msg = "Unknown error";
		}
		dprint(func + " - AL Error: " + msg);
	}
#endif
}

nsaudio_system::nsaudio_system():
	nssystem(type_to_hash(nsaudio_system)),
	master_gain(1.0f),
	units_per_meter(1.0f)
{
	
}

nsaudio_system::~nsaudio_system()
{}

void nsaudio_system::init()
{
    ALCdevice * alc_device = alcOpenDevice(nullptr);
    if(!alc_device)
    {
		dprint("nsaudio_system::init Unable to initialize openAL audio device");		
    }
	else
	{
		dprint("nsaudio_system::init Successfully initialized openAL audio device");
	}
    alc_context = alcCreateContext(alc_device, nullptr);
    if(!alc_context)
    {
		dprint("nsaudio_system::init Unable to initialize openAL audio context");
	}
	else
	{
		dprint("nsaudio_system::init Successfully initialized openAL audio context");
	}
	alcMakeContextCurrent(alc_context);

	register_handler(nsaudio_system::handle_audio_play_event);
	register_handler(nsaudio_system::handle_audio_start_streaming_event);
	register_handler(nsaudio_system::handle_audio_stop_streaming_event);
}

void nsaudio_system::release()
{
	ALCdevice * alc_device = alcGetContextsDevice(alc_context);
	alcMakeContextCurrent(nullptr);
	alcDestroyContext(alc_context);
    alcCloseDevice(alc_device);
}

void nsaudio_system::update()
{
    if (chunk_error_check())
		return;

	fvec3 pos;
	fvec3 vel;
	fbox dir(fvec3(0.0f,0.0f,-1.0f),fvec3(0.0f,1.0f,0.0f));
	
	nsentity * listener_ent = m_active_chunk->find_entity(listener.y);
	if (listener_ent != nullptr)
	{
		nstform_comp * tfc = listener_ent->get<nstform_comp>();
		if (tfc != nullptr)
		{
			pos = tfc->world_position() / units_per_meter;
			vel = fvec3(0.0f);//tfc->phys.velocity / units_per_meter;
			fquat ori = tfc->world_orientation();
			dir.a = ori.target(); dir.b = ori.up();
		}
	}

	alListenerf(AL_GAIN,master_gain);
	//alListenerfv(AL_POSITION,pos.data);
	//alListenerfv(AL_VELOCITY,vel.data);
	//alListenerfv(AL_ORIENTATION,dir.a.data);
	
	entity_set * es = m_active_chunk->entities_with_comp<nsaudio_source_comp>();
	if (es == nullptr)
		return;
	
	auto ent_iter = es->begin();
	while (ent_iter != es->end())
	{
		nsaudio_source_comp * asc = (*ent_iter)->get<nsaudio_source_comp>();
		auto source_iter = asc->sources.begin();
		while (source_iter != asc->sources.end())
		{
			if ((*source_iter)->update)
			{
				if ((*source_iter)->buffer != nullptr)
				{
					alSourcei((*source_iter)->al_source, AL_BUFFER, (*source_iter)->buffer->al_id());
					alSourcei((*source_iter)->al_source, AL_LOOPING, (*source_iter)->loop);
                    alSourcef((*source_iter)->al_source, AL_PITCH, (*source_iter)->pitch_multiplier);
					alSourcef((*source_iter)->al_source, AL_GAIN, (*source_iter)->gain);
                    alSourcef((*source_iter)->al_source, AL_MAX_DISTANCE, (*source_iter)->max_distance);
					alSourcef((*source_iter)->al_source, AL_ROLLOFF_FACTOR, (*source_iter)->rolloff);
					alSourcef((*source_iter)->al_source, AL_REFERENCE_DISTANCE, (*source_iter)->reference_distance);
					alSourcef((*source_iter)->al_source, AL_CONE_OUTER_GAIN, (*source_iter)->cone_outer_gain);
					alSourcef((*source_iter)->al_source, AL_CONE_INNER_ANGLE, (*source_iter)->cone_inner_outer_angle.x);
					alSourcef((*source_iter)->al_source, AL_CONE_OUTER_ANGLE, (*source_iter)->cone_inner_outer_angle.y);
				}
				else
				{
					dprint("nsaudio_system::update - update requested in audio source comp with no buffer assigned");
				}
				(*source_iter)->update = false;
			}
			
			if ((*source_iter)->stream_obj.streaming && (*source_iter)->buffer != nullptr)
			{
				int32 numb = 0;
				alGetSourcei((*source_iter)->al_source, AL_BUFFERS_PROCESSED, &numb);
				if (numb != 0)
				{
					int32 unq_ind = 1 - (*source_iter)->stream_obj.cur_ind;
					alSourceUnqueueBuffers((*source_iter)->al_source, 1, &(*source_iter)->stream_obj.al_buf[unq_ind]);

					int32 frame_cnt = (*source_iter)->buffer->stream_data((*source_iter)->stream_obj.stream_offset, (*source_iter)->stream_obj.buf_size, (*source_iter)->stream_obj.buf[unq_ind]);

					if (frame_cnt != (*source_iter)->stream_obj.buf_size)
						(*source_iter)->stream_obj.stream_offset = 0;
					else
						(*source_iter)->stream_obj.stream_offset += frame_cnt;

					int fmt = 0;
					((*source_iter)->buffer->channel_count() == 1) ? fmt = AL_FORMAT_MONO16 : fmt = AL_FORMAT_STEREO16;

					alBufferData((*source_iter)->stream_obj.al_buf[unq_ind],
								 fmt,
								 (*source_iter)->stream_obj.buf[unq_ind],
								 (*source_iter)->buffer->channel_count() * frame_cnt * sizeof(int16),
								 (*source_iter)->buffer->sample_freq());

					alSourceQueueBuffers((*source_iter)->al_source, 1, &(*source_iter)->stream_obj.al_buf[unq_ind]);

					(*source_iter)->stream_obj.cur_ind = unq_ind;
				}
			}
            ++source_iter;
		}
		++ent_iter;
	}
}

bool nsaudio_system::handle_audio_play_event(audio_play_event * evnt)
{
	if (m_active_chunk == nullptr)
		return true;

	nsentity * ent = m_active_chunk->find_entity(evnt->source_id.y);
	if (ent == nullptr)
		return true;

	fvec3 pos, vel, dir;
	nstform_comp * tfc = ent->get<nstform_comp>();
	if (tfc != nullptr)
	{
		pos = tfc->world_position() / units_per_meter;
		vel = fvec3(1.0f);
		dir = tfc->world_orientation().target().normalize();
		dir.z *= -1.0f;
	}

	nsaudio_source_comp * ac = ent->get<nsaudio_source_comp>();
    alSourcefv(ac->sources[evnt->effect_index]->al_source, AL_POSITION, pos.data);
    alSourcefv(ac->sources[evnt->effect_index]->al_source, AL_VELOCITY, vel.data);
	alSourcefv(ac->sources[evnt->effect_index]->al_source, AL_DIRECTION, dir.data);
	alSourcePlay(ac->sources[evnt->effect_index]->al_source);
	return true;
}

bool nsaudio_system::handle_audio_start_streaming_event(audio_start_streaming_event * evnt)
{
	if (m_active_chunk == nullptr)
		return true;
	
	nsentity * src = m_active_chunk->find_entity(evnt->source_id.y);
	
	fvec3 pos, vel, dir;
	nstform_comp * tfc = src->get<nstform_comp>();
	if (tfc != nullptr)
	{
		pos = tfc->world_position() / units_per_meter;
		vel = fvec3(1.0f);//itf.phys.velocity  / units_per_meter;
		dir = tfc->world_orientation().target().normalize();
		dir.z *= -1.0f;
	}

	nsaudio_source_comp * ac = src->get<nsaudio_source_comp>();
	source_obj * sobj = ac->sources[evnt->effect_index];

	if (sobj->buffer->frame_count() < sobj->buffer->sample_freq() / 4)
	{
		dprint("nsaudio_system::handle_audio_start_streaming_event Cannot stream buffer that is less than .25 s long");
		return true;
	}

	if (sobj->stream_obj.buf_size * 2 + sobj->buffer->sample_freq() / 4 > sobj->buffer->frame_count())
	{
		dprint("nsaudio_system::handle_audio_start_streaming_event Streaming buffers should be smaller for such a short duration buffer ");
		return true;
	}
	
    alSourcefv(sobj->al_source, AL_POSITION, pos.data);
    alSourcefv(sobj->al_source, AL_VELOCITY, vel.data);
	alSourcefv(sobj->al_source, AL_DIRECTION, dir.data);

	sobj->stream_obj.cur_ind = 0;
	sobj->stream_obj.stream_offset = 0;

	int fmt;
	(sobj->buffer->channel_count() == 1) ? fmt = AL_FORMAT_MONO16 : fmt = AL_FORMAT_STEREO16;
	// fill up both buffers

	uint32 byte_size = 0;
	int32 frame_cnt = 0;
	
	for (sobj->stream_obj.cur_ind = 0; sobj->stream_obj.cur_ind < 2; ++sobj->stream_obj.cur_ind)
	{
		frame_cnt = sobj->buffer->stream_data(sobj->stream_obj.stream_offset, sobj->stream_obj.buf_size, sobj->stream_obj.buf[sobj->stream_obj.cur_ind]);
		sobj->stream_obj.stream_offset += frame_cnt;
		byte_size = sobj->buffer->channel_count() * sizeof(int16) * frame_cnt;
		alBufferData(
			sobj->stream_obj.al_buf[sobj->stream_obj.cur_ind],
			fmt,
			sobj->stream_obj.buf[sobj->stream_obj.cur_ind],
			byte_size,
			sobj->buffer->sample_freq());
		alSourceQueueBuffers(sobj->al_source, 1, &sobj->stream_obj.al_buf[sobj->stream_obj.cur_ind]);
	}

	--sobj->stream_obj.cur_ind;
	// fill up buffers
	alSourcePlay(sobj->al_source);
	sobj->stream_obj.streaming = true;

	return true;		
}
	
bool nsaudio_system::handle_audio_stop_streaming_event(audio_stop_streaming_event * evnt)
{
	return true;
}

int32 nsaudio_system::update_priority()
{
	return AUDIO_SYS_UPDATE_PR;
}
