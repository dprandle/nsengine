/*!
\file nsinputmanager.cpp

\brief Definition file for NSInputManager class

This file contains all of the neccessary definitions for the NSInputManager class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#include <nsinputmanager.h>
#include <nseventhandler.h>
#include <nsevent.h>
#include <nsengine.h>
#include <nsrendersystem.h>

NSInputManager::NSInputManager() :
mContextStack(),
mContexts(),
mMods(),
mMouseMods(),
mDirectory(LOCAL_INPUT_DIR_DEFAULT)
{
}

NSInputManager::~NSInputManager()
{
	mContextStack.clear();
	ContextCollection::iterator iter = mContexts.begin();
	while (iter != mContexts.end())
	{
		delete iter->second;
		iter->second = NULL;
		iter = mContexts.erase(iter);
	}
}

void NSInputManager::addAllowedModifier(Key pKey)
{
	mAllowedModifiers.insert(pKey);
}

// In adding the context InputManager takes ownership
bool NSInputManager::addContext(Context * toAdd)
{
	if (toAdd == NULL)
	{
		dprint("NSInputManager::addContext Trying to add NULL context");
		return false;
	}
	if (mContexts.find(toAdd->mName) != mContexts.end())
	{
		dprint("NSInputManager::addContext Cannot add context with same name " + toAdd->mName);
		return false;
	}
	mContexts[toAdd->mName] = toAdd;
	return true;
}

bool NSInputManager::addKeyTrigger(const nsstring & pContextName, Key pKey, const Trigger & pTrigger)
{
	ContextCollection::iterator conIter = mContexts.find(pContextName);
	if (conIter == mContexts.end())
	{
		dprint("NSInputManager::addKeyTrigger Cannot find context with name " + pContextName);
		return false;
	}

	// Allow for multiple trigger entrees, but all of the triggers must be unique - they
	// can have the same name but if they do then the key modifiers must be different, for example.
	auto iterPair = conIter->second->mKeyMap.equal_range(pKey);
	auto iterStart = iterPair.first;
	while (iterStart != iterPair.second)
	{
		if (iterStart->second == pTrigger)
		{
			dprint("NSInputManager::addKeyTrigger Cannot add trigger \"" + pTrigger.mName + "\" as an exact copy already exists in context");
			return false;
		}
		++iterStart;
	}

	conIter->second->mKeyMap.emplace(pKey,pTrigger);
	return true;
}

bool NSInputManager::addMouseTrigger(const nsstring & pContextName, MouseButton pButton, const Trigger & pTrigger)
{
	ContextCollection::iterator conIter = mContexts.find(pContextName);
	if (conIter == mContexts.end())
	{
		dprint("NSInputManager::addMouseTrigger Cannot find context with name " + pContextName);
		return false;
	}

	// Allow for multiple trigger entrees, but all of the triggers must be unique - they
	// can have the same name but if they do then the key modifiers must be different, for example.
	auto iterPair = conIter->second->mMouseButtonMap.equal_range(pButton);
	auto iterStart = iterPair.first;
	while (iterStart != iterPair.second)
	{
		if (iterStart->second == pTrigger)
		{
			dprint("NSInputManager::addMouseTrigger Cannot add trigger \"" + pTrigger.mName + "\" as an exact copy already exists in context");
			return false;
		}
		++iterStart;
	}

	conIter->second->mMouseButtonMap.emplace(pButton,pTrigger);
	return true;
}

NSInputManager::Context * NSInputManager::createContext(const nsstring & pName)
{
	Context * toAdd = new Context();
	toAdd->mName = pName;
	if (!addContext(toAdd))
	{
		delete toAdd;
		return NULL;
	}
	return toAdd;
}

const nsstring & NSInputManager::directory() const
{
	return mDirectory;
}

void NSInputManager::keyPress(Key pKey)
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

void NSInputManager::keyRelease(Key pKey)
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

void NSInputManager::mouseMove(nsfloat pPosX, nsfloat pPosY)
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

void NSInputManager::mousePress(MouseButton pButton, nsfloat pPosX, nsfloat pPosY)
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

void NSInputManager::mouseRelease(MouseButton pButton, nsfloat pPosX, nsfloat pPosY)
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

void NSInputManager::mouseScroll(nsfloat pDelta, nsfloat pPosX, nsfloat pPosY)
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

void NSInputManager::popContext()
{
	if (mContextStack.empty())
		return;
	mContextStack.pop_back();
}

void NSInputManager::pushContext(const nsstring & pName)
{
	ContextCollection::iterator conIter = mContexts.find(pName);
	if (conIter == mContexts.end())
	{
		dprint("NSInputManager::pushContext Cannot find context with name " + pName);
		return;
	}
	mContextStack.push_back(conIter->second);
}

void NSInputManager::removeAllowedModifier(Key pKey)
{
	mAllowedModifiers.erase(pKey);
}

bool NSInputManager::removeContext(const nsstring & pName)
{
	ContextCollection::iterator conIter = mContexts.find(pName);
	if (conIter == mContexts.end())
	{
		dprint("NSInputManager::pushContext Cannot find context with name " + pName);
		return false;
	}
	delete conIter->second;
	conIter->second = NULL;
	mContexts.erase(conIter);
	return true;
}

bool NSInputManager::removeKeyTrigger(const nsstring & pContextName, Key pKey, const Trigger & pTrigger)
{
	ContextCollection::iterator conIter = mContexts.find(pContextName);
	if (conIter == mContexts.end())
	{
		dprint("NSInputManager::removeKeyTrigger Cannot find context with name " + pContextName);
		return false;
	}

	auto keyIterPair = conIter->second->mKeyMap.equal_range(pKey);
	auto keyIter = keyIterPair.first;

	while (keyIter != keyIterPair.second)
	{
		if (keyIter->second == pTrigger)
		{
			conIter->second->mKeyMap.erase(keyIter);
			return true;
		}
		++keyIter;
	}
	return false;
}

/*!
Remove all key triggers with the name pTriggerName
Returns true if any triggers are removed and false if none are
*/
bool NSInputManager::removeKeyTriggers(const nsstring & pContextName, const nsstring & pTriggerName)
{
	ContextCollection::iterator conIter = mContexts.find(pContextName);
	if (conIter == mContexts.end())
	{
		dprint("NSInputManager::removeKeyTriggers Cannot find context with name " + pContextName);
		return false;
	}

	bool ret = false;
	auto keyIter = conIter->second->mKeyMap.begin();
	while (keyIter != conIter->second->mKeyMap.end())
	{
		if (keyIter->second.mName == pTriggerName)
		{
			keyIter = conIter->second->mKeyMap.erase(keyIter);
			ret = true;
			continue;
		}
		++keyIter;
	}
	return ret;
}

bool NSInputManager::removeMouseTrigger(const nsstring & pContextName, MouseButton pButton, const Trigger & pTrigger)
{
	ContextCollection::iterator conIter = mContexts.find(pContextName);
	if (conIter == mContexts.end())
	{
		dprint("NSInputManager::removeMouseTrigger Cannot find context with name " + pContextName);
		return false;
	}
	auto mouseIterPair = conIter->second->mMouseButtonMap.equal_range(pButton);
	auto mouseIter = mouseIterPair.first;
	while (mouseIter != mouseIterPair.second)
	{
		if (mouseIter->second == pTrigger)
		{
			conIter->second->mMouseButtonMap.erase(mouseIter);
			return true;
		}
		++mouseIter;
	}
	return false;
}

/*!
Remove all mouse triggers with the name pTriggerName
Returns true if any triggers are removed and false if none are
*/
bool NSInputManager::removeMouseTriggers(const nsstring & pContextName, const nsstring & pTriggerName)
{
	ContextCollection::iterator conIter = mContexts.find(pContextName);
	if (conIter == mContexts.end())
	{
		dprint("NSInputManager::removeMouseTriggers Cannot find context with name " + pContextName);
		return false;
	}
	bool ret = false;
	auto mouseIter = conIter->second->mMouseButtonMap.begin();
	while (mouseIter != conIter->second->mMouseButtonMap.end())
	{
		if (mouseIter->second.mName == pTriggerName)
		{
			mouseIter = conIter->second->mMouseButtonMap.erase(mouseIter);
			ret = true;
			continue;
		}
		++mouseIter;
	}
	return ret;
}

bool NSInputManager::renameContext(const nsstring & pOldContextName, const nsstring & pNewContextName)
{
	ContextCollection::iterator conIter = mContexts.find(pOldContextName);
	if (conIter == mContexts.end())
	{
		dprint("NSInputManager::renameContext Cannot find context with name " + pOldContextName);
		return false;
	}
	if (mContexts.find(pNewContextName) != mContexts.end())
	{
		dprint("NSInputManager::renameContext Cannot rename context " + pOldContextName + " to name " + pNewContextName + ": Context with that name already exists");
		return false;
	}

	Context * context = conIter->second;
	mContexts.erase(conIter);
	context->mName = pNewContextName;

	if (!addContext(context))
	{
		dprint("NSInputManager::renameContext Warning memory leak: Could not addContext and the context was removed without being deleted");
		return false;
	}
	return true;
}

void NSInputManager::setDirectory(const nsstring & pDir)
{
	mDirectory = pDir;
}

void NSInputManager::setLastPos(const fvec2 & pLastPos)
{
	mMLastPos = pLastPos;
}

bool NSInputManager::_checkKeyTriggerModifiers(const Trigger & t)
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

bool NSInputManager::_checkMouseTriggerModifiers(const Trigger & t)
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
