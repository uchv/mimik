#include "mk_facseditor.h"

#include <godot_cpp/variant/utility_functions.hpp>

void MK_FACSEditor::init_content()
{
	attach_content_scene("window_facseditor");

	scene_context = MK_SceneContext::get_singleton();
	scene_data = MK_SceneData::get_singleton();
}

void MK_FACSEditor::update_content()
{
	content_node->call("update_content");


}

void MK_FACSEditor::_bind_methods()
{
}
