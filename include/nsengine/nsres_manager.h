/*! 
	\file nsres_manager.h
	
	\brief Header file for nsres_manager class

	This file contains all of the neccessary declarations for the nsres_manager class.

	\author Daniel Randle
	\date November 23 2013
	\copywrite Earth Banana Games 2013
*/

#ifndef NSRES_MANAGER_H
#define NSRES_MANAGER_H

#include <nsengine.h>
#include <nsglobal.h>
#include <unordered_map>
#include <nsresource.h>

class nsres_manager
{
public:

	nsres_manager();

	virtual ~nsres_manager();
	
	friend class nsmanager_factory;
	
	enum s_mode
	{
		binary,
		text
	};

	typedef std::unordered_map<uint32, nsresource*> map_type;

	virtual bool add(nsresource * res);

	map_type::iterator begin();

	template<class T>
	bool changed(const T & resource, const nsstring & fname="")
	{
		nsresource * res = get(resource);
		return changed(res,fname);
	}
	
	template<class T>
	bool contains(const T & res_name)
	{
		return contains(get(res_name));
	}

	template <class res_type>
	res_type * create(const nsstring & resName)
	{
		uint32 res_type_id = type_to_hash(res_type);
		return static_cast<res_type*>(create(res_type_id, resName));
	}

	template<class T>
	bool del(const T & res_name)
	{
		nsresource * res = get(res_name);
		return del(res);
	}

	template <class res_type, class T>
	res_type * get(const T & rname)
	{
		return static_cast<res_type*>(get(rname));
	}

	template<class res_type>
	res_type * load(const nsstring & fname)
	{
		uint32 res_type_id = type_to_hash(res_type);
		return static_cast<res_type*>(load(res_type_id, fname));
	}
	
	template<class res_type, class T >
	res_type * remove(const T & rname)
	{
		nsresource * res = get(rname);
		return static_cast<res_type*>(remove(res));
	}

	template<class T>
	bool save(const T & res_name, nsstring path="")
	{
		nsresource * res = get(res_name);
		return save(res, path);
	}

	template<class T>
	bool save_as(const T & resname, const nsstring & fname)
	{
		nsresource * res = get(resname);
		return save_as(res, fname);
	}

	virtual bool changed(nsresource * res, nsstring fname);

	virtual bool contains(nsresource * res);

	uint32 count() const;

	virtual nsresource * create(const nsstring & resName)=0;

	virtual nsresource * create(uint32 res_type_id, const nsstring & resName);

	nsresource * create(const nsstring & guid_, const nsstring & resName);

	virtual bool del(nsresource * res);

	map_type::iterator end();

	bool empty();

	uint32 type();

	virtual nsresource * get(uint32 resid);

	virtual nsresource * get(const nsstring & resName);

	virtual nsresource * get(nsresource * res);

	virtual const nsstring & res_dir();

	const nsstring & local_dir();

	uint32 plugin_id();

	virtual nsresource * load(const nsstring & fname) = 0;
	
	virtual nsresource * load(uint32 res_type_id, const nsstring & fname);
	
	nsresource * load(const nsstring & res_guid, const nsstring & fname);
	
	virtual void name_change(const uivec2 & oldid, const uivec2 newid);

	virtual nsresource * remove(uint32 res_type_id);

	virtual nsresource * remove(const nsstring & resName);
	
	virtual nsresource * remove(nsresource * res);

	virtual bool rename(const nsstring & oldName, const nsstring & newName);
	
	virtual bool save(nsresource * res,const nsstring & path);

	virtual void save_all(const nsstring & path="", nssave_resouces_callback * scallback = NULL);

	virtual bool save_as(nsresource * res, const nsstring & fname);

	void set_plugin_id(uint32 plugid);
	
	virtual void set_res_dir(const nsstring & pDirectory);

	void set_local_dir(const nsstring & pDirectory);

	s_mode save_mode() const;

	void set_save_mode(s_mode sm);

	virtual void destroy_all();

	template<class T>
	bool destroy(const T & resname)
	{
		nsresource * res = get(resname);
		return destroy(res);
	}
	
	virtual bool destroy(nsresource * res);

	static nsstring name_from_filename(const nsstring & fname);

	static nsstring path_from_filename(const nsstring & fname);
	
protected:
	uint32 m_hashed_type;
	nsstring m_res_dir;
	nsstring m_local_dir;
	map_type m_id_resmap;
	uint32 m_plugin_id;
	s_mode m_save_mode;
};

#endif
