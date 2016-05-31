#ifndef NSINPUTMAP_H
#define NSINPUTMAP_H

#include <nsresource.h>

class nsinput_map : public nsresource
{
  public:

	enum key_val {
		key_a,
		key_b,
		key_c,
		key_d,
		key_e,
		key_f,
		key_g,
		key_h,
		key_i,
		key_j,
		key_k,
		key_l,
		key_m,
		key_n,
		key_o,
		key_p,
		key_q,
		key_r,
		key_s,
		key_t,
		key_u,
		key_v,
		key_w,
		key_x,
		key_y,
		key_z,
		key_1,
		key_2,
		key_3,
		key_4,
		key_5,
		key_6,
		key_7,
		key_8,
		key_9,
		key_0,
		key_grave_accent,
		key_minus,
		key_equal,
		key_backspace,
		key_tab,
		key_lbracket,
		key_rbracket,
		key_backslash,
		key_capslock,
		key_semicolon,
		key_apostrophe,
		key_enter,
		key_lshift,
		key_comma,
		key_period,
		key_forwardslash,
		key_rshift,
		key_lctrl,
		key_lsuper,
		key_lalt,
		key_space,
		key_ralt,
		key_rsuper,
		key_rctrl,
		key_left,
		key_right,
		key_up,
		key_down,
		key_esc,
		key_f1,
		key_f2,
		key_f3,
		key_f4,
		key_f5,
		key_f6,
		key_f7,
		key_f8,
		key_f9,
		key_f10,
		key_f11,
		key_f12,
		key_f13,
		key_f14,
		key_print_screen,
		key_scroll_lock,
		key_pause,
		key_insert,
		key_delete,
		key_page_up,
		key_page_down,
		key_home,
		key_end,
		key_numlock,
		key_keypad_divide,
		key_keypad_multiply,
		key_keypad_subtract,
		key_keypad_add,
		key_keypad_decimal,
		key_keypad_enter,
		key_keypad_equal,
		key_keypad_0,
		key_keypad_1,
		key_keypad_2,
		key_keypad_3,
		key_keypad_4,
		key_keypad_5,
		key_keypad_6,
		key_keypad_7,
		key_keypad_8,
		key_keypad_9,
		key_any
	};

	enum mouse_button_val {
		left_button,
		right_button,
		middle_button,
		aux_button_1,
		aux_button_2,
		aux_button_3,
		aux_button_4,
		movement,
		scrolling,
		any_button
	};

	enum t_state
	{
		t_pressed,
		t_released,
		t_both
	};

	
    typedef std::unordered_set<key_val, EnumHash> key_val_set;
    typedef std::unordered_set<mouse_button_val, EnumHash> mbutton_val_set;

	struct trigger
	{
		trigger(
			const nsstring & name_="",
			t_state trigger_state_=t_pressed,
			bool overwrite_lower_contexts_=true
			);

		void add_key_mod(key_val mod);
		void remove_key_mod(key_val mod);

		void add_mouse_mod(mouse_button_val mod);
		void remove_mouse_mod(mouse_button_val mod);

		const trigger & operator=(const trigger & rhs);
		bool operator==(const trigger & rhs);
		
		nsstring name;
        uint32 hash_name;
		t_state trigger_state;
		key_val_set key_modifiers;
		mbutton_val_set mouse_modifiers;
		bool overwrite_lower_contexts;
	};

	typedef std::unordered_multimap<key_val, trigger, EnumHash> key_trigger_map;
    typedef std::unordered_multimap<mouse_button_val, trigger, EnumHash> mousebutton_trigger_map;

	struct ctxt
	{
		nsstring name;
		key_trigger_map key_map;
		mousebutton_trigger_map mousebutton_map;
	};

	typedef std::unordered_map<nsstring, ctxt*> context_collection;
	
	template <class PUPer>
	friend void pup(PUPer & p, nsinput_map & input);

	nsinput_map();

	nsinput_map(const nsinput_map & copy_);

    virtual ~nsinput_map();

	nsinput_map & operator=(nsinput_map rhs);

	void add_allowed_mod(key_val pKey);
	// In adding the context InputManager takes ownership
	bool add_context(ctxt * toAdd);

	bool add_key_trigger(const nsstring & pContextName, key_val pKey, trigger & pTrigger);

	bool add_mouse_trigger(const nsstring & pContextName, mouse_button_val pButton, trigger & pTrigger);

	bool allowed_mod(key_val mod);
	
	ctxt * context(nsstring name);
	
	ctxt * create_context(const nsstring & name_);

	void remove_allowed_mod(key_val pKey);

	bool remove_context(const nsstring & name_);

	bool remove_key(const nsstring & context_name, key_val key);

	bool remove_key_trigger(const nsstring & pContextName, key_val pKey, const trigger & pTrigger);

	bool remove_key_triggers(const nsstring & pContextName, const nsstring & pTriggerName);

	bool remove_mouse_button(const nsstring & context_name, mouse_button_val button);
	
	bool remove_mouse_button_trigger(const nsstring & pContextName, mouse_button_val pButton, const trigger & pTrigger);

	bool remove_mouse_button_triggers(const nsstring & pContextName, const nsstring & pTriggerName);

	bool rename_context(const nsstring & pOldContextName, const nsstring & pNewContextName);

	virtual void init();

	virtual void name_change(const uivec2 & oldid, const uivec2 newid);

	virtual void pup(nsfile_pupper * p);
	
  private:	
	context_collection m_contexts;
	key_val_set m_allowed_mods;
};

uint8 input_key_to_ascii(nsinput_map::key_val key, bool shift_pressed);


template <class PUPer>
void pup(PUPer & p, nsinput_map & input)
{
	pup(p, input.m_contexts, "contexts");
	pup(p, input.m_allowed_mods, "allowed_mods");
}

template<class PUPer>
void pup(PUPer & p, nsinput_map::key_val & en, const nsstring & var_name)
{
	uint32 in = static_cast<uint32>(en);
	pup(p, in, var_name);
	en = static_cast<nsinput_map::key_val>(in);
}

template<class PUPer>
void pup(PUPer & p, nsinput_map::mouse_button_val & en, const nsstring & var_name)
{
	uint32 in = static_cast<uint32>(en);
	pup(p, in, var_name);
	en = static_cast<nsinput_map::mouse_button_val>(in);
}

template <class PUPer>
void pup(PUPer & p, nsinput_map::ctxt & c, const nsstring & var_name)
{
	pup(p, c.name, var_name + ".name");
	pup(p, c.key_map, var_name + ".key_map");
	pup(p, c.mousebutton_map, var_name + ".mousebutton_map");
}

template <class PUPer>
void pup(PUPer & p, nsinput_map::ctxt * & c, const nsstring & var_name)
{
	if (p.mode() == PUP_IN)
		c = new nsinput_map::ctxt();
	pup(p, *c, var_name);
}

template<class PUPer>
void pup(PUPer & p, nsinput_map::t_state & en, const nsstring & var_name)
{
	uint32 in = static_cast<uint32>(en);
	pup(p, in, var_name);
	en = static_cast<nsinput_map::t_state>(in);
}

uint32 hash_id(const nsstring & );

template <class PUPer>
void pup(PUPer & p, nsinput_map::trigger & t, const nsstring & var_name)
{
	pup(p, t.name, var_name + ".name");
	t.hash_name = hash_id(t.name);
	pup(p, t.trigger_state, var_name + ".trigger_state");
	pup(p, t.key_modifiers, var_name + ".key_modifiers");
	pup(p, t.mouse_modifiers, var_name + ".mouse_modifiers");
	pup(p, t.overwrite_lower_contexts, var_name + ".overwrite_lower_contexts");
}

#endif
