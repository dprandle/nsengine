#ifndef NSINPUTMAP_H
#define NSINPUTMAP_H

#include <nsresource.h>
#include <nsglobal.h>
#include <nspupper.h>

class NSInputMap : public NSResource
{
  public:

	enum Key {
		Key_A,
		Key_B,
		Key_C,
		Key_D,
		Key_E,
		Key_F,
		Key_G,
		Key_H,
		Key_I,
		Key_J,
		Key_K,
		Key_L,
		Key_M,
		Key_N,
		Key_O,
		Key_P,
		Key_Q,
		Key_R,
		Key_S,
		Key_T,
		Key_U,
		Key_V,
		Key_W,
		Key_X,
		Key_Y,
		Key_Z,
		Key_1,
		Key_2,
		Key_3,
		Key_4,
		Key_5,
		Key_6,
		Key_7,
		Key_8,
		Key_9,
		Key_0,
		Key_GraveAccent,
		Key_Minus,
		Key_Equal,
		Key_Backspace,
		Key_Tab,
		Key_LBracket,
		Key_RBracket,
		Key_BSlash,
		Key_CapsLock,
		Key_Semicolon,
		Key_Apostrophe,
		Key_Enter,
		Key_LShift,
		Key_Comma,
		Key_Period,
		Key_FSlash,
		Key_RShift,
		Key_LCtrl,
		Key_LSuper,
		Key_LAlt,
		Key_Space,
		Key_RAlt,
		Key_RSuper,
		Key_RCtrl,
		Key_Left,
		Key_Right,
		Key_Up,
		Key_Down,
		Key_Esc,
		Key_F1,
		Key_F2,
		Key_F3,
		Key_F4,
		Key_F5,
		Key_F6,
		Key_F7,
		Key_F8,
		Key_F9,
		Key_F10,
		Key_F11,
		Key_F12,
		Key_F13,
		Key_F14,
		Key_PrntScrn,
		Key_ScrollLock,
		Key_Pause,
		Key_Insert,
		Key_Delete,
		Key_PGUp,
		Key_PGDown,
		Key_Home,
		Key_End,
		Key_NumLock,
		Key_KPDivide,
		Key_KPMultiply,
		Key_KPSubtract,
		Key_KPAdd,
		Key_KPDecimal,
		Key_KPEnter,
		Key_KPEqual,
		Key_KP0,
		Key_KP1,
		Key_KP2,
		Key_KP3,
		Key_KP4,
		Key_KP5,
		Key_KP6,
		Key_KP7,
		Key_KP8,
		Key_KP9,
		Key_Any
	};

	enum MouseButton {
		LeftButton,
		RightButton,
		MiddleButton,
		AuxButton1,
		AuxButton2,
		AuxButton3,
		AuxButton4,
		Movement,
		Scrolling,
		AnyButton
	};

	enum Axis
	{
		None = 0x0000,
		MouseXPos = 0x0001,
		MouseYPos = 0x0002,
		MouseXDelta = 0x0004,
		MouseYDelta = 0x0008,
		ScrollDelta = 0x0010
	};

	enum TState
	{
		Pressed,
		Released,
		Both,
		Toggle
	};

	
    typedef std::unordered_set<Key, EnumHash> KeyModifiers;
    typedef std::unordered_set<MouseButton, EnumHash> MouseModifiers;
    typedef std::unordered_map<Axis, float, EnumHash> AxisMap;

	struct Trigger
	{
		Trigger(
			const nsstring & pName="",
			TState triggerOn=Pressed,
			uint interestedAxis=0
			);

		void addKeyModifier(Key mod);
		void removeKeyModifier(Key mod);

		void addMouseModifier(MouseButton mod);
		void removeMouseModifier(MouseButton mod);

		const Trigger & operator=(const Trigger & pRhs);
		bool operator==(const Trigger & pRhs);
		
		nsstring mName;
		uint mHashName;
		uint mAxes;
		TState mTriggerOn;
		KeyModifiers mKeyMods;
		MouseModifiers mMouseMods;
		
	};

	typedef std::unordered_multimap<Key, Trigger, EnumHash> KeyMap;
    typedef std::unordered_multimap<MouseButton, Trigger, EnumHash> MouseButtonMap;
	
	struct Context
	{
		nsstring mName;
		KeyMap mKeyMap;
		MouseButtonMap mMouseButtonMap;
	};

	typedef std::unordered_map<nsstring, Context*> ContextCollection;
	
	template <class PUPer>
	friend void pup(PUPer & p, NSInputMap & input);

	NSInputMap();
    virtual ~NSInputMap();

	void addAllowedModifier(Key pKey);
	// In adding the context InputManager takes ownership
	bool addContext(Context * toAdd);

	bool addKeyTrigger(const nsstring & pContextName, Key pKey, Trigger & pTrigger);

	bool addMouseTrigger(const nsstring & pContextName, MouseButton pButton, Trigger & pTrigger);

	bool allowedModifier(Key mod);
	
	Context * context(nsstring name);
	
	Context * createContext(const nsstring & pName);

	void removeAllowedModifier(Key pKey);

	bool removeContext(const nsstring & pName);

	bool removeKey(const nsstring & context_name, Key key);

	bool removeKeyTrigger(const nsstring & pContextName, Key pKey, const Trigger & pTrigger);

	bool removeKeyTriggers(const nsstring & pContextName, const nsstring & pTriggerName);

	bool removeMouseButton(const nsstring & context_name, MouseButton button);
	
	bool removeMouseButtonTrigger(const nsstring & pContextName, MouseButton pButton, const Trigger & pTrigger);

	bool removeMouseButtonTriggers(const nsstring & pContextName, const nsstring & pTriggerName);

	bool renameContext(const nsstring & pOldContextName, const nsstring & pNewContextName);

	virtual void init();

	virtual uivec2array resources();

	virtual void nameChange(const uivec2 & oldid, const uivec2 newid);

	virtual void pup(NSFilePUPer * p);

  private:	
	ContextCollection mContexts;
	KeyModifiers mAllowedModifiers;
};


template <class PUPer>
void pup(PUPer & p, NSInputMap & input)
{
	pup(p, input.mContexts, "contexts");
	pup(p, input.mAllowedModifiers, "allowedmodifiers");
}

template<class PUPer>
void pup(PUPer & p, NSInputMap::Key & en, const nsstring & pString)
{
	uint32 in = static_cast<uint32>(en);
	pup(p, in, pString);
	en = static_cast<NSInputMap::Key>(in);
}

template<class PUPer>
void pup(PUPer & p, NSInputMap::MouseButton & en, const nsstring & pString)
{
	uint32 in = static_cast<uint32>(en);
	pup(p, in, pString);
	en = static_cast<NSInputMap::MouseButton>(in);
}

template <class PUPer>
void pup(PUPer & p, NSInputMap::Context & c, const nsstring & varName)
{
	pup(p, c.mName, varName + ".mName");
	pup(p, c.mKeyMap, varName + ".mKeyMap");
	pup(p, c.mMouseButtonMap, varName + ".mMouseButtonMap");
}

template <class PUPer>
void pup(PUPer & p, NSInputMap::Context * & c, const nsstring & varName)
{
	if (p.mode() == PUP_IN)
		c = new NSInputMap::Context();
	pup(p, *c, varName);
}

template<class PUPer>
void pup(PUPer & p, NSInputMap::Axis & en, const nsstring & pString)
{
	uint32 in = static_cast<uint32>(en);
	pup(p, in, pString);
	en = static_cast<NSInputMap::Axis>(in);
}

template<class PUPer>
void pup(PUPer & p, NSInputMap::TState & en, const nsstring & pString)
{
	uint32 in = static_cast<uint32>(en);
	pup(p, in, pString);
	en = static_cast<NSInputMap::TState>(in);
}

template <class PUPer>
void pup(PUPer & p, NSInputMap::Trigger & t, const nsstring & varName)
{
	pup(p, t.mName, varName + ".mName");
	t.mHashName = hash_id(t.mName);
	pup(p, t.mTriggerOn, varName + ".mTriggerOn");
	pup(p, t.mAxes, varName + ".mAxes");
	pup(p, t.mKeyMods, varName + ".mKeyModifiers");
	pup(p, t.mMouseMods, varName + ".mMouseModifiers");
}

#endif
