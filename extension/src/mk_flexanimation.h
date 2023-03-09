#ifndef MIMIK_FLEXANIMATION
#define MIMIK_FLEXANIMATION

#include "mk_window.h"

using namespace godot;


// window for flex animation
class MK_FlexAnimation : public MimikWindow
{
	GDCLASS(MK_FlexAnimation, MimikWindow);

public:
	void init_content();
	void update_content();

	String get_window_name() { return "Action Unit Animation"; }

protected:
	static void _bind_methods();
};

#endif // MIMIK_FLEXANIMATION