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
#include <nseventdispatcher.h>
#include <nsinputcomp.h>
#include <nsinputmap.h>
#include <nsscene.h>

NSInputSystem::NSInputSystem() :
	mScrollDelta(0.0f),
	NSSystem()
{

}

NSInputSystem::~NSInputSystem()
{
 
}

bool NSInputSystem::keyEvent(NSKeyEvent * evnt)
{
	if (evnt->mPressed)
		_keyPress(evnt->mKey);
	else
		_keyRelease(evnt->mKey);
	return true;
}

bool NSInputSystem::mouseButtonEvent(NSMouseButtonEvent * evnt)
{
	if (evnt->mPressed)
		_mousePress(evnt->mb, evnt->mNormMousePos);
	else
		_mouseRelease(evnt->mb, evnt->mNormMousePos);
	return true;
}

bool NSInputSystem::mouseMoveEvent(NSMouseMoveEvent * evnt)
{
	return false;
}

bool NSInputSystem::mouseScrollEvent(NSMouseScrollEvent * evnt)
{
	return false;
}

void NSInputSystem::_keyPress(NSInputMap::Key pKey)
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

void NSInputSystem::_keyRelease(NSInputMap::Key pKey)
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
				
				foundInContext = true;
			}
			++keyIter.first;
		}
		if (foundInContext)
			return;
		++rIter;
	}
}

void NSInputSystem::setCursorPos(const fvec2 & cursorPos)
{
	mLastPos = mCurrentPos;
	mCurrentPos = cursorPos;
}

void NSInputSystem::_setAxesFromTrigger(NSInputMap::AxisMap & am, const NSInputMap::Trigger & t)
{
	if ((t.mAxes & NSInputMap::MouseXPos) == NSInputMap::MouseXPos)
		am[NSInputMap::MouseXPos] = mCurrentPos.x;
	if ((t.mAxes & NSInputMap::MouseYPos) == NSInputMap::MouseYPos)
		am[NSInputMap::MouseYPos] = mCurrentPos.y;
	if ((t.mAxes & NSInputMap::MouseXDelta) == NSInputMap::MouseXDelta)
		am[NSInputMap::MouseXDelta] = mCurrentPos.x - mLastPos.x;
	if ((t.mAxes & NSInputMap::MouseYDelta) == NSInputMap::MouseYDelta)
		am[NSInputMap::MouseYDelta] = mCurrentPos.y - mLastPos.y;
	if ((t.mAxes & NSInputMap::ScrollDelta) == NSInputMap::ScrollDelta)
		am[NSInputMap::ScrollDelta] = mScrollDelta;	
}

void NSInputSystem::_mouseMove(const fvec2 & cursorPos)
{
	setCursorPos(cursorPos);
	
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
				NSActionEvent * av = nsengine.eventDispatch()->push<NSActionEvent>(mouseIter.first->second.mName);
				if (av != NULL)
					_setAxesFromTrigger(av->axes, mouseIter.first->second);
				foundInContext = true;
			}
			++mouseIter.first;
		}
		if (foundInContext)
			return;
		++rIter;
	}
}

void NSInputSystem::_mousePress(NSInputMap::MouseButton pButton, const fvec2 & mousePos)
{
	setCursorPos(mousePos);
	
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

void NSInputSystem::_mouseRelease(NSInputMap::MouseButton pButton, const fvec2 & mousePos)
{
	setCursorPos(mousePos);
	
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
				
				foundInContext = true;
			}
			++mouseIter.first;
		}
		if (foundInContext)
			return;
		++rIter;
	}
}

void NSInputSystem::_mouseScroll(nsfloat pDelta, const fvec2 & mousePos)
{
	setCursorPos(mousePos);
	
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
	nsengine.eventDispatch()->registerListener<NSKeyEvent>(this);
	nsengine.eventDispatch()->registerListener<NSMouseButtonEvent>(this);
	nsengine.eventDispatch()->registerListener<NSMouseMoveEvent>(this);
	nsengine.eventDispatch()->registerListener<NSMouseScrollEvent>(this);
    registerHandlerFunc(this, &NSInputSystem::keyEvent);
}

nsint NSInputSystem::updatePriority()
{
	return INP_SYS_UPDATE_PR;
}

void NSInputSystem::setInputMap(const uivec2 & resid)
{
	mInputMapID = resid;
}

const uivec2 & NSInputSystem::inputMap()
{
	return mInputMapID;
}

void NSInputSystem::update()
{
	NSScene * scene = nsengine.currentScene();

	if (scene == NULL)
	{
		nsengine.eventDispatch()->process(this);
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
	nsengine.eventDispatch()->process(this);
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

