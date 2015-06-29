#ifndef NSFACTORY_H
#define NSFACTORY_H

#include <nsglobal.h>
class NSComponent;
class NSSystem;
class NSResource;
class NSResManager;


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
public:
	NSResFactory() : NSFactory(Resource) {}
	virtual NSResource * create() = 0;
};

template<class T>
class NSResFactoryType : public NSResFactory
{
public:
	NSResFactoryType() :NSResFactory() {}
	NSResource* create() { return new T(); }
};

class NSResManagerFactory : public NSFactory
{
public:
	NSResManagerFactory() : NSFactory(ResManager) {}
	virtual NSResManager * create() = 0;
};

template<class T>
class NSResManagerFactoryType : public NSResManagerFactory
{
public:
	NSResManagerFactoryType() :NSResManagerFactory() {}
	NSResManager * create() { return new T(); }
};

class NSCompFactory : public NSFactory
{
public:
	NSCompFactory() : NSFactory(Component) {}
	virtual NSComponent * create() = 0;
};

template<class T>
class NSCompFactoryType : public NSCompFactory
{
public:
	NSCompFactoryType() :NSCompFactory() {}
	NSComponent* create() { return new T(); }
};

class NSSysFactory : public NSFactory
{
public:
	NSSysFactory() : NSFactory(System) {}
	virtual NSSystem * create() = 0;
};

template<class T>
class NSSysFactoryType : public NSSysFactory
{
public:
	NSSysFactoryType() :NSSysFactory() {}
	NSSystem* create() { return new T(); }
};

#endif