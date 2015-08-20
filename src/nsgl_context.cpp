#include <nsgl_context.h>
#include <nsplugin.h>
#include <nsplugin_manager.h>
#include <nsevent_dispatcher.h>
#include <nstimer.h>

#ifdef NSDEBUG
#include <nsdebug.h>
#endif

gl_ctxt::gl_ctxt(uint32 id) :
	glew_context(new GLEWContext()),
	core_plugin(new nsplugin()),
	systems(new system_hash_map()),
	plugins(new nsplugin_manager()),
	event_disp(new nsevent_dispatcher()),
	fb_map(),
	timer(new nstimer()),
	composite_buf(0),
	context_id(id)
#ifdef NSDEBUG
	,deb(new nsdebug())
#endif
{
	core_plugin->rename(ENGINE_PLUG);
#ifdef NSDEBUG
	deb->set_log_file("engine_ctxt" + std::to_string(context_id) + ".log");
	deb->set_log_dir(nsengine.cwd() + "logs");
	deb->clear_log();
#endif
}

gl_ctxt::~gl_ctxt()
{
	delete plugins;
	delete systems;
	delete core_plugin;
	delete event_disp;
	delete glew_context;
	delete timer;
#ifdef NSDEBUG
	delete deb;
#endif
}

GLEWContext * glewGetContext()
{
	return nse.current()->glew_context;
}
