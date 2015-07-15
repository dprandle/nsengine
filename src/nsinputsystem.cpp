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
#include <nsinputmap.h>
#include <nsscene.h>

NSInputSystem::NSInputSystem() :NSSystem()
{

}

NSInputSystem::~NSInputSystem()
{

}


void NSInputSystem::keyPress(NSInputMap::Key pKey)
{
	// Search context at top of stack for trigger with key -
	// create an event for every key that has matching modifiers
	ContextStack::reverse_iterator rIter = mContextStack.rbegin();
	bool foundInContext = false;
	while (rIter != mContextStack.rend())
	{
		auto keyIter = (*rIter)->mKeyMap.equal_range(pKey);
		while (keyIter.first != keyIter.second)
		{
			// Send input event to event system if modifiers match the trigger modifiers
			if (_checkTriggerModifiers(keyIter.first->second))
			{
				nsengine.events()->push(new NSInputKeyEvent(keyIter.first->second.mName, NSInputKeyEvent::Pressed, mMLastPos));
				foundInContext = true;
			}
			++keyIter.first;
		}
		if (foundInContext) // If found a match for this key/modifier then search no further
			rIter = mContextStack.rend();
		else
			++rIter;
	}

	// Add the key to the modifier set (if not already there)..
	NSInputMap * inmap = nsengine.resource<NSInputMap>(mInputMapID);
	if (inmap->allowedModifier(pKey))
		mMods.insert(pKey);
}

void NSInputSystem::keyRelease(NSInputMap::Key pKey)
{
	// Remove the key from the modifier set..
	mMods.erase(pKey);

	// Check each context for pKey - create an event for each trigger. Since multiple triggers can
	// be assigned to each key sequence, I have to finish going through found triggers and set the iterator
	// to the rend
	ContextStack::reverse_iterator rIter = mContextStack.rbegin();
	bool foundInContext = false;
	while (rIter != mContextStack.rend())
	{
		auto keyIter = (*rIter)->mKeyMap.equal_range(pKey);
		while (keyIter.first != keyIter.second)
		{
			if (_checkTriggerModifiers(keyIter.first->second))
			{
				nsengine.events()->push(new NSInputKeyEvent(keyIter.first->second.mName, NSInputKeyEvent::Released, mMLastPos));
				foundInContext = true;
			}
			++keyIter.first;
		}
		if (foundInContext)
			return;
		++rIter;
	}
}

void NSInputSystem::setLastPos(const fvec2 & pLastPos)
{
	mMLastPos = pLastPos;
}

void NSInputSystem::mouseMove(nsfloat pPosX, nsfloat pPosY)
{
	nsfloat deltaX = mMLastPos.u - pPosX, deltaY = mMLastPos.v - pPosY;
	mMLastPos.u = pPosX; mMLastPos.v = pPosY;

	ContextStack::reverse_iterator rIter = mContextStack.rbegin();
	bool foundInContext = false;
	while (rIter != mContextStack.rend())
	{
		auto mouseIter = (*rIter)->mMouseButtonMap.equal_range(NSInputMap::Movement);
		while (mouseIter.first != mouseIter.second)
		{
			// Send input event to event system if modifiers match the trigger modifiers
			if (_checkTriggerModifiers(mouseIter.first->second))
			{
				nsengine.events()->push(new NSInputMouseMoveEvent(mouseIter.first->second.mName, fvec2(pPosX, pPosY), fvec2(deltaX, deltaY)));
				foundInContext = true;
			}
			++mouseIter.first;
		}
		if (foundInContext)
			return;
		++rIter;
	}
}

void NSInputSystem::mousePress(	NSInputMap::MouseButton pButton, nsfloat pPosX, nsfloat pPosY)
{
	mMLastPos.u = pPosX; mMLastPos.v = pPosY;
	// Go check each context for the key starting from the last context on the stack
	// if it is found there - then return
	ContextStack::reverse_iterator rIter = mContextStack.rbegin();
	bool foundInContext = false;
	while (rIter != mContextStack.rend())
	{
		auto mouseIter = (*rIter)->mMouseButtonMap.equal_range(pButton);
		while (mouseIter.first != mouseIter.second)
		{
			// Send input event to event system if modifiers match the trigger modifiers
			if (_checkTriggerModifiers(mouseIter.first->second))
			{
				nsengine.events()->push(new NSSelPickEvent(mouseIter.first->second.mName, fvec2(pPosX, pPosY)));
				nsengine.events()->push(new NSInputMouseButtonEvent(mouseIter.first->second.mName, NSInputMouseButtonEvent::Pressed, fvec2(pPosX, pPosY)));
				foundInContext = true;
			}
			++mouseIter.first;
		}
		if (foundInContext)
			rIter = mContextStack.rend();
		else
			++rIter;
	}

	mMouseMods.insert(pButton);
}

void NSInputSystem::mouseRelease(	NSInputMap::MouseButton pButton, nsfloat pPosX, nsfloat pPosY)
{
	mMouseMods.erase(pButton);

	// Go check each context for the key starting from the last context on the stack
	// if it is found there - then return
	ContextStack::reverse_iterator rIter = mContextStack.rbegin();
	bool foundInContext = false;
	while (rIter != mContextStack.rend())
	{
		auto mouseIter = (*rIter)->mMouseButtonMap.equal_range(pButton);
		while (mouseIter.first != mouseIter.second)
		{
			if (_checkTriggerModifiers(mouseIter.first->second))
			{
				// Send input event to event system if modifiers match the trigger modifiers
				nsengine.events()->push(new NSInputMouseButtonEvent(mouseIter.first->second.mName, NSInputMouseButtonEvent::Released, fvec2(pPosX, pPosY)));
				foundInContext = true;
			}
			++mouseIter.first;
		}
		if (foundInContext)
			return;
		++rIter;
	}
}

void NSInputSystem::mouseScroll(nsfloat pDelta, nsfloat pPosX, nsfloat pPosY)
{
	ContextStack::reverse_iterator rIter = mContextStack.rbegin();
	bool foundInContext = false;
	while (rIter != mContextStack.rend())
	{
		auto mouseIter = (*rIter)->mMouseButtonMap.equal_range(NSInputMap::Scrolling);
		while (mouseIter.first != mouseIter.second)
		{
			// Send input event to event system if modifiers match the trigger modifiers
			if (_checkTriggerModifiers(mouseIter.first->second))
			{
				nsengine.events()->push(new NSInputMouseScrollEvent(mouseIter.first->second.mName, fvec2(pPosX, pPosY), pDelta));
				foundInContext = true;
			}
			++mouseIter.first;
		}
		if (foundInContext)
			return;
		++rIter;
	}
}

void NSInputSystem::popContext()
{
	if (mContextStack.empty())
		return;
	mContextStack.pop_back();
}

void NSInputSystem::pushContext(const nsstring & pName)
{
	NSInputMap * inmap = nsengine.resource<NSInputMap>(mInputMapID);
	NSInputMap::Context * ctxt = inmap->context(pName);

	if (ctxt == NULL)
		return;
	
	mContextStack.push_back(ctxt);
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


void NSInputSystem::setInputMap(const uivec2 & resid)
{
	mInputMapID = resid;
}

const uivec2 & NSInputSystem::inputMap()
{
	return mInputMapID;
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

bool NSInputSystem::_checkTriggerModifiers(const NSInputMap::Trigger & t)
{
	NSInputMap::Key key1 = t.mKModifier1;
	NSInputMap::Key key2 = t.mKModifier2;
	NSInputMap::MouseButton mb1 = t.mMModifier1;
	NSInputMap::MouseButton mb2 = t.mMModifier2;
	NSInputMap * inmap = nsengine.resource<NSInputMap>(mInputMapID);
	
	// If modifier isnt allowed then return false
	if (!inmap->allowedModifier(key1) || !inmap->allowedModifier(key2))
		return false;

	// We cannot have more than two of either type of modifier
	if (mMods.size() > 2 || mMouseMods.size() > 2)
		return false;

	if (mMods.empty())
	{
		if (key1 != NSInputMap::NoButton || key2 != NSInputMap::NoButton)
			return false;
	}
	else if (mMods.size() == 2)
	{
		if (mMods.find(key1) == mMods.end() || mMods.find(key2) == mMods.end())
			return false;
	}
	else
	{
		if (mMods.find(key1) == mMods.end() && mMods.find(key2) == mMods.end())
			return false;
	}

	if (mMouseMods.empty())
	{
		if (t.mMModifier1 != NSInputMap::NoButton || t.mMModifier2 != NSInputMap::NoButton)
			return false;
	}
	else if (mMouseMods.size() == 2)
	{
		if (mMouseMods.find(t.mMModifier1) == mMouseMods.end() || mMouseMods.find(t.mMModifier2) == mMouseMods.end())
			return false;
	}
	else
	{
		if (mMouseMods.find(t.mMModifier1) == mMouseMods.end() && mMouseMods.find(t.mMModifier2) == mMouseMods.end())
			return false;
	}
	return true;
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
