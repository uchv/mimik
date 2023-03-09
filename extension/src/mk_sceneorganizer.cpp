#include "mk_sceneorganizer.h"


void MK_SceneOrganizer::init_content()
{
	attach_content_scene("window_sceneorganizer");

	scene_context = MK_SceneContext::get_singleton();
	scene_data = MK_SceneData::get_singleton();
}

void MK_SceneOrganizer::update_content()
{
	content_node->call("update_content");
}

void MK_SceneOrganizer::_bind_methods()
{
}
