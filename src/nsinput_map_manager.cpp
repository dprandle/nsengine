#include <nsinput_map_manager.h>


NSInputMapManager::NSInputMapManager()
{
	setLocalDirectory(LOCAL_INPUT_DIR_DEFAULT);
	setSaveMode(Text);
}

NSInputMapManager::~NSInputMapManager()
{
	
}
