#ifndef MIMIK_APPLICATION_H
#define MIMIK_APPLICATION_H

// We don't need windows.h in this example plugin but many others do, and it can
// lead to annoying situations due to the ton of macros it defines.
// So we include it and make sure CI warns us if we use something that conflicts
// with a Windows define.#ifdef WIN32
#include <windows.h>
#endif

#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/color_rect.hpp>
#include <godot_cpp/classes/h_box_container.hpp>
#include <godot_cpp/classes/v_box_container.hpp>
#include "mk_window.h"
#include "mk_scenecontext.h"
#include "mk_scenedata.h"


using namespace godot;


class MimikApplication : public Control
{
	GDCLASS(MimikApplication, Control);

public:
	MimikApplication();

	void init_application();
	void update_application();

	void window_to_front(MimikWindow* window);

protected:
	static void _bind_methods();

private:
	//const int num_windows = 4;
	std::vector<MimikWindow*> windows;

	MimikWindow* front_window = nullptr;

	MK_SceneContext* scene_context = nullptr;
	MK_SceneData* scene_data = nullptr;


	// adds window to godot node tree and member variable array std::vector
	void add_window(MimikWindow* window, MimikWindow* sibling_window, bool start_visible = true, bool horizontal = true, bool before = true);
};