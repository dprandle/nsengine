#ifndef NSFACTORY_H
#define NSFACTORY_H

//#include <nsglobal.h>
#include <nstypes.h>

class nscomponent;
class nssystem;
class nsres_manager;
class nsresource;
struct nsvid_obj;

class nsfactory
{
public:

	enum factory_t
	{
		f_component,
		f_system,
		f_resource,
		f_res_manager,
		f_vid_obj
	};

	nsfactory(factory_t fac_type_) : m_fac_type(fac_type_) {}

	factory_t type()
	{
		return m_fac_type;
	}

private:
	factory_t m_fac_type;
};

class nsres_factory : public nsfactory
{
	friend class nsengine;
  public:
	nsres_factory():
		nsfactory(f_resource)
	{}
	virtual nsresource * create(nsresource * copy_=nullptr) = 0;
};

template<class obj_type>
class nsres_factory_type : public nsres_factory
{
  public:
	nsres_factory_type() :
		nsres_factory()
	{}
	
	nsresource* create(nsresource * copy_)
	{
		nsresource * res_ = nullptr;
		if (copy_ == nullptr)
		{
			res_ = new obj_type();
		}
		else
		{
			obj_type * copy = dynamic_cast<obj_type*>(copy_);
			if (copy != nullptr)
				res_ = new obj_type(*copy);
		}
		return res_;
	}
};

class nsmanager_factory : public nsfactory
{
public:
	nsmanager_factory() : nsfactory(f_res_manager) {}
	virtual nsres_manager * create() = 0;
};

template<class obj_type>
class nsmanager_factory_type : public nsmanager_factory
{
public:

	nsmanager_factory_type() :nsmanager_factory() {}

	nsres_manager * create()
	{
		nsres_manager * rm = new obj_type();
		return rm;
	}
};

class nscomp_factory : public nsfactory
{
public:
	nscomp_factory() : nsfactory(f_component) {}
	virtual nscomponent * create(nscomponent * copy_ = nullptr) = 0;
};

template<class obj_type>
class nscomp_factory_type : public nscomp_factory
{
public:
	nscomp_factory_type() :nscomp_factory() {}
	nscomponent* create(nscomponent * copy_)
	{
		nscomponent * cmp_ = new obj_type();
		if (copy_ == nullptr)
		{
			cmp_ = new obj_type();
		}
		else
		{
			obj_type * copy = dynamic_cast<obj_type*>(copy_);
			if (copy != nullptr)
				cmp_ = new obj_type(*copy);
		}
		return cmp_;
	}
};

class nssys_factory : public nsfactory
{
public:
	nssys_factory() : nsfactory(f_system) {}
	virtual nssystem * create() = 0;
};

template<class obj_type>
class nssys_factory_type : public nssys_factory
{
public:

	nssys_factory_type() :nssys_factory() {}

	nssystem* create()
	{
		nssystem * sys_ = new obj_type();
		return sys_;
	}
};

class nsvideo_object;
class nsvid_obj_factory : public nsfactory
{
  public:

	nsvid_obj_factory():
		nsfactory(f_vid_obj)
	{}

	virtual nsvid_obj * create(nsvideo_object * parent) = 0;
};

template<class vid_obj_type>
class nsvid_obj_factory_type : public nsvid_obj_factory
{
  public:

	nsvid_obj_factory_type() :
		nsvid_obj_factory()
	{}
	
	vid_obj_type * create(nsvideo_object * parent)
	{
		return new vid_obj_type(parent);
	}
};

#endif
