#include <nsinputmap.h>
#include <nsengine.h>

NSInputMap::NSInputMap()
{}

NSInputMap::~NSInputMap()
{
	ContextCollection::iterator iter = mContexts.begin();
	while (iter != mContexts.end())
	{
		delete iter->second;
		iter->second = NULL;
		iter = mContexts.erase(iter);
	}	
}

void NSInputMap::addAllowedModifier(Key pKey)
{
	mAllowedModifiers.insert(pKey);
}

// In adding the context InputManager takes ownership
bool NSInputMap::addContext(Context * toAdd)
{
	if (toAdd == NULL)
	{
		dprint("NSInputMap::addContext Trying to add NULL context");
		return false;
	}
	if (mContexts.find(toAdd->mName) != mContexts.end())
	{
		dprint("NSInputMap::addContext Cannot add context with same name " + toAdd->mName);
		return false;
	}
	mContexts[toAdd->mName] = toAdd;
	return true;
}

bool NSInputMap::addKeyTrigger(const nsstring & pContextName, Key pKey, const Trigger & pTrigger)
{
	ContextCollection::iterator conIter = mContexts.find(pContextName);
	if (conIter == mContexts.end())
	{
		dprint("NSInputMap::addKeyTrigger Cannot find context with name " + pContextName);
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
			dprint("NSInputMap::addKeyTrigger Cannot add trigger \"" + pTrigger.mName + "\" as an exact copy already exists in context");
			return false;
		}
		++iterStart;
	}

	conIter->second->mKeyMap.emplace(pKey,pTrigger);
	return true;
}

bool NSInputMap::addMouseTrigger(const nsstring & pContextName, MouseButton pButton, const Trigger & pTrigger)
{
	ContextCollection::iterator conIter = mContexts.find(pContextName);
	if (conIter == mContexts.end())
	{
		dprint("NSInputMap::addMouseTrigger Cannot find context with name " + pContextName);
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
			dprint("NSInputMap::addMouseTrigger Cannot add trigger \"" + pTrigger.mName + "\" as an exact copy already exists in context");
			return false;
		}
		++iterStart;
	}

	conIter->second->mMouseButtonMap.emplace(pButton,pTrigger);
	return true;
}

NSInputMap::Context * NSInputMap::createContext(const nsstring & pName)
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

void NSInputMap::keyRelease(Key pKey)
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

void NSInputMap::pushContext(const nsstring & pName)
{
	ContextCollection::iterator conIter = mContexts.find(pName);
	if (conIter == mContexts.end())
	{
		dprint("NSInputMap::pushContext Cannot find context with name " + pName);
		return;
	}
	mContextStack.push_back(conIter->second);
}

void NSInputMap::removeAllowedModifier(Key pKey)
{
	mAllowedModifiers.erase(pKey);
}

bool NSInputMap::removeContext(const nsstring & pName)
{
	ContextCollection::iterator conIter = mContexts.find(pName);
	if (conIter == mContexts.end())
	{
		dprint("NSInputMap::pushContext Cannot find context with name " + pName);
		return false;
	}
	delete conIter->second;
	conIter->second = NULL;
	mContexts.erase(conIter);
	return true;
}

bool NSInputMap::removeKeyTrigger(const nsstring & pContextName, Key pKey, const Trigger & pTrigger)
{
	ContextCollection::iterator conIter = mContexts.find(pContextName);
	if (conIter == mContexts.end())
	{
		dprint("NSInputMap::removeKeyTrigger Cannot find context with name " + pContextName);
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
bool NSInputMap::removeKeyTriggers(const nsstring & pContextName, const nsstring & pTriggerName)
{
	ContextCollection::iterator conIter = mContexts.find(pContextName);
	if (conIter == mContexts.end())
	{
		dprint("NSInputMap::removeKeyTriggers Cannot find context with name " + pContextName);
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

bool NSInputMap::removeMouseTrigger(const nsstring & pContextName, MouseButton pButton, const Trigger & pTrigger)
{
	ContextCollection::iterator conIter = mContexts.find(pContextName);
	if (conIter == mContexts.end())
	{
		dprint("NSInputMap::removeMouseTrigger Cannot find context with name " + pContextName);
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
bool NSInputMap::removeMouseTriggers(const nsstring & pContextName, const nsstring & pTriggerName)
{
	ContextCollection::iterator conIter = mContexts.find(pContextName);
	if (conIter == mContexts.end())
	{
		dprint("NSInputMap::removeMouseTriggers Cannot find context with name " + pContextName);
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

bool NSInputMap::renameContext(const nsstring & pOldContextName, const nsstring & pNewContextName)
{
	ContextCollection::iterator conIter = mContexts.find(pOldContextName);
	if (conIter == mContexts.end())
	{
		dprint("NSInputMap::renameContext Cannot find context with name " + pOldContextName);
		return false;
	}
	if (mContexts.find(pNewContextName) != mContexts.end())
	{
		dprint("NSInputMap::renameContext Cannot rename context " + pOldContextName + " to name " + pNewContextName + ": Context with that name already exists");
		return false;
	}

	Context * context = conIter->second;
	mContexts.erase(conIter);
	context->mName = pNewContextName;

	if (!addContext(context))
	{
		dprint("NSInputMap::renameContext Warning memory leak: Could not addContext and the context was removed without being deleted");
		return false;
	}
	return true;
}

void NSInputMap::init()
{

}

uivec2array NSInputMap::resources()
{

}

void NSInputMap::nameChange(const uivec2 & oldid, const uivec2 newid)
{

}

void NSInputMap::pup(NSFilePUPer * p)
{

}
