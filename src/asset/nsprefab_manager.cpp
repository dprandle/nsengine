#include <asset/nsprefab_manager.h>

nsprefab_manager::nsprefab_manager() :
	nsasset_manager(type_to_hash(nsprefab_manager))
{
	set_local_dir(LOCAL_SCENE_DIR_DEFAULT);
}

nsprefab_manager::~nsprefab_manager()
{}
