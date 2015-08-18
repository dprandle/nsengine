#ifndef NSFACTORY_H
#define NSFACTORY_H

#include <nsglobal.h>
class NSComponent;
class NSSystem;
class nsres_manager;
class nsresource;

class NSFactory
{
public:

	enum FactoryType
	{
		Component,
		System,
		Resource,
		ResManager
	};

	NSFactory(FactoryType t) : mType(t) {}

	FactoryType type()
	{
		return mType;
	}

private:
	FactoryType mType;
};

class NSResFactory : public NSFactory
{
	friend class NSEngine;
  public:
	NSResFactory():
		NSFactory(Resource)
	{}
	virtual nsresource * create() = 0;
	void setid(nsresource * res);	
	uint32 type_id;
};

template<class T>
class NSResFactoryType : public NSResFactory
{
  public:
	NSResFactoryType() :
		NSResFactory()
	{}
	
	nsresource* create() {
		nsresource * res = new T();
		setid(res);
		return res;
	}
};

class nsres_managerFactory : public NSFactory
{
public:
	nsres_managerFactory() : NSFactory(ResManager) {}
	virtual nsres_manager * create() = 0;
	void setid(nsres_manager * manager);
	uint32 type_id;
};

template<class T>
class nsres_managerFactoryType : public nsres_managerFactory
{
public:
	nsres_managerFactoryType() :nsres_managerFactory() {}
	nsres_manager * create() {
		nsres_manager * rm = new T();
		setid(rm);
		return rm;
	}
};

class NSCompFactory : public NSFactory
{
public:
	NSCompFactory() : NSFactory(Component) {}
	virtual NSComponent * create() = 0;
	void setid(NSComponent * comp);
	uint32 type_id;
};

template<class T>
class NSCompFactoryType : public NSCompFactory
{
public:
	NSCompFactoryType() :NSCompFactory() {}
	NSComponent* create() {
		NSComponent * cmp = new T();
		setid(cmp);
		return cmp;
	}
};

class NSSysFactory : public NSFactory
{
public:
	NSSysFactory() : NSFactory(System) {}
	virtual NSSystem * create() = 0;
	void setid(NSSystem * sys);	
	uint32 type_id;
};

template<class T>
class NSSysFactoryType : public NSSysFactory
{
public:
	NSSysFactoryType() :NSSysFactory() {}
	NSSystem* create() {
		NSSystem * sys = new T();
		setid(sys);
		return sys;
	}
};

#endif
