/*!
\file nsinputsystem.cpp

\brief Definition file for NSInputSystem class

This file contains all of the neccessary definitions for the NSInputSystem class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#include <nsinputsystem.h>
#include <nsevent.h>
#include <nseventhandler.h>
#include <nsinputcomp.h>
#include <nsscene.h>

NSInputSystem::NSInputSystem() :NSSystem()
{

}

NSInputSystem::~NSInputSystem()
{

}

void NSInputSystem::init()
{
	nsengine.events()->addListener(this, NSEvent::InputKey);
	nsengine.events()->addListener(this, NSEvent::InputMouseButton);
	nsengine.events()->addListener(this, NSEvent::InputMouseMove);
	nsengine.events()->addListener(this, NSEvent::InputMouseScroll);
}

float NSInputSystem::updatePriority()
{
	return INP_SYS_UPDATE_PR;
}

nsstring NSInputSystem::getTypeString()
{
	return INP_SYS_TYPESTRING;
}

bool NSInputSystem::handleEvent(NSEvent * pEvent)
{
	NSScene * scene = nsengine.currentScene();
	if (scene == NULL)
		return false;

	switch (pEvent->mID)
	{
	case (NSEvent::InputKey) :
		_eventKey((NSInputKeyEvent*)pEvent);
		return true;
	case (NSEvent::InputMouseButton) :
		_eventMouseButton((NSInputMouseButtonEvent*)pEvent);
		return true;
	case (NSEvent::InputMouseMove) :
		_eventMouseMove((NSInputMouseMoveEvent*)pEvent);
		return true;
	case (NSEvent::InputMouseScroll) :
		_eventMouseScroll((NSInputMouseScrollEvent*)pEvent);
		return true;
	}
	return false;
}

void NSInputSystem::update()
{
	NSScene * scene = nsengine.currentScene();
	if (scene == NULL)
		return;

	if (scene == NULL)
	{
		nsengine.events()->process(this);
		return;
	}

	auto ents = scene->entities<NSInputComp>();
	auto entIter = ents.begin();

	// deactivate all actions
	while (entIter != ents.end())
	{
		NSInputComp * inComp = (*entIter)->get<NSInputComp>();
		inComp->setActivated(false);
		++entIter;
	}
	// activate ones recieved
	nsengine.events()->process(this);
}

void NSInputSystem::_eventKey(NSInputKeyEvent * pEvent)
{
	NSScene * scene = nsengine.currentScene();
	if (scene == NULL)
		return;

	auto ents = scene->entities<NSInputComp>();
	auto entIter = ents.begin();
	while (entIter != ents.end())
	{
		NSInputComp * inComp = (*entIter)->get<NSInputComp>();
		NSInputComp::Action * action = inComp->action(pEvent->mName);
		if (action != NULL)
		{
			action->mPressed = pEvent->mPorR && 1;
			action->mActivated = true;
			action->mPos = pEvent->mMousePos;
		}
		++entIter;
	}
}

void NSInputSystem::_eventMouseButton(NSInputMouseButtonEvent * pEvent)
{
	NSScene * scene = nsengine.currentScene();
	if (scene == NULL)
		return;

	auto ents = scene->entities<NSInputComp>();
	auto entIter = ents.begin();
	while (entIter != ents.end())
	{
		NSInputComp * inComp = (*entIter)->get<NSInputComp>();
		NSInputComp::Action * action = inComp->action(pEvent->mName);
		if (action != NULL)
		{
			action->mPressed = pEvent->mPorR && 1;
			action->mPos = pEvent->mPos;
			action->mActivated = true;
		}
		++entIter;
	}
}

void NSInputSystem::_eventMouseMove(NSInputMouseMoveEvent * pEvent)
{
	NSScene * scene = nsengine.currentScene();
	if (scene == NULL)
		return;

	auto ents = scene->entities<NSInputComp>();
	auto entIter = ents.begin();
	while (entIter != ents.end())
	{
		NSInputComp * inComp = (*entIter)->get<NSInputComp>();
		NSInputComp::Action * action = inComp->action(pEvent->mName);
		if (action != NULL)
		{
			action->mPos = pEvent->mPos;
			action->mDelta = pEvent->mDelta;
			action->mActivated = true;
		}
		++entIter;
	}
}

void NSInputSystem::_eventMouseScroll(NSInputMouseScrollEvent * pEvent)
{
	NSScene * scene = nsengine.currentScene();
	if (scene == NULL)
		return;

	auto ents = scene->entities<NSInputComp>();
	auto entIter = ents.begin();
	while (entIter != ents.end())
	{
		NSInputComp * inComp = (*entIter)->get<NSInputComp>();
		if (inComp->contains(pEvent->mName))
		{
			NSInputComp::Action * action = inComp->action(pEvent->mName);
			if (action != NULL)
			{
				action->mPos = pEvent->mPos;
				action->mScroll = pEvent->mScroll;
				action->mActivated = true;
			}
		}
		++entIter;
	}
}