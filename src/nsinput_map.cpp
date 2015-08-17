#include <nsinput_map.h>
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
	if (context(toAdd->mName) != NULL)
	{
		dprint("NSInputMap::addContext Cannot add context with same name " + toAdd->mName);
		return false;
	}
	mContexts[toAdd->mName] = toAdd;
	return true;
}

bool NSInputMap::addKeyTrigger(const nsstring & pContextName, Key pKey, Trigger & pTrigger)
{
	Context * ctxt = context(pContextName);
	if (ctxt == NULL)
		return false;

	pTrigger.mHashName = hash_id(pTrigger.mName);
	
	// Allow for multiple trigger entrees, but all of the triggers must be unique - they
	// can have the same name but if they do then the key modifiers must be different, for example.
	auto iterPair = ctxt->mKeyMap.equal_range(pKey);
	while (iterPair.first != iterPair.second)
	{
		if (iterPair.first->second == pTrigger)
		{
			dprint("NSInputMap::addKeyTrigger Cannot add trigger \"" + pTrigger.mName + "\" as an exact copy already exists in context");
			return false;
		}
		++iterPair.first;
	}

	ctxt->mKeyMap.emplace(pKey,pTrigger);
	return true;
}

bool NSInputMap::addMouseTrigger(const nsstring & pContextName, MouseButton pButton, Trigger & pTrigger)
{
	Context * ctxt = context(pContextName);
	if (ctxt == NULL)
		return false;

	pTrigger.mHashName = hash_id(pTrigger.mName);
	// Allow for multiple trigger entrees, but all of the triggers must be unique - they
	// can have the same name but if they do then the key modifiers must be different, for example.
	// Or, if the key modifiers are the same then the names must be different.
	auto iterPair = ctxt->mMouseButtonMap.equal_range(pButton);
	while (iterPair.first != iterPair.second)
	{
		if (iterPair.first->second == pTrigger)
		{
			dprint("NSInputMap::addMouseTrigger Cannot add trigger \"" + pTrigger.mName + "\" as an exact copy already exists in context");
			return false;
		}
		++iterPair.first;
	}

	ctxt->mMouseButtonMap.emplace(pButton,pTrigger);
	return true;
}

bool NSInputMap::allowedModifier(Key mod)
{
	return (mAllowedModifiers.find(mod) != mAllowedModifiers.end());
}

NSInputMap::Context * NSInputMap::context(nsstring name)
{
	ContextCollection::iterator conIter = mContexts.find(name);
	if (conIter == mContexts.end())
		return NULL;
	return conIter->second;
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

void NSInputMap::removeAllowedModifier(Key pKey)
{
	mAllowedModifiers.erase(pKey);
}

bool NSInputMap::removeContext(const nsstring & pName)
{
	ContextCollection::iterator conIter = mContexts.find(pName);

	if (conIter == mContexts.end())
		return false;

	delete conIter->second;
	conIter->second = NULL;
	mContexts.erase(conIter);
	return true;
}

/* 
Remove all key triggers  
 */
bool NSInputMap::removeKey(const nsstring & context_name, Key key)
{
	Context * ctxt = context(context_name);
	if (ctxt == NULL)
		return false;
	
	auto keyIterPair = ctxt->mKeyMap.equal_range(key);
	ctxt->mKeyMap.erase(keyIterPair.first, keyIterPair.second);
	return true;
}

bool NSInputMap::removeKeyTrigger(const nsstring & pContextName, Key pKey, const Trigger & pTrigger)
{
	Context * ctxt = context(pContextName);
	if (ctxt == NULL)
		return false;
	
	auto keyIterPair = ctxt->mKeyMap.equal_range(pKey);
	while (keyIterPair.first != keyIterPair.second)
	{
		if (keyIterPair.first->second == pTrigger)
		{
			ctxt->mKeyMap.erase(keyIterPair.first);
			return true;
		}
		++keyIterPair.first;
	}
	return false;
}

/*!
Remove all key triggers with the name pTriggerName
Returns true if any triggers are removed and false if none are
*/
bool NSInputMap::removeKeyTriggers(const nsstring & pContextName, const nsstring & pTriggerName)
{
	Context * ctxt = context(pContextName);
	if (ctxt == NULL)
		return false;

	bool ret = false;
	auto keyIter = ctxt->mKeyMap.begin();
	while (keyIter != ctxt->mKeyMap.end())
	{
		if (keyIter->second.mName == pTriggerName)
		{
			keyIter = ctxt->mKeyMap.erase(keyIter);
			ret = true;
			continue;
		}
		++keyIter;
	}
	return ret;
}

 
/* 
Remove all mouse button triggers  
 */
bool NSInputMap::removeMouseButton(const nsstring & context_name, MouseButton button)
{
	Context * ctxt = context(context_name);
	if (ctxt == NULL)
		return false;
	auto mouseIterPair = ctxt->mMouseButtonMap.equal_range(button);
	ctxt->mMouseButtonMap.erase(mouseIterPair.first, mouseIterPair.second);
	return true;
}

bool NSInputMap::removeMouseButtonTrigger(const nsstring & pContextName, MouseButton pButton, const Trigger & pTrigger)
{
	Context * ctxt = context(pContextName);
	if (ctxt == NULL)
		return false;

	auto mouseIterPair = ctxt->mMouseButtonMap.equal_range(pButton);
	while (mouseIterPair.first != mouseIterPair.second)
	{
		if (mouseIterPair.first->second == pTrigger)
		{
			ctxt->mMouseButtonMap.erase(mouseIterPair.first);
			return true;
		}
		++mouseIterPair.first;
	}
	return false;
}

/*!
Remove all mouse triggers with the name pTriggerName
Returns true if any triggers are removed and false if none are
*/
bool NSInputMap::removeMouseButtonTriggers(const nsstring & pContextName, const nsstring & pTriggerName)
{
	Context * ctxt = context(pContextName);
	if (ctxt == NULL)
		return false;

	bool ret = false;
	auto mouseIter = ctxt->mMouseButtonMap.begin();
	while (mouseIter != ctxt->mMouseButtonMap.end())
	{
		if (mouseIter->second.mName == pTriggerName)
		{
			mouseIter = ctxt->mMouseButtonMap.erase(mouseIter);
			ret = true;
			continue;
		}
		++mouseIter;
	}
	return ret;
}

bool NSInputMap::renameContext(const nsstring & pOldContextName, const nsstring & pNewContextName)
{
	auto conIter = mContexts.find(pOldContextName); Context * nctxt = context(pNewContextName);
	if (conIter == mContexts.end() || nctxt != NULL)
		return false;
	
	Context * ctxt = conIter->second;
	mContexts.erase(conIter);
	ctxt->mName = pNewContextName;

	if (!addContext(ctxt))
		return false;
	
	return true;
}

void NSInputMap::init()
{
	mAllowedModifiers.insert(Key_Any);
	mAllowedModifiers.insert(Key_LCtrl);
	mAllowedModifiers.insert(Key_LShift);
	mAllowedModifiers.insert(Key_LAlt);
	mAllowedModifiers.insert(Key_LSuper);
	mAllowedModifiers.insert(Key_Z);
	mAllowedModifiers.insert(Key_X);
	mAllowedModifiers.insert(Key_Y);
}

uivec2array NSInputMap::resources()
{
	return uivec2array();
}

void NSInputMap::nameChange(const uivec2 & oldid, const uivec2 newid)
{

}

void NSInputMap::pup(NSFilePUPer * p)
{
	if (p->type() == NSFilePUPer::Binary)
	{
		NSBinFilePUPer * bf = static_cast<NSBinFilePUPer *>(p);
		::pup(*bf, *this);
	}
	else
	{
		NSTextFilePUPer * tf = static_cast<NSTextFilePUPer *>(p);
		::pup(*tf, *this);
	}
}


NSInputMap::Trigger::Trigger(
	const nsstring & pName,
	TState triggerOn,
	uint interestedAxis):
	mName(pName),
	mHashName(0),
	mTriggerOn(triggerOn),
	mAxes(interestedAxis)
	{}

void NSInputMap::Trigger::addKeyModifier(Key mod)
{
	mKeyMods.insert(mod);
}

void NSInputMap::Trigger::removeKeyModifier(Key mod)
{
	mKeyMods.erase(mod);
}

void NSInputMap::Trigger::addMouseModifier(MouseButton mod)
{
	mMouseMods.insert(mod);	
}
void NSInputMap::Trigger::removeMouseModifier(MouseButton mod)
{
	mMouseMods.erase(mod);
}
		
const NSInputMap::Trigger & NSInputMap::Trigger::operator=(const Trigger & pRhs)
{
	mName = pRhs.mName;
	mTriggerOn = pRhs.mTriggerOn;
	mAxes = pRhs.mAxes;
	mKeyMods = pRhs.mKeyMods;
	mMouseMods = pRhs.mMouseMods;
	return *this;
}

bool NSInputMap::Trigger::operator==(const Trigger & pRhs)
{
	return (
		mHashName == pRhs.mHashName &&
		mKeyMods == pRhs.mKeyMods &&
		mMouseMods == pRhs.mMouseMods
		);
}
