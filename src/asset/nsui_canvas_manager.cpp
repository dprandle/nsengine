#include <asset/nsui_canvas_manager.h>

nsui_canvas_manager::nsui_canvas_manager() :
	nsasset_manager(type_to_hash(nsui_canvas_manager))
{
	set_local_dir(LOCAL_SCENE_DIR_DEFAULT);
}

nsui_canvas_manager::~nsui_canvas_manager()
{}
