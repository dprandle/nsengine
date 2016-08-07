
#include <nsaudio_system.h>
#include <AL/al.h>
#include <AL/alc.h>


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
	m_device(nullptr),
	m_ctxt(nullptr)
{
	
}

nsaudio_system::~nsaudio_system()
{
	
}

void nsaudio_system::init()
{
    m_device = alcOpenDevice(nullptr);
    if(!m_device)
    {
		dprint("nsaudio_system::init Unable to initialize openAL audio device");
		
    }
	else
	{
		dprint("nsaudio_system::init Successfully initialized openAL audio device");
	}
    m_ctxt = alcCreateContext(m_device, nullptr);
    if(!m_ctxt)
    {
		dprint("nsaudio_system::init Unable to initialize openAL audio context");
	}
	else
	{
		dprint("nsaudio_system::init Successfully initialized openAL audio context");
	}
	alcMakeContextCurrent(m_ctxt);
}

void nsaudio_system::release()
{
	
}

void nsaudio_system::update()
{
	
}

int32 nsaudio_system::update_priority()
{
	return AUDIO_SYS_UPDATE_PR;
}
