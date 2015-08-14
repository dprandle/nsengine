/*!
\file nsinputsystem.cpp

\brief Definition file for NSInputSystem class

This file contains all of the neccessary definitions for the NSInputSystem class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#include <iostream>


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
#ifdef INPUTTEST
	std::cout << "Key was pressed: " << evnt->mPressed << " and key was: " << evnt->mKey << std::endl;
#endif
	if (evnt->mPressed)
		_keyPress(evnt->mKey);
	else
		_keyRelease(evnt->mKey);
	return true;
}

bool NSInputSystem::mouseButtonEvent(NSMouseButtonEvent * evnt)
{
#ifdef INPUTTEST
	std::cout << "Mouse was pressed: " << evnt->mPressed << " and key was: " << evnt->mb << std::endl;
	std::cout << "Mouse coords:" << evnt->mNormMousePos.toString() << std::endl;
#endif
	if (evnt->mPressed)
		_mousePress(evnt->mb, evnt->mNormMousePos);
	else
		_mouseRelease(evnt->mb, evnt->mNormMousePos);
	return true;
}

bool NSInputSystem::mouseMoveEvent(NSMouseMoveEvent * evnt)
{
#ifdef INPUTTEST
	std::cout << "Mouse moved: Mouse coords:" << evnt->mNormMousePos.toString() << std::endl;
#endif
	_mouseMove(evnt->mNormMousePos);
	return true;
}

bool NSInputSystem::mouseScrollEvent(NSMouseScrollEvent * evnt)
{
#ifdef INPUTTEST
	std::cout << "Mouse Scrolled: Mouse coords:" << evnt->mNormMousePos.toString() << " Scroll amount: " << evnt->mScroll << std::endl;
#endif
	_mouseScroll(evnt->mScroll, evnt->mNormMousePos);
	return true;
}

void NSInputSystem::_keyPress(NSInputMap::Key pKey)
{
	// Add the key to the modifier set (if not already there)..
	NSInputMap * inmap = nsengine.resource<NSInputMap>(mInputMapID);
	if (inmap == NULL)
		return;

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
				if (keyIter.first->second.mTriggerOn == NSInputMap::Toggle)
					_createStateEvent(keyIter.first->second, true);
				else if (keyIter.first->second.mTriggerOn != NSInputMap::Released)
					_createActionEvent(keyIter.first->second);
				
				foundInContext = true;
			}
			++keyIter.first;
		}
		if (foundInContext) // If found a match for this key/modifier then search no further
			rIter = mContextStack.rend();
		else
			++rIter;
	}

	if (inmap->allowedModifier(pKey))
		mKeyMods.insert(pKey);
}

void NSInputSystem::_createActionEvent(NSInputMap::Trigger & trigger)
{
	// If the trigger is set to toggle then create a state event - otherwise create an action event
	NSActionEvent * evnt = nsengine.eventDispatch()->push<NSActionEvent>(trigger.mHashName);
	_setAxesFromTrigger(evnt->axes, trigger);
}

void NSInputSystem::_createStateEvent(NSInputMap::Trigger & trigger, bool toggle)
{
	// If the trigger is set to toggle then create a state event - otherwise create an action event
	NSStateEvent * evnt = nsengine.eventDispatch()->push<NSStateEvent>(trigger.mHashName, toggle);
	_setAxesFromTrigger(evnt->axes, trigger);
}

void NSInputSystem::_keyRelease(NSInputMap::Key pKey)
{
	NSInputMap * inmap = nsengine.resource<NSInputMap>(mInputMapID);
	if (inmap == NULL)
		return;

	// Remove the key from the modifier set..
	mKeyMods.erase(pKey);

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
				if (keyIter.first->second.mTriggerOn == NSInputMap::Toggle)
					_createStateEvent(keyIter.first->second, false);
				else if (keyIter.first->second.mTriggerOn != NSInputMap::Pressed)
					_createActionEvent(keyIter.first->second);
				
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

	NSInputMap * inmap = nsengine.resource<NSInputMap>(mInputMapID);
	if (inmap == NULL)
		return;

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
				_createActionEvent(mouseIter.first->second);
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

	NSInputMap * inmap = nsengine.resource<NSInputMap>(mInputMapID);
	if (inmap == NULL)
		return;

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
				if (mouseIter.first->second.mTriggerOn == NSInputMap::Toggle)
					_createStateEvent(mouseIter.first->second, true);
				else if (mouseIter.first->second.mTriggerOn != NSInputMap::Released)
					_createActionEvent(mouseIter.first->second);

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
	
	NSInputMap * inmap = nsengine.resource<NSInputMap>(mInputMapID);
	if (inmap == NULL)
		return;

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
				if (mouseIter.first->second.mTriggerOn == NSInputMap::Toggle)
					_createStateEvent(mouseIter.first->second, false);
				else if (mouseIter.first->second.mTriggerOn != NSInputMap::Pressed)
					_createActionEvent(mouseIter.first->second);

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
	mScrollDelta = pDelta;

	NSInputMap * inmap = nsengine.resource<NSInputMap>(mInputMapID);
	if (inmap == NULL)
		return;

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
				_createActionEvent(mouseIter.first->second);
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
    registerHandlerFunc(this, &NSInputSystem::keyEvent);
    registerHandlerFunc(this, &NSInputSystem::mouseButtonEvent);
    registerHandlerFunc(this, &NSInputSystem::mouseScrollEvent);
    registerHandlerFunc(this, &NSInputSystem::mouseMoveEvent);
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
		return;

	auto ents = scene->entities<NSInputComp>();
	auto entIter = ents.begin();

	// deactivate all actions
	while (entIter != ents.end())
	{
		NSInputComp * inComp = (*entIter)->get<NSInputComp>();
		inComp->setActivated(false);
		++entIter;
	}
}

bool NSInputSystem::_checkTriggerModifiers(const NSInputMap::Trigger & t)
{
	NSInputMap * inmap = nsengine.resource<NSInputMap>(mInputMapID);
	

	// If Key_Any is not part of key modifiers than there must be a key for key match
	if ( (t.mKeyMods.find(NSInputMap::Key_Any) == t.mKeyMods.end()) &&
		 t.mKeyMods.size() != mKeyMods.size())
		return false;

	// Same thing with mouse modifiers
	if ( (t.mMouseMods.find(NSInputMap::AnyButton) == t.mMouseMods.end()) &&
		 t.mMouseMods.size() != mMouseMods.size())
		return false;


	// Check key modifiers
	auto keyIter = t.mKeyMods.begin();
	while (keyIter != t.mKeyMods.end())
	{
		// If modifier isnt allowed or if it is not in current mod stack then return false
		if ((*keyIter != NSInputMap::Key_Any) &&
			(!inmap->allowedModifier(*keyIter) || mKeyMods.find(*keyIter) == mKeyMods.end()))
			return false;
		++keyIter;
	}

	// Check mouse modifiers
	auto mouseIter = t.mMouseMods.begin();
	while (mouseIter != t.mMouseMods.end())
	{
		// If modifier isnt allowed or if it is not in current mod stack then return false
		if (*mouseIter != NSInputMap::AnyButton &&
			mMouseMods.find(*mouseIter) == mMouseMods.end())
			return false;
		++mouseIter;
	}

	return true;
}

