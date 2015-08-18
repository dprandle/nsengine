/*!
\file nsinputsystem.cpp

\brief Definition file for NSInputSystem class

This file contains all of the neccessary definitions for the NSInputSystem class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#include <iostream>


#include <nsinput_system.h>
#include <nsevent.h>
#include <nsevent_dispatcher.h>
#include <nsinput_comp.h>
#include <nsinput_map.h>
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

void NSInputSystem::_keyPress(nsinput_map::key_val pKey)
{
	// Add the key to the modifier set (if not already there)..
	nsinput_map * inmap = nsengine.resource<nsinput_map>(mInputMapID);
	if (inmap == NULL)
		return;

	// Search context at top of stack for trigger with key -
	// create an event for every key that has matching modifiers
	ContextStack::reverse_iterator rIter = mContextStack.rbegin();
	bool foundInContext = false;
	while (rIter != mContextStack.rend())
	{
		auto keyIter = (*rIter)->key_map.equal_range(pKey);
		while (keyIter.first != keyIter.second)
		{
			// Send input event to event system if modifiers match the trigger modifiers
			if (_checkTriggerModifiers(keyIter.first->second))
			{
				if (keyIter.first->second.trigger_state == nsinput_map::t_toggle)
					_createStateEvent(keyIter.first->second, true);
				else if (keyIter.first->second.trigger_state != nsinput_map::t_released)
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

	if (inmap->allowed_mod(pKey))
		m_key_modifiers.insert(pKey);
}

void NSInputSystem::_createActionEvent(nsinput_map::trigger & trigger)
{
	// If the trigger is set to toggle then create a state event - otherwise create an action event
	NSActionEvent * evnt = nsengine.eventDispatch()->push<NSActionEvent>(trigger.hash_name);
	_setAxesFromTrigger(evnt->axes, trigger);
}

void NSInputSystem::_createStateEvent(nsinput_map::trigger & trigger, bool toggle)
{
	// If the trigger is set to toggle then create a state event - otherwise create an action event
	NSStateEvent * evnt = nsengine.eventDispatch()->push<NSStateEvent>(trigger.hash_name, toggle);
	_setAxesFromTrigger(evnt->axes, trigger);
}

void NSInputSystem::_keyRelease(nsinput_map::key_val pKey)
{
	nsinput_map * inmap = nsengine.resource<nsinput_map>(mInputMapID);
	if (inmap == NULL)
		return;

	// Remove the key from the modifier set..
	m_key_modifiers.erase(pKey);

	// Check each context for pKey - create an event for each trigger. Since multiple triggers can
	// be assigned to each key sequence, I have to finish going through found triggers and set the iterator
	// to the rend
	ContextStack::reverse_iterator rIter = mContextStack.rbegin();
	bool foundInContext = false;
	while (rIter != mContextStack.rend())
	{
		auto keyIter = (*rIter)->key_map.equal_range(pKey);
		while (keyIter.first != keyIter.second)
		{
			if (_checkTriggerModifiers(keyIter.first->second))
			{
				if (keyIter.first->second.trigger_state == nsinput_map::t_toggle)
					_createStateEvent(keyIter.first->second, false);
				else if (keyIter.first->second.trigger_state != nsinput_map::t_pressed)
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

void NSInputSystem::_setAxesFromTrigger(nsinput_map::axis_map & am, const nsinput_map::trigger & t)
{
	if ((t.axis_bitfield & nsinput_map::axis_mouse_xpos) == nsinput_map::axis_mouse_xpos)
		am[nsinput_map::axis_mouse_xpos] = mCurrentPos.x;
	if ((t.axis_bitfield & nsinput_map::axis_mouse_ypos) == nsinput_map::axis_mouse_ypos)
		am[nsinput_map::axis_mouse_ypos] = mCurrentPos.y;
	if ((t.axis_bitfield & nsinput_map::axis_mouse_xdelta) == nsinput_map::axis_mouse_xdelta)
		am[nsinput_map::axis_mouse_xdelta] = mCurrentPos.x - mLastPos.x;
	if ((t.axis_bitfield & nsinput_map::axis_mouse_ydelta) == nsinput_map::axis_mouse_ydelta)
		am[nsinput_map::axis_mouse_ydelta] = mCurrentPos.y - mLastPos.y;
	if ((t.axis_bitfield & nsinput_map::axis_scroll_delta) == nsinput_map::axis_scroll_delta)
		am[nsinput_map::axis_scroll_delta] = mScrollDelta;
}

void NSInputSystem::_mouseMove(const fvec2 & cursorPos)
{
	setCursorPos(cursorPos);

	nsinput_map * inmap = nsengine.resource<nsinput_map>(mInputMapID);
	if (inmap == NULL)
		return;

	ContextStack::reverse_iterator rIter = mContextStack.rbegin();
	bool foundInContext = false;
	while (rIter != mContextStack.rend())
	{
		auto mouseIter = (*rIter)->mousebutton_map.equal_range(nsinput_map::movement);
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

void NSInputSystem::_mousePress(nsinput_map::mouse_button_val pButton, const fvec2 & mousePos)
{
	setCursorPos(mousePos);

	nsinput_map * inmap = nsengine.resource<nsinput_map>(mInputMapID);
	if (inmap == NULL)
		return;

	// Go check each context for the key starting from the last context on the stack
	// if it is found there - then return
	ContextStack::reverse_iterator rIter = mContextStack.rbegin();
	bool foundInContext = false;
	while (rIter != mContextStack.rend())
	{
		auto mouseIter = (*rIter)->mousebutton_map.equal_range(pButton);
		while (mouseIter.first != mouseIter.second)
		{
			// Send input event to event system if modifiers match the trigger modifiers
			if (_checkTriggerModifiers(mouseIter.first->second))
			{
				if (mouseIter.first->second.trigger_state == nsinput_map::t_toggle)
					_createStateEvent(mouseIter.first->second, true);
				else if (mouseIter.first->second.trigger_state != nsinput_map::t_released)
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

	m_mouse_modifiers.insert(pButton);
}

void NSInputSystem::_mouseRelease(nsinput_map::mouse_button_val pButton, const fvec2 & mousePos)
{
	setCursorPos(mousePos);
	
	nsinput_map * inmap = nsengine.resource<nsinput_map>(mInputMapID);
	if (inmap == NULL)
		return;

	m_mouse_modifiers.erase(pButton);
	// Go check each context for the key starting from the last context on the stack
	// if it is found there - then return
	ContextStack::reverse_iterator rIter = mContextStack.rbegin();
	bool foundInContext = false;
	while (rIter != mContextStack.rend())
	{
		auto mouseIter = (*rIter)->mousebutton_map.equal_range(pButton);
		while (mouseIter.first != mouseIter.second)
		{
			if (_checkTriggerModifiers(mouseIter.first->second))
			{
				if (mouseIter.first->second.trigger_state == nsinput_map::t_toggle)
					_createStateEvent(mouseIter.first->second, false);
				else if (mouseIter.first->second.trigger_state != nsinput_map::t_pressed)
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

void NSInputSystem::_mouseScroll(float pDelta, const fvec2 & mousePos)
{
	setCursorPos(mousePos);
	mScrollDelta = pDelta;

	nsinput_map * inmap = nsengine.resource<nsinput_map>(mInputMapID);
	if (inmap == NULL)
		return;

	ContextStack::reverse_iterator rIter = mContextStack.rbegin();
	bool foundInContext = false;
	while (rIter != mContextStack.rend())
	{
		auto mouseIter = (*rIter)->mousebutton_map.equal_range(nsinput_map::scrolling);
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
	nsinput_map * inmap = nsengine.resource<nsinput_map>(mInputMapID);
	nsinput_map::ctxt * ctxt = inmap->context(pName);

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

int32 NSInputSystem::update_priority()
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
	nsscene * scene = nsengine.currentScene();

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

bool NSInputSystem::_checkTriggerModifiers(const nsinput_map::trigger & t)
{
	nsinput_map * inmap = nsengine.resource<nsinput_map>(mInputMapID);
	

	// If Key_Any is not part of key modifiers than there must be a key for key match
	if ( (t.key_modifiers.find(nsinput_map::key_any) == t.key_modifiers.end()) &&
		 t.key_modifiers.size() != m_key_modifiers.size())
		return false;

	// Same thing with mouse modifiers
	if ( (t.mouse_modifiers.find(nsinput_map::any_button) == t.mouse_modifiers.end()) &&
		 t.mouse_modifiers.size() != m_mouse_modifiers.size())
		return false;


	// Check key modifiers
	auto keyIter = t.key_modifiers.begin();
	while (keyIter != t.key_modifiers.end())
	{
		// If modifier isnt allowed or if it is not in current mod stack then return false
		if ((*keyIter != nsinput_map::key_any) &&
			(!inmap->allowed_mod(*keyIter) || m_key_modifiers.find(*keyIter) == m_key_modifiers.end()))
			return false;
		++keyIter;
	}

	// Check mouse modifiers
	auto mouseIter = t.mouse_modifiers.begin();
	while (mouseIter != t.mouse_modifiers.end())
	{
		// If modifier isnt allowed or if it is not in current mod stack then return false
		if (*mouseIter != nsinput_map::any_button &&
			m_mouse_modifiers.find(*mouseIter) == m_mouse_modifiers.end())
			return false;
		++mouseIter;
	}

	return true;
}

