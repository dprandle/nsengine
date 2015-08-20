#ifndef NSFACTORY_H
#define NSFACTORY_H

//#include <nsglobal.h>
#include <nstypes.h>

class nscomponent;
class nssystem;
class nsres_manager;
class nsresource;

class nsfactory
{
public:

	enum factory_t
	{
		f_component,
		f_system,
		f_resource,
		f_res_manager
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
	virtual nsresource * create() = 0;
	void set_id(nsresource * res_);	
	uint32 type_id;
};

template<class obj_type>
class nsres_factory_type : public nsres_factory
{
  public:
	nsres_factory_type() :
		nsres_factory()
	{}
	
	nsresource* create() {
		nsresource * res_ = new obj_type();
		set_id(res_);
		return res_;
	}
};

class nsmanager_factory : public nsfactory
{
public:
	nsmanager_factory() : nsfactory(f_res_manager) {}
	virtual nsres_manager * create() = 0;
	void set_id(nsres_manager * manager_);
	uint32 type_id;
};

template<class obj_type>
class nsmanager_factory_type : public nsmanager_factory
{
public:
	nsmanager_factory_type() :nsmanager_factory() {}
	nsres_manager * create() {
		nsres_manager * rm = new obj_type();
		set_id(rm);
		return rm;
	}
};

class nscomp_factory : public nsfactory
{
public:
	nscomp_factory() : nsfactory(f_component) {}
	virtual nscomponent * create() = 0;
	void set_id(nscomponent * cmp_);
	uint32 type_id;
};

template<class obj_type>
class nscomp_factory_type : public nscomp_factory
{
public:
	nscomp_factory_type() :nscomp_factory() {}
	nscomponent* create() {
		nscomponent * cmp_ = new obj_type();
		set_id(cmp_);
		return cmp_;
	}
};

class nssys_factory : public nsfactory
{
public:
	nssys_factory() : nsfactory(f_system) {}
	virtual nssystem * create() = 0;
	void set_id(nssystem * sys_);	
	uint32 type_id;
};

template<class obj_type>
class nssys_factory_type : public nssys_factory
{
public:
	nssys_factory_type() :nssys_factory() {}
	nssystem* create() {
		nssystem * sys_ = new obj_type();
		set_id(sys_);
		return sys_;
	}
};

#endif
