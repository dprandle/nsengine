#ifndef NSFACTORY_H
#define NSFACTORY_H

#include <nsglobal.h>
class NSComponent;
class NSSystem;
class NSResManager;
class NSResource;

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
	virtual NSResource * create() = 0;
	void setid(NSResource * res);	
	nsuint type_id;
};

template<class T>
class NSResFactoryType : public NSResFactory
{
  public:
	NSResFactoryType() :
		NSResFactory()
	{}
	
	NSResource* create() {
		NSResource * res = new T();
		setid(res);
		return res;
	}
};

class NSResManagerFactory : public NSFactory
{
public:
	NSResManagerFactory() : NSFactory(ResManager) {}
	virtual NSResManager * create() = 0;
	void setid(NSResManager * manager);
	nsuint type_id;
};

template<class T>
class NSResManagerFactoryType : public NSResManagerFactory
{
public:
	NSResManagerFactoryType() :NSResManagerFactory() {}
	NSResManager * create() {
		NSResManager * rm = new T();
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
	nsuint type_id;
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
	nsuint type_id;
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
