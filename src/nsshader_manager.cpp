/*! 
	\file nsshader_manager.cpp
	
	\brief Definition file for nsshader_manager class

	This file contains all of the neccessary definitions for the nsshader_manager class.

	\author Daniel Randle
	\date November 23 2013
	\copywrite Earth Banana Games 2013
*/

#include <nsshader_manager.h>
#include <nsfile_os.h>
#include <nsengine.h>

nsshader_manager::nsshader_manager():nsres_manager()
{
	set_local_dir(LOCAL_SHADER_DIR_DEFAULT);
	set_save_mode(text);
}

nsshader_manager::~nsshader_manager()
{}

bool nsshader_manager::compile(nsshader * sh)
{
	return sh->compile();		
}

bool nsshader_manager::compile_all()
{
	bool ret = true;
	map_type::iterator iter = begin();
	while (iter != end())
	{
		ret = ((nsshader*)iter->second)->compile() && ret;
		++iter;
	}
	return ret;
}

void nsshader_manager::init_uniforms(nsshader * sh)
{
	sh->init_uniforms();
}

void nsshader_manager::init_uniforms_all()
{
	map_type::iterator iter = begin();
	while (iter != end())
	{
		nsshader * shader = static_cast<nsshader*>(iter->second);
		shader->bind();
		shader->init_uniforms();
		shader->unbind();
		++iter;
	}
}

bool nsshader_manager::load_stage(nsshader * sh, const nsstring & fname, nsshader::shader_type stagetype)
{
	nsstring fName;
	bool shouldPrefix = false;
	nsstring prefixdirs = m_res_dir + m_local_dir;

	size_t pos = fname.find_last_of("/\\");
	if (pos != nsstring::npos)
	{
		if (fname[0] != '/' && fname[0] != '.' && fname.find(":") == nsstring::npos) // then subdir
			shouldPrefix = true;
	}
	else
		shouldPrefix = true;

	if (shouldPrefix)
		fName = prefixdirs + fname;
	else
		fName = fname;
	
	nsfstream file;
	NSFilePUPer * p;
	if (m_save_mode == binary)
	{
		file.open(fName, nsfstream::in | nsfstream::binary);
		p = new NSBinFilePUPer(file, PUP_IN);
	}
	else
	{
		file.open(fName, nsfstream::in);
		p = new NSTextFilePUPer(file, PUP_IN);
	}

	if (!file.is_open())
	{
		dprint("nsshader_manager::loadStage : Error opening file with name - " + fName);
		delete p;
		return false;
	}

	nsstring rt;
	if (m_save_mode == binary)
		pup(*(static_cast<NSBinFilePUPer*>(p)), rt, "type");
	else
		pup(*(static_cast<NSTextFilePUPer*>(p)), rt, "type");

	if (rt != nsengine.guid(sh->type()) + "_stage:" + sh->stage_name(stagetype))
	{
		dprint("nsshader_manager::loadStage Attempted to load shader stage " + fName + " which is not of stage type " + std::to_string(stagetype));
		delete p;
		file.close();
		destroy(sh);
		return false;
	}

	sh->pup(p, stagetype);
	dprint("nsshader_manager::loadStage - Succesfully loaded stage " + sh->stage_name(stagetype) + " from file " + fName);
	delete p;
	file.close();
	return true;
}

bool nsshader_manager::save_stage(nsshader * sh, const nsstring & filename, nsshader::shader_type stagetype)
{
	nsstring fName;
	bool shouldPrefix = false;
	
	size_t pos = filename.find_last_of("/\\");
	if (pos != nsstring::npos)
	{
		if (filename[0] != '/' && filename[0] != '.' && filename.find(":") == nsstring::npos) // then subdir
			shouldPrefix = true;
	}
	else
		shouldPrefix = true;

	if (shouldPrefix)
		fName = m_res_dir + m_local_dir + filename;
	else
		fName = filename;

	bool fret = nsfileio::create_dir(fName);
	if (fret)
	{
		dprint("nsshader_manager::save Created directory " + fName);
	}

	nsfstream file;
	NSFilePUPer * p;
	if (m_save_mode == binary)
	{
		file.open(fName, nsfstream::out | nsfstream::binary);
		p = new NSBinFilePUPer(file, PUP_OUT);
	}
	else
	{
		file.open(fName, nsfstream::out);
		p = new NSTextFilePUPer(file, PUP_OUT);
	}

	if (!file.is_open())
	{
		dprint("nsshader_manager::saveStage : Error opening stage " + sh->stage_name(stagetype) + " file " + fName);
		delete p;
		return false;
	}

	nsstring guid_ = nsengine.guid(sh->type());
    std::string savestr = guid_ + "_stage:" + sh->stage_name(stagetype);
	if (m_save_mode == binary)
        pup(*(static_cast<NSBinFilePUPer*>(p)), savestr, "type");
	else
        pup(*(static_cast<NSTextFilePUPer*>(p)), savestr, std::string("type"));

	sh->pup(p, stagetype);
	dprint("nsshader_manager::saveStage - Succesfully saved stage " + sh->stage_name(stagetype) + " to file " + fName);
	delete p;
	file.close();
	return true;
}

bool nsshader_manager::link(nsshader * sh)
{
	return sh->link();
}

bool nsshader_manager::link_all()
{
	bool ret = true;
	map_type::iterator iter = begin();
	while (iter != end())
	{
		ret = ((nsshader*)iter->second)->link() && ret;
		++iter;
	}
	return ret;
}
