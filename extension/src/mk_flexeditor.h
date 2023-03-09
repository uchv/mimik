#ifndef MIMIK_FLEXEDITOR
#define MIMIK_FLEXEDITOR

#include "mk_window.h"

using namespace godot;


class MK_FlexEditor : public MimikWindow
{
	GDCLASS(MK_FlexEditor, MimikWindow);

public:
	void init_content();
	void update_content();

	String get_window_name() { return "Action Unit Sliders"; }

protected:
	static void _bind_methods();
};

#endif // MIMIK_FLEXEDITOR