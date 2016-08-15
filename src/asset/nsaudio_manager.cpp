/*!
\file nsaudio_manager.cpp

\brief Header file for nsaudio_manager class

This file contains all of the neccessary definitions for the nsaudio_manager class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#include <nsengine.h>
#include <asset/nsaudio_manager.h>
#include <sndfile.h>
#include <asset/nsaudio_clip.h>
#include <nsplatform.h>
#include <algorithm>

nsaudio_manager::nsaudio_manager():
	nsasset_manager(type_to_hash(nsaudio_manager))
{
	set_local_dir(LOCAL_AUDIO_DIR_DEFAULT);
}

nsaudio_manager::~nsaudio_manager()
{}

nsaudio_clip * nsaudio_manager::load(uint32 res_type_id, const nsstring & fname, bool finalize_)
{
	if (fname.empty())
		return NULL;
	
	nsstring res_name(fname);
	nsstring res_ext;
	nsstring f_name;
	nsstring sub_dir;
	bool should_prefix = false;
	
	nsstring prefixdirs = m_res_dir + m_local_dir;

	size_t pos = res_name.find_last_of("/\\");
	if (pos != nsstring::npos)
	{
		if (res_name[0] != '/' && res_name[0] != '.' && res_name.find(":") == nsstring::npos) // then subdir
		{
			sub_dir = res_name.substr(0, pos + 1);
			should_prefix = true;
		}
		res_name = res_name.substr(pos + 1);
	}
	else
		should_prefix = true;

	size_t extPos = res_name.find_last_of(".");
	res_ext = res_name.substr(extPos);
	res_name = res_name.substr(0, extPos);
	std::transform(res_ext.begin(), res_ext.end(), res_ext.begin(), ::tolower);

	if (should_prefix)
		f_name = prefixdirs + sub_dir + res_name + res_ext;
	else
		f_name = fname;

	if (res_ext != ".ogg" && res_ext != ".wav" && res_ext != ".aiff" && res_ext != ".flac")
	{
		dprint("nsaudio_manager::load Audio format " + res_ext + " is not supported");
		return nullptr;
	}
	
	SF_INFO si = {};
    SNDFILE * file = nullptr;
    file = sf_open(f_name.c_str(), SFM_READ, &si);
    if (file == nullptr)
	{
		dprint("nsaudio_manager::load Error opening audio file " + f_name);
		return nullptr;
	}
	
	if (si.channels > 2 || si.channels < 1)
	{
		dprint("nsaudio_manager::load Cannot use " + f_name + " as it has " + std::to_string(si.channels) + " channels (it needs to have 1 or 2)");
		sf_close(file);
		return nullptr;		
	}		

	nsaudio_clip * audio_clip = get<nsaudio_clip>(res_name);
	if (audio_clip == nullptr)
	{
		audio_clip = create<nsaudio_clip>(res_name);
	}
	else
	{
		dprint("nsasset_manager::load - Error trying to load audio clip with name " + res_name + " as clip already exists with same name");
		sf_close(file);
		return nullptr;
	}

	audio_clip->m_stream_obj->file = file;
	audio_clip->set_subdir(sub_dir);
	audio_clip->set_ext(res_ext);
	audio_clip->m_channel_cnt = si.channels;
	audio_clip->m_seekable = si.seekable == 1; // don't complain about casting during compile
	audio_clip->m_sample_freq = si.samplerate;
	audio_clip->m_frame_cnt = si.frames;
	
	dprint("nsaudio_manager::load Succesfully loaded audio clip from file " + f_name);

	if (finalize_)
		audio_clip->finalize();

	return audio_clip;
}

bool nsaudio_manager::save(nsasset * clipa, const nsstring & path)
{
	nsaudio_clip * clip = (nsaudio_clip *)clipa;
	if (clip == nullptr)
		return false;

	nsstring f_name(clip->name() + clip->extension());

	if (path == "")
		f_name = m_res_dir + m_local_dir + clip->subdir() + f_name;
	else
		f_name = path + f_name;
	// otherwise create in cwd

    bool fret = platform::create_dir(f_name);
	if (fret)
	{
		dprint("nsaudio_manager::save Created directory " + f_name);
	}

	SF_INFO si = {};
	if (clip->extension() == ".ogg")
		si.format = SF_FORMAT_OGG | SF_FORMAT_VORBIS;
	else if (clip->extension() == ".wav")
		si.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
	else if (clip->extension() == ".aiff")
		si.format = SF_FORMAT_AIFF | SF_FORMAT_PCM_16;
	else if (clip->extension() == ".flac")
		si.format = SF_FORMAT_FLAC | SF_FORMAT_PCM_16;
	else
	{
		dprint("nsaudio_manager::save Cannot save " + clip->extension() + " format - not supported");
		return false;
	}
	si.samplerate = clip->sample_freq();
	si.channels = clip->channel_count();
	
	SNDFILE * file = nullptr;
    file = sf_open(f_name.c_str(), SFM_WRITE, &si);
    if (file == nullptr)
	{
		dprint("nsaudio_manager::save Error opening audio file " + f_name);
		return false;
	}

	if (clip->pcm_data() == nullptr)
	{
		clip->read_data();
		sf_writef_short(file, clip->pcm_data(), clip->m_frame_cnt);
		clip->release();
		clip->init();
	}
	else
	{
		sf_writef_short(file, clip->pcm_data(), clip->m_frame_cnt);
	}
	
	dprint("nsaudio_manager::save Succesfully saved " + clip->name() + " to file " + f_name);
	sf_close(file);
	return true;
}
