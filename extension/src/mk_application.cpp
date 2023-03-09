#include "mk_application.h"

#include <godot_cpp/core/class_db.hpp>

#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/theme.hpp>
#include <godot_cpp/classes/input.hpp>

#include "mk_window.h"
#include "mk_scenedata.h"

#include "mk_modelviewport.h"
#include "mk_flexeditor.h"
#include "mk_flexanimation.h"
#include "mk_sceneorganizer.h"
#include "mk_lipsync.h"
#include "mk_facseditor.h"

#include <godot_cpp/variant/utility_functions.hpp>



using namespace godot;


MimikApplication::MimikApplication()
{

}

void MimikApplication::init_application()
{
	UtilityFunctions::print("Initiating application");

	// set project theme
	Ref<Theme> theme = ResourceLoader::get_singleton()->load("res://MimikTheme.tres");

	// get singletons
	scene_data = MK_SceneData::get_singleton();
	scene_context = MK_SceneContext::get_singleton();


	// CREATE WINDOWS
	// ModelViewport
	MK_ModelViewport* window_viewport = memnew(MK_ModelViewport);
	add_window(window_viewport, nullptr, true);
	window_viewport->set_position(Vector2(10.0f, 60.0f));


	// @TODO messy way of doing this
	scene_context->timer_node = window_viewport;

	

	// AUEditor
	MK_FlexEditor* window_flexeditor = memnew(MK_FlexEditor);
	add_window(window_flexeditor, window_viewport, true);

	window_flexeditor->set_position(Vector2(500.0f, 60.0f));
	window_flexeditor->set_window_size(Vector2(600.0f, 500.0f));
	

	// AUAnimation
	MK_FlexAnimation* window_flexanimation = memnew(MK_FlexAnimation);
	add_window(window_flexanimation, window_flexeditor, true);

	window_flexanimation->set_position(Vector2(700.0f, 600.0f));
	window_flexanimation->set_window_size(Vector2(600, 400.0f));


	// SceneOrganizer
	MK_SceneOrganizer* window_sceneorganizer = memnew(MK_SceneOrganizer);
	add_window(window_sceneorganizer, window_flexanimation, true);

	window_sceneorganizer->set_position(Vector2(10.0f, 600.0f));
	window_sceneorganizer->set_window_size(Vector2(600, 400.0f));

	// LipSync
	MK_LipSync* window_lipsync = memnew(MK_LipSync);
	add_window(window_lipsync, window_sceneorganizer, false);

	window_lipsync->set_position(Vector2(1500.0f, 100.0f));
	window_lipsync->set_window_size(Vector2(600.0f, 400.0f));

	// FACS Editor
	MK_FACSEditor* window_facseditor= memnew(MK_FACSEditor);
	add_window(window_facseditor, window_lipsync, false);

	window_facseditor->set_position(Vector2(1500.0f, 600.0f));
	window_facseditor->set_window_size(Vector2(600.0f, 400.0f));


	for (int i = 0; i < windows.size(); i++)
	{
		windows[i]->set_theme(theme);
		windows[i]->init_window(windows[i]->get_window_name());
		windows[i]->init_content();
	}


	UtilityFunctions::print("Finished initiating application");
}


void MimikApplication::update_application()
{
	scene_context->update_context();
	scene_data->update_data();


	bool need_reordering = false;
	MimikWindow* top_window = nullptr;
	for (int i = 0; i < windows.size(); i++)
	{
		windows[i]->update_window();
		
		// put the window that is being interacted with in front of the other windows
		if (windows[i]->is_window_dragged() && windows[i] != front_window)
		{
			need_reordering = true;
			top_window = windows[i];
		}
	}

	// reorder to focus on window that has been clicked
	if (need_reordering && top_window)
	{
		UtilityFunctions::print("reordering");
		front_window = top_window;
		window_to_front(top_window);
	}
}

void MimikApplication::window_to_front(MimikWindow* window)
{
	if (window)
	{
		Node* parent = window->get_parent();
		parent->remove_child(window);
		parent->add_child(window);
	}
}

void MimikApplication::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("init_application"), &MimikApplication::init_application);
	ClassDB::bind_method(D_METHOD("update_application"), &MimikApplication::update_application);
}


void MimikApplication::add_window(MimikWindow* window, MimikWindow* sibling_window, bool start_visible, bool horizontal, bool before)
{
	windows.push_back(window);

	add_child(window);

	call("register_window_taskbar", window, start_visible);
}
