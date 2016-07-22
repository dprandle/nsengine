/*!
\file nsresmanager.cpp

\brief Header file for nsasset_manager class

This file contains all of the neccessary definitions for the nsasset_manager class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#include <nsengine.h>
#include <nsasset_manager.h>
#include <nspupper.h>
#include <nsplatform.h>
#include <nsfactory.h>
#include <nscallback.h>
#include <hash/sha256.h>
#include <nsasset.h>

nsasset_manager::nsasset_manager(uint32 hashed_type):
m_res_dir(),
m_local_dir(),
m_id_resmap(),
m_plugin_id(),
m_hashed_type(hashed_type),
m_save_mode(binary)
{}


nsasset_manager::~nsasset_manager()
{
	destroy_all();
}

bool nsasset_manager::add(nsasset * res)
{
	if (res == NULL)
	{
		dprint("nsasset_manager::add Can not add NULL valued resource");
		return false;
	}

	auto check = m_id_resmap.emplace(res->id(), res);
	if (check.second)
	{
		dprint("nsasset_manager::add Successfully added resource with name " + res->name());
		res->m_plugin_id = m_plugin_id;
		res->m_owned = true;
	}
	else
	{
		dprint("nsasset_manager::add Could not add resource with name " + res->name() + " because resource with that name already exists");
	}
	return check.second;	
}

nsasset_manager::map_type::iterator nsasset_manager::begin()
{
	return m_id_resmap.begin();
}

bool nsasset_manager::changed(nsasset * res, nsstring fname)
{
	if (fname == "")
		fname = m_res_dir + m_local_dir + res->subdir() + res->name() + res->extension();

	save_as(res, ".tmp");

	ui8_vector v1, v2;
	platform::read(".tmp",&v1);
	platform::read(fname, &v2);

	if (v1.empty() || v2.empty())
		return true;

	SHA256 sha256;
	std::string s1 = sha256(&v1[0], v1.size());
	std::string s2 = sha256(&v2[0], v2.size());
	platform::remove(".tmp");
	return (s1 != s2);
}

uint32 nsasset_manager::type()
{
	return m_hashed_type;
}

nsasset * nsasset_manager::create(uint32 res_type_id, const nsstring & resName, nsasset * to_copy)
{
	if (resName.empty())
		return nullptr;
	
	// Create the resource and add it to the map - if there is a resource with the same name already
	// in the map then insertion will have failed, so delete the created resource and retun NULL
	nsasset * res = nse.factory<nsasset_factory>(res_type_id)->create(to_copy);
	res->rename(resName);
	if (!add(res))
	{
		dprint("nsasset_manager::create Deleting unadded " + nse.guid(res_type_id) + " with name " + resName);
		delete res;
		return NULL;
	}
	res->init();
	return res;
}

nsasset * nsasset_manager::create(const nsstring &guid_, const nsstring &res_name, nsasset * to_copy)
{
	return create(hash_id(guid_), res_name, to_copy);
}

bool nsasset_manager::contains(nsasset * res)
{
	if (res == NULL)
		return false;
	return (get(res->id()) != NULL);
}

uint32 nsasset_manager::count() const
{
	return static_cast<uint32>(m_id_resmap.size());
}

bool nsasset_manager::del(nsasset * res)
{
	nsstring dir = m_res_dir + m_local_dir + res->subdir();
	nsstring fName = dir + res->name() + res->extension();
    bool ret = platform::remove(fName);

	if (ret)
	{
		dprint("nsasset_manager::del - Succesfully deleted file with name: " + fName);
		destroy(res);
	}
	else
	{
		dprint("nsasset_manager::del - Could not delete file with name: " + fName);
	}
	
	return ret;
}

nsasset_manager::map_type::iterator nsasset_manager::end()
{
	return m_id_resmap.end();
}

bool nsasset_manager::empty()
{
	return m_id_resmap.empty();
}

void nsasset_manager::finalize_all()
{
	auto iter = begin();
	while (iter != end())
	{
		iter->second->finalize();
		++iter;
	}
}
nsasset * nsasset_manager::get(uint32 resid)
{
	map_type::iterator iter = m_id_resmap.find(resid);
	if (iter != m_id_resmap.end())
		return iter->second;
	return NULL;
}

nsasset * nsasset_manager::get(const nsstring & resName)
{
	return get(hash_id(resName));
}

nsasset * nsasset_manager::get(nsasset * res)
{
	return get(res->id());
}

nsasset_manager::s_mode nsasset_manager::save_mode() const
{
	return m_save_mode;
}

void nsasset_manager::set_save_mode(s_mode sm)
{
	m_save_mode = sm;
}

nsasset * nsasset_manager::load(const nsstring & res_guid,
								 const nsstring & fname,
								 bool finalize_)
{
	return load(hash_id(res_guid), fname, finalize_);
}

nsasset * nsasset_manager::load(uint32 res_type_id, const nsstring & fname, bool finalize_)
{
	if (fname.empty())
		return NULL;
	
	nsstring resName(fname);
	nsstring resExtension;
	nsstring fName;
	nsstring subDir;
	bool shouldPrefix = false;
	
	nsstring prefixdirs = m_res_dir + m_local_dir;

	size_t pos = resName.find_last_of("/\\");
	if (pos != nsstring::npos)
	{
		if (resName[0] != '/' && resName[0] != '.' && resName.find(":") == nsstring::npos) // then subdir
		{
			subDir = resName.substr(0, pos + 1);
			shouldPrefix = true;
		}
		resName = resName.substr(pos + 1);
	}
	else
		shouldPrefix = true;

	size_t extPos = resName.find_last_of(".");
	resExtension = resName.substr(extPos);
	resName = resName.substr(0, extPos);

	if (shouldPrefix)
		fName = prefixdirs + subDir + resName + resExtension;
	else
		fName = fname;

	nsfstream file;
	nsfile_pupper * p;
	if (m_save_mode == binary)
	{
		file.open(fName, nsfstream::in | nsfstream::binary);
		p = new nsbinary_file_pupper(file, PUP_IN);
	}
	else
	{
		file.open(fName, nsfstream::in);
		p = new nstext_file_pupper(file, PUP_IN);
	}

	if (!file.is_open())
	{
		dprint("nsasset_manager::load Error opening resource file " + fName);
		delete p;
		return NULL;
	}

	nsasset * res = get(resName);
	nsstring restype(nse.guid(res_type_id));
	if (res == NULL)
	{
		res = create(res_type_id, resName);
	}
	else
	{
		dprint("nsasset_manager::load - Error trying to load " + restype  + " with same name as already existing " + restype + " - " + resName);
		file.close();
		delete p;
		return NULL;
	}

	res->set_subdir(subDir);
	res->set_ext(resExtension);

	nsstring rt;

	if (m_save_mode == binary)
		pup(*(static_cast<nsbinary_file_pupper*>(p)), rt, "type");
	else
		pup(*(static_cast<nstext_file_pupper*>(p)), rt, "type");

	if (rt != nse.guid(res_type_id))
	{
		dprint("nsasset_manager::load Attempted to load resource type " + restype + " from file that is not of that type: " + fName);
		delete p;
		file.close();
		destroy(res);
		return NULL;
	}


	res->pup(p);
	dprint("nsasset_manager::load Succesfully loaded resource from file " + fName);
	if (finalize_)
		res->finalize();
	delete p;
	file.close();
	return res;
}

const nsstring & nsasset_manager::local_dir()
{
	return m_local_dir;
}

const nsstring & nsasset_manager::res_dir()
{
	return m_res_dir;
}

/*!
This should be called if there was a name change to a resource - will check if the resource is used by this component and if is
is then it will update the handle
*/
void nsasset_manager::name_change(const uivec2 & oldid, const uivec2 newid)
{
	// first see if we need to remove and re-add
	if (oldid.x == m_plugin_id)
	{
		nsasset * res = get<nsasset>(oldid.y);
		if (res != NULL)
		{
			dprint("nsasset_manager::name_change - name change in res_manager " + hash_to_guid(m_hashed_type));
			remove(oldid.y);
			add(res);
		}
	}

	map_type::iterator resIter = m_id_resmap.begin();
	while (resIter != m_id_resmap.end())
	{
		resIter->second->name_change(oldid, newid);
		++resIter;
	}
}

uint32 nsasset_manager::plugin_id()
{
	return m_plugin_id;
}

nsasset * nsasset_manager::remove(uint32 res_id)
{
	nsasset * res = get(res_id);
	if (res == NULL)
		return NULL;
	m_id_resmap.erase(res_id);
	res->m_plugin_id = 0;
	res->m_owned = false;
	dprint("nsasset_manager::remove Succesfully removed resource " + res->name());
	return res;
}

nsasset * nsasset_manager::remove(const nsstring & resName)
{
	return remove(hash_id(resName));
}

nsasset * nsasset_manager::remove(nsasset * res)
{
	if (res != NULL)
		return remove(res->id());
	return res;
}

bool nsasset_manager::rename(const nsstring & oldName, const nsstring & newName)
{
	nsasset * res = get(oldName);

	if (res == NULL) // resource not in map
		return false;

	res->rename(newName); // name change will propagate


	nsstring dir = m_res_dir + m_local_dir + res->subdir();
	nsstring fNameOld = dir + oldName + res->extension();
	nsstring fNameNew = dir + res->name() + res->extension();
    bool ret = platform::rename(fNameOld.c_str(), fNameNew.c_str());
			
    if (ret)
	{
		dprint("nsasset_manager::rename Succesfully renamed file with old name: " + oldName + " to file with new name: " + newName);
	}
	else
	{
		dprint("nsasset_manager::rename Could not rename file with old name: " + oldName + " to file with new name: " + newName);
	}
	
    return ret;
}

void nsasset_manager::save_all(const nsstring & path, nssave_resouces_callback * scallback)
{
	// Iterate through resources
	map_type::iterator iter = m_id_resmap.begin();
	while (iter != m_id_resmap.end() && iter->second->group_save_enabled())
	{
		// Save the current iterated resource to file
		bool success = save(iter->second, path);
		if (scallback != NULL)
		{
			scallback->saved = success;
			scallback->res_id = iter->second->full_id();
			scallback->run();
		}
		++iter;
	}
}

bool nsasset_manager::save(nsasset * res,const nsstring & path)
{
	if (res == NULL)
	{
		dprint("nsasset_manager::save : Cannot save NULL valued resource");
		return false;
	}

	nsstring fName(res->name() + res->extension());

	if (path == "")
		fName = m_res_dir + m_local_dir + res->subdir() + fName;
	else
		fName = path + fName;
	// otherwise create in cwd

    bool fret = platform::create_dir(fName);
	if (fret)
	{
		dprint("nsasset_manager::save Created directory " + fName);
	}

	nsfstream file;
	nsfile_pupper * p;
	if (m_save_mode == binary)
	{
		file.open(fName, nsfstream::out | nsfstream::binary);
		p = new nsbinary_file_pupper(file, PUP_OUT);
	}
	else
	{
		file.open(fName, nsfstream::out);
		p = new nstext_file_pupper(file, PUP_OUT);
	}

	if (!file.is_open())
	{
		dprint("nsasset_manager::save : Error opening file " + fName);
		delete p;
		return false;
	}

	uint32 hashed_type = res->type();
	nsstring rest = nse.guid(hashed_type);

	if (m_save_mode == binary)
		pup(*(static_cast<nsbinary_file_pupper*>(p)), rest, "type");
	else
		pup(*(static_cast<nstext_file_pupper*>(p)), rest, "type");

	res->pup(p);
	dprint("nsasset_manager::save Succesfully saved " + rest + " to file " + fName);
	delete p;
	file.close();
	return true;
}

bool nsasset_manager::save_as(nsasset * res, const nsstring & fname)
{
	nsstring origName = res->name();
	res->rename(name_from_filename(fname));
	bool success = save(res, path_from_filename(fname));
	res->rename(origName);
	return success;
}

void nsasset_manager::set_local_dir(const nsstring & pDirectory)
{
	m_local_dir = pDirectory;
}

void nsasset_manager::set_plugin_id(uint32 plugid)
{
	m_plugin_id = plugid;
}

void nsasset_manager::destroy_all()
{
	while (begin() != end())
	{
		if (!destroy(begin()->second))
			return;
	}
	m_id_resmap.clear();
}

bool nsasset_manager::destroy(nsasset * res)
{
	if (res != NULL)
	{
		uint32 id = res->id();
		nsstring name = res->name();
		res->release();
		delete res;
		m_id_resmap.erase(id);
		dprint("nsasset_manager::destroy Successfully destroyed " + name);
		return true;
	}
	return false;
}

void nsasset_manager::set_res_dir(const nsstring & pDirectory)
{
	m_res_dir = pDirectory;
}

nsstring nsasset_manager::name_from_filename(const nsstring & fname)
{
	// full file path
	nsstring resName(fname);

	// After last / or \\ (ie if bla/blabla/blablabla/name.ext this will get position before name.ext)
	size_t pos = resName.find_last_of("/\\");

	// If position is valid, the extract name.ext. If it is not then means there was likely not a path in the first
	// place and so we leave resName alone.
	if (pos != nsstring::npos && (pos + 1) < fname.size())
		resName = resName.substr(pos + 1);

	// Find the position before the period
	size_t extPos = resName.find_last_of(".");

	// If valid position was found, extract name without characters after the period (name.ext) becomes (name)
	if (extPos != nsstring::npos)
		resName = resName.substr(0, extPos);

	return resName;
}

nsstring nsasset_manager::path_from_filename(const nsstring & fname)
{
	nsstring path = "";

	size_t pos = fname.find_last_of("/\\");
	if (pos != nsstring::npos && (pos + 1) < fname.size())
		path = fname.substr(0, pos + 1);

	return path;
}
