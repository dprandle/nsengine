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


void NSInputSystem::keyPress(Key pKey)
{
	// Go check each context for the key starting from the last context on the stack
	// if it is found there - then return
	ContextStack::reverse_iterator rIter = mContextStack.rbegin();
	while (rIter != mContextStack.rend())
	{
		KeyMap::iterator keyIter = (*rIter)->mKeyMap.find(pKey);
		while (keyIter != (*rIter)->mKeyMap.end() && keyIter->first == pKey)
		{
			// Send input event to event system if modifiers match the trigger modifiers
			if (_checkKeyTriggerModifiers(keyIter->second))
			{
				nsengine.events()->push(new NSInputKeyEvent(keyIter->second.mName, NSInputKeyEvent::Pressed, mMLastPos));

				// Add the key to the modifier set (if not already there)..
				if (mAllowedModifiers.find(pKey) != mAllowedModifiers.end())
					mMods.insert(pKey);

				return;
			}
			++keyIter;
		}
		++rIter;
	}

	// Add the key to the modifier set (if not already there)..
	if (mAllowedModifiers.find(pKey) != mAllowedModifiers.end())
		mMods.insert(pKey);
}

void NSInputSystem::keyRelease(Key pKey)
{
	// Remove the key from the modifier set..
	if (mAllowedModifiers.find(pKey) != mAllowedModifiers.end() && mMods.find(pKey) != mMods.end())
		mMods.erase(pKey);

	// Go check each context for the key starting from the last context on the stack
	// if it is found there - then send a released signal and return return
	ContextStack::reverse_iterator rIter = mContextStack.rbegin();
	while (rIter != mContextStack.rend())
	{
		KeyMap::iterator keyIter = (*rIter)->mKeyMap.find(pKey);
		while (keyIter != (*rIter)->mKeyMap.end() && keyIter->first == pKey)
		{
			if (_checkKeyTriggerModifiers(keyIter->second))
			{
				nsengine.events()->push(new NSInputKeyEvent(keyIter->second.mName, NSInputKeyEvent::Released, mMLastPos));
				return;
			}
			++keyIter;
		}
		++rIter;
	}
}

void NSInputSystem::mouseMove(nsfloat pPosX, nsfloat pPosY)
{
	nsfloat deltaX = mMLastPos.u - pPosX, deltaY = mMLastPos.v - pPosY;
	mMLastPos.u = pPosX; mMLastPos.v = pPosY;

	ContextStack::reverse_iterator rIter = mContextStack.rbegin();
	while (rIter != mContextStack.rend())
	{
		MouseButtonMap::iterator mouseIter = (*rIter)->mMouseButtonMap.find(Movement);
		while (mouseIter != (*rIter)->mMouseButtonMap.end() && mouseIter->first == Movement)
		{
			// Send input event to event system if modifiers match the trigger modifiers
			if (_checkMouseTriggerModifiers(mouseIter->second))
			{
				nsengine.events()->push(new NSInputMouseMoveEvent(mouseIter->second.mName, fvec2(pPosX, pPosY), fvec2(deltaX, deltaY)));
				return;
			}

			++mouseIter;
		}
		++rIter;
	}
}

void NSInputSystem::mousePress(MouseButton pButton, nsfloat pPosX, nsfloat pPosY)
{
	mMLastPos.u = pPosX; mMLastPos.v = pPosY;
	// Go check each context for the key starting from the last context on the stack
	// if it is found there - then return
	ContextStack::reverse_iterator rIter = mContextStack.rbegin();
	while (rIter != mContextStack.rend())
	{
		MouseButtonMap::iterator mouseIter = (*rIter)->mMouseButtonMap.find(pButton);
		while (mouseIter != (*rIter)->mMouseButtonMap.end() && mouseIter->first == pButton)
		{
			// Send input event to event system if modifiers match the trigger modifiers
			if (_checkMouseTriggerModifiers(mouseIter->second))
			{
				nsengine.events()->push(new NSSelPickEvent(mouseIter->second.mName, fvec2(pPosX, pPosY)));
				nsengine.events()->push(new NSInputMouseButtonEvent(mouseIter->second.mName, NSInputMouseButtonEvent::Pressed, fvec2(pPosX, pPosY)));

				// Add the key to the modifier set (if not already there)..
				mMouseMods.insert(pButton);

				return;
			}
			++mouseIter;
		}
		++rIter;
	}

	mMouseMods.insert(pButton);
}

void NSInputSystem::mouseRelease(MouseButton pButton, nsfloat pPosX, nsfloat pPosY)
{
	// Add the key to the modifier set (if not already there)..
	if (mMouseMods.find(pButton) != mMouseMods.end())
		mMouseMods.erase(pButton);

	// Go check each context for the key starting from the last context on the stack
	// if it is found there - then return
	ContextStack::reverse_iterator rIter = mContextStack.rbegin();
	while (rIter != mContextStack.rend())
	{
		MouseButtonMap::iterator mouseIter = (*rIter)->mMouseButtonMap.find(pButton);
		while (mouseIter != (*rIter)->mMouseButtonMap.end() && mouseIter->first == pButton)
		{
			if (_checkMouseTriggerModifiers(mouseIter->second))
			{
				// Send input event to event system if modifiers match the trigger modifiers
				nsengine.events()->push(new NSInputMouseButtonEvent(mouseIter->second.mName, NSInputMouseButtonEvent::Released, fvec2(pPosX, pPosY)));
				return;
			}
			++mouseIter;
		}
		++rIter;
	}
}

void NSInputSystem::mouseScroll(nsfloat pDelta, nsfloat pPosX, nsfloat pPosY)
{
	ContextStack::reverse_iterator rIter = mContextStack.rbegin();
	while (rIter != mContextStack.rend())
	{
		MouseButtonMap::iterator mouseIter = (*rIter)->mMouseButtonMap.find(Scrolling);
		while (mouseIter != (*rIter)->mMouseButtonMap.end() && mouseIter->first == Scrolling)
		{
			// Send input event to event system if modifiers match the trigger modifiers
			if (_checkMouseTriggerModifiers(mouseIter->second))
			{
				nsengine.events()->push(new NSInputMouseScrollEvent(mouseIter->second.mName, fvec2(pPosX, pPosY), pDelta));
				return;
			}

			++mouseIter;
		}
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
	ContextCollection::iterator conIter = mContexts.find(pName);
	if (conIter == mContexts.end())
	{
		dprint("NSInputSystem::pushContext Cannot find context with name " + pName);
		return;
	}
	mContextStack.push_back(conIter->second);
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


bool NSInputSystem::_checkKeyTriggerModifiers(const Trigger & t)
{
	Key key1 = t.mKModifier1;
	Key key2 = t.mKModifier2;
	MouseButton mb1 = t.mMModifier1;
	MouseButton mb2 = t.mMModifier2;

	if (mAllowedModifiers.find(key1) == mAllowedModifiers.end())
		key1 = Key_None;
	if (mAllowedModifiers.find(key2) == mAllowedModifiers.end())
		key2 = Key_None;

	// We cannot have more than two of either type of modifier
	if (mMods.size() > 2 || mMouseMods.size() > 2)
		return false;

	if (mMods.empty())
	{
		if (key1 != Key_None || key2 != Key_None)
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

	if (t.mMModifier1 != NoButton || t.mMModifier2 != NoButton)
	{
		if (mMouseMods.empty())
				return false;
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
	}

	return true;
}

bool NSInputSystem::_checkMouseTriggerModifiers(const Trigger & t)
{
	Key key1 = t.mKModifier1;
	Key key2 = t.mKModifier2;
	MouseButton mb1 = t.mMModifier1;
	MouseButton mb2 = t.mMModifier2;

	if (mAllowedModifiers.find(key1) == mAllowedModifiers.end())
		key1 = Key_None;
	if (mAllowedModifiers.find(key2) == mAllowedModifiers.end())
		key2 = Key_None;

	// We cannot have more than two of either type of modifier
	if (mMods.size() > 2 || mMouseMods.size() > 2)
		return false;

	if (mMods.empty())
	{
		if (key1 != NoButton || key2 != NoButton)
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
		if (t.mMModifier1 != NoButton || t.mMModifier2 != NoButton)
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
