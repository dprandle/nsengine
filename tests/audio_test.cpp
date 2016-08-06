#include <AL/al.h>
#include <AL/alc.h>
#include <sndfile.h>
#include <iostream>
#include <nstypes.h>

int main()
{
	ALCdevice *dev;
    ALCcontext *ctx;

    dev = alcOpenDevice(nullptr);
    if(!dev)
    {
        fprintf(stderr, "Oops\n");
        return 1;
    }
    ctx = alcCreateContext(dev, nullptr);
    alcMakeContextCurrent(ctx);
    if(!ctx)
    {
        fprintf(stderr, "Oops2\n");
        return 1;
    }

	SF_INFO si = {};
	SNDFILE * file = nullptr;
	file = sf_open("import/example2.ogg", SFM_READ, &si);
	if (file == nullptr)
		std::cout << "Could not load file" << std::endl;
	int16 * buffer = new int16[si.channels*si.frames];
	sf_readf_short(file, buffer, si.frames);

    uint32 al_buf_name=0, source=0;
	alGenBuffers(1, &al_buf_name);

    alBufferData(al_buf_name, AL_FORMAT_STEREO16, buffer, si.channels*si.frames*sizeof(int16), si.samplerate);

	alGenSources(1, &source);

	alSourceQueueBuffers(source, 1, &al_buf_name);
    alSourceQueueBuffers(source, 1, &al_buf_name);
	alSourcePlay(source);
    int x = 0;
	while (x == 0)
	{
		alGetSourcei(source,AL_BUFFERS_PROCESSED, &x);
		if (x != 0)
		{
			alSourceQueueBuffers(source, 1, &al_buf_name);
			x = 0;
		}
	}
	return 0;
}
