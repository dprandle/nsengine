#ifndef NSUI_CANVAS_MANAGER_H
#define NSUI_CANVAS_MANAGER_H

#include <asset/nsasset_manager.h>
#include <asset/nsui_canvas.h>

class nsui_canvas_manager : public nsasset_manager
{
public:

	MANAGER_TEMPLATES
	
	nsui_canvas_manager();
	~nsui_canvas_manager();
};

#endif
