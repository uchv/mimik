#include "mk_viewportscene.h"

#include "mk_scenedata.h"

#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/packed_scene.hpp>


void MK_ViewportScene::setup_basics()
{
	ResourceLoader* loader = ResourceLoader::get_singleton();
	Ref<PackedScene> base_scene = loader->load("res://PackedScenes/ViewportSceneBase.tscn");

	Node* node = base_scene->instantiate();
	add_child(node);
}


void MK_ViewportScene::_bind_methods()
{

}
