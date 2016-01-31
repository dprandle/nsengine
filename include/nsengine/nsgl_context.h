#ifndef GL_CONTEXT_H
#define GL_CONTEXT_H

#include <nsengine.h>
#include <myGL/glew.h>

GLEWContext * glewGetContext();

struct gl_ctxt
{
	gl_ctxt(uint32 id);
	~gl_ctxt();
	GLEWContext * glew_context;
	nsplugin * core_plugin;
	system_hash_map * systems;
	nsplugin_manager * plugins;
	nsevent_dispatcher * event_disp;
	nstimer * timer;
	uint32 composite_buf;
	uint32 context_id;
#ifdef NSDEBUG
	nsdebug * deb;
#endif
	
};


#endif
