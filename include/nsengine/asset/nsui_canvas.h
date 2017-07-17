
#ifndef NSUI_CANVAS_H
#define NSUI_CANVAS_H

#include <asset/nsasset.h>

class nsui_canvas : public nsasset
{
public:

	template <class PUPer>
	friend void pup(PUPer & p, nsui_canvas & shader);

	nsui_canvas();

	nsui_canvas(const nsui_canvas & copy_);

	~nsui_canvas();

	nsui_canvas & operator=(nsui_canvas rhs);

	void init() {}

	virtual void pup(nsfile_pupper * p);
};

template <class PUPer>
void pup(PUPer & p, nsui_canvas & canvas)
{
}


#endif
