#include <nsinput_map.h>
#include <nsengine.h>

nsinput_map::nsinput_map()
{
	set_ext(DEFAULT_INPUT_MAP_EXTENSION);
}

nsinput_map::~nsinput_map()
{
	context_collection::iterator iter = m_contexts.begin();
	while (iter != m_contexts.end())
	{
		delete iter->second;
		iter->second = NULL;
		iter = m_contexts.erase(iter);
	}	
}

void nsinput_map::add_allowed_mod(key_val pKey)
{
	m_allowed_mods.insert(pKey);
}

// In adding the context InputManager takes ownership
bool nsinput_map::add_context(ctxt * toAdd)
{
	if (toAdd == NULL)
	{
		dprint("nsinput_map::addContext Trying to add NULL context");
		return false;
	}
	if (context(toAdd->name) != NULL)
	{
		dprint("nsinput_map::addContext Cannot add context with same name " + toAdd->name);
		return false;
	}
	m_contexts[toAdd->name] = toAdd;
	return true;
}

bool nsinput_map::add_key_trigger(const nsstring & pContextName, key_val pKey, trigger & pTrigger)
{
	ctxt * ctxt = context(pContextName);
	if (ctxt == NULL)
		return false;

	pTrigger.hash_name = hash_id(pTrigger.name);
	
	// Allow for multiple trigger entrees, but all of the triggers must be unique - they
	// can have the same name but if they do then the key modifiers must be different, for example.
	auto iterPair = ctxt->key_map.equal_range(pKey);
	while (iterPair.first != iterPair.second)
	{
		if (iterPair.first->second == pTrigger)
		{
			dprint("nsinput_map::addKeyTrigger Cannot add trigger \"" + pTrigger.name + "\" as an exact copy already exists in context");
			return false;
		}
		++iterPair.first;
	}

	ctxt->key_map.emplace(pKey,pTrigger);
	return true;
}

bool nsinput_map::add_mouse_trigger(const nsstring & pContextName, mouse_button_val pButton, trigger & pTrigger)
{
	ctxt * ctxt = context(pContextName);
	if (ctxt == NULL)
		return false;

	pTrigger.hash_name = hash_id(pTrigger.name);
	// Allow for multiple trigger entrees, but all of the triggers must be unique - they
	// can have the same name but if they do then the key modifiers must be different, for example.
	// Or, if the key modifiers are the same then the names must be different.
	auto iterPair = ctxt->mousebutton_map.equal_range(pButton);
	while (iterPair.first != iterPair.second)
	{
		if (iterPair.first->second == pTrigger)
		{
			dprint("nsinput_map::addMouseTrigger Cannot add trigger \"" + pTrigger.name + "\" as an exact copy already exists in context");
			return false;
		}
		++iterPair.first;
	}

	ctxt->mousebutton_map.emplace(pButton,pTrigger);
	return true;
}

bool nsinput_map::allowed_mod(key_val mod)
{
	return (m_allowed_mods.find(mod) != m_allowed_mods.end());
}

nsinput_map::ctxt * nsinput_map::context(nsstring name)
{
	context_collection::iterator conIter = m_contexts.find(name);
	if (conIter == m_contexts.end())
		return NULL;
	return conIter->second;
}

nsinput_map::ctxt * nsinput_map::create_context(const nsstring & pName)
{
	ctxt * toAdd = new ctxt();
	toAdd->name = pName;
	if (!add_context(toAdd))
	{
		delete toAdd;
		return NULL;
	}
	return toAdd;
}

void nsinput_map::remove_allowed_mod(key_val pKey)
{
	m_allowed_mods.erase(pKey);
}

bool nsinput_map::remove_context(const nsstring & pName)
{
	context_collection::iterator conIter = m_contexts.find(pName);

	if (conIter == m_contexts.end())
		return false;

	delete conIter->second;
	conIter->second = NULL;
	m_contexts.erase(conIter);
	return true;
}

/* 
Remove all key triggers  
 */
bool nsinput_map::remove_key(const nsstring & context_name, key_val key)
{
	ctxt * ctxt = context(context_name);
	if (ctxt == NULL)
		return false;
	
	auto keyIterPair = ctxt->key_map.equal_range(key);
	ctxt->key_map.erase(keyIterPair.first, keyIterPair.second);
	return true;
}

bool nsinput_map::remove_key_trigger(const nsstring & pContextName, key_val pKey, const trigger & pTrigger)
{
	ctxt * ctxt = context(pContextName);
	if (ctxt == NULL)
		return false;
	
	auto keyIterPair = ctxt->key_map.equal_range(pKey);
	while (keyIterPair.first != keyIterPair.second)
	{
		if (keyIterPair.first->second == pTrigger)
		{
			ctxt->key_map.erase(keyIterPair.first);
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
bool nsinput_map::remove_key_triggers(const nsstring & pContextName, const nsstring & pTriggerName)
{
	ctxt * ctxt = context(pContextName);
	if (ctxt == NULL)
		return false;

	bool ret = false;
	auto keyIter = ctxt->key_map.begin();
	while (keyIter != ctxt->key_map.end())
	{
		if (keyIter->second.name == pTriggerName)
		{
			keyIter = ctxt->key_map.erase(keyIter);
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
bool nsinput_map::remove_mouse_button(const nsstring & context_name, mouse_button_val button)
{
	ctxt * ctxt = context(context_name);
	if (ctxt == NULL)
		return false;
	auto mouseIterPair = ctxt->mousebutton_map.equal_range(button);
	ctxt->mousebutton_map.erase(mouseIterPair.first, mouseIterPair.second);
	return true;
}

bool nsinput_map::remove_mouse_button_trigger(const nsstring & pContextName, mouse_button_val pButton, const trigger & pTrigger)
{
	ctxt * ctxt = context(pContextName);
	if (ctxt == NULL)
		return false;

	auto mouseIterPair = ctxt->mousebutton_map.equal_range(pButton);
	while (mouseIterPair.first != mouseIterPair.second)
	{
		if (mouseIterPair.first->second == pTrigger)
		{
			ctxt->mousebutton_map.erase(mouseIterPair.first);
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
bool nsinput_map::remove_mouse_button_triggers(const nsstring & pContextName, const nsstring & pTriggerName)
{
	ctxt * ctxt = context(pContextName);
	if (ctxt == NULL)
		return false;

	bool ret = false;
	auto mouseIter = ctxt->mousebutton_map.begin();
	while (mouseIter != ctxt->mousebutton_map.end())
	{
		if (mouseIter->second.name == pTriggerName)
		{
			mouseIter = ctxt->mousebutton_map.erase(mouseIter);
			ret = true;
			continue;
		}
		++mouseIter;
	}
	return ret;
}

bool nsinput_map::rename_context(const nsstring & pOldContextName, const nsstring & pNewContextName)
{
	auto conIter = m_contexts.find(pOldContextName); ctxt * nctxt = context(pNewContextName);
	if (conIter == m_contexts.end() || nctxt != NULL)
		return false;
	
	ctxt * ctxt = conIter->second;
	m_contexts.erase(conIter);
	ctxt->name = pNewContextName;

	if (!add_context(ctxt))
		return false;
	
	return true;
}

void nsinput_map::init()
{
	m_allowed_mods.insert(key_any);
	m_allowed_mods.insert(key_lctrl);
	m_allowed_mods.insert(key_lshift);
	m_allowed_mods.insert(key_lalt);
	m_allowed_mods.insert(key_lsuper);
	m_allowed_mods.insert(key_z);
	m_allowed_mods.insert(key_x);
	m_allowed_mods.insert(key_y);
}

void nsinput_map::name_change(const uivec2 & oldid, const uivec2 newid)
{

}

void nsinput_map::pup(nsfile_pupper * p)
{
	if (p->type() == nsfile_pupper::pup_binary)
	{
		nsbinary_file_pupper * bf = static_cast<nsbinary_file_pupper *>(p);
		::pup(*bf, *this);
	}
	else
	{
		nstext_file_pupper * tf = static_cast<nstext_file_pupper *>(p);
		::pup(*tf, *this);
	}
}


nsinput_map::trigger::trigger(
	const nsstring & pName,
	t_state triggerOn,
    uint32 interestedAxis,
	bool overwrite_lower_contexts_):
	name(pName),
	hash_name(0),
	trigger_state(triggerOn),
	axis_bitfield(interestedAxis),
	overwrite_lower_contexts(overwrite_lower_contexts_)
	{}

void nsinput_map::trigger::add_key_mod(key_val mod)
{
	key_modifiers.insert(mod);
}

void nsinput_map::trigger::remove_key_mod(key_val mod)
{
	key_modifiers.erase(mod);
}

void nsinput_map::trigger::add_mouse_mod(mouse_button_val mod)
{
	mouse_modifiers.insert(mod);	
}
void nsinput_map::trigger::remove_mouse_mod(mouse_button_val mod)
{
	mouse_modifiers.erase(mod);
}
		
const nsinput_map::trigger & nsinput_map::trigger::operator=(const trigger & pRhs)
{
	name = pRhs.name;
	trigger_state = pRhs.trigger_state;
	axis_bitfield = pRhs.axis_bitfield;
	key_modifiers = pRhs.key_modifiers;
	mouse_modifiers = pRhs.mouse_modifiers;
	return *this;
}

bool nsinput_map::trigger::operator==(const trigger & pRhs)
{
	return (
		hash_name == pRhs.hash_name &&
		key_modifiers == pRhs.key_modifiers &&
		mouse_modifiers == pRhs.mouse_modifiers
		);
}
