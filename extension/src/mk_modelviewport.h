#ifndef MIMIK_MODELVIEWPORT
#define MIMIK_MODELVIEWPORT

#include "mk_window.h"

#include <godot_cpp/classes/node.hpp>


using namespace godot;

class MK_ModelViewport : public MimikWindow
{
	GDCLASS(MK_ModelViewport, MimikWindow);

public:
	void init_content();
	void update_content();

	String get_window_name() { return "Model Viewport"; }
protected:
	static void _bind_methods();

private:
	
};

#endif //MIMIK_MODELVIEWPORT