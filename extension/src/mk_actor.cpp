#include "mk_actor.h"

#include "mk_scenedata.h"
#include "mk_scenecontext.h"

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/array_mesh.hpp>

#include <godot_cpp/classes/gltf_document.hpp>
#include <godot_cpp/classes/gltf_state.hpp>

#include <godot_cpp/variant/string_name.hpp>



// finds mesh instance in gltf import scene
MeshInstance3D* find_mesh_instance(Node* parent)
{
	MeshInstance3D* result = nullptr;

	int child_count = parent->get_child_count();
	for (int i = 0; i < child_count; i++)
	{
		Node* child = parent->get_child(i);
		
		// check for type
		MeshInstance3D* mesh_ptr = Object::cast_to<MeshInstance3D>(child);
		if(mesh_ptr != nullptr)
		{
			return mesh_ptr;
		}
		else
		{
			MeshInstance3D* child_mesh = find_mesh_instance(child);
			if (child_mesh != nullptr)
			{
				return child_mesh;
			}
		}
	}

	return result;
}


void MK_Actor::load_actor(String model_path)
{
	Ref<GLTFDocument> gltf_doc;
	gltf_doc.instantiate();
	Ref<GLTFState> gltf_state;
	gltf_state.instantiate();

	Error err = gltf_doc->append_from_file(model_path, gltf_state);

	// Error 0 = OK, everything else -> fail
	if (err != 0)
	{
		UtilityFunctions::print("Failed to load model.");
	}

	Node* model_scene = gltf_doc->generate_scene(gltf_state);


	MeshInstance3D* mesh_instance_node = find_mesh_instance(model_scene);
	
	if (!mesh_instance_node)
	{
		// could not load mesh, therefore actor stays unloaded and we abort.
		is_loaded = false;
		UtilityFunctions::print("Could not load mesh.");
		return;
	}

	actor_root_node = model_scene;
	actor_mesh_instance = mesh_instance_node;
	
	// fill flex shape arrays
	flexes_count = actor_mesh_instance->get_blend_shape_count();

	frame_editor_flex_states = new float[flexes_count];
	current_frame_flex_states = new float[flexes_count];

	for (int i = 0; i < flexes_count; i++)
	{
		frame_editor_flex_states[i] = 0.0f;
		current_frame_flex_states[i] = 0.0f;
	}

	is_loaded = true;
}

void MK_Actor::set_actor_id(int new_id)
{
	id = new_id;
}

int MK_Actor::get_flex_id_by_name(String name)
{
	return actor_mesh_instance->find_blend_shape_by_name(name);
}

String MK_Actor::get_flex_name_by_id(int flex_id)
{
	if (flex_id < 0 || flex_id > flexes_count || !actor_mesh_instance)
		return "wrong_flex_id";


	Ref<ArrayMesh> mesh = actor_mesh_instance->get_mesh();
	return mesh->get_blend_shape_name(flex_id);
}

// returns 0.0f if flex can't be accessed 
int MK_Actor::get_frame_editor_flex_state(int flex_id)
{
	if (!is_loaded || flex_id < 0 || flex_id >= flexes_count)
		return 0.0f;

	return frame_editor_flex_states[flex_id];
}

void MK_Actor::set_frame_editor_flex_state(int flex_id, float value)
{
	if (flex_id < 0 || flex_id >= flexes_count)
		return;

	frame_editor_flex_states[flex_id] = value;
}

Node* MK_Actor::get_actor_root_node()
{
	if (is_loaded)
	{
		return actor_root_node;
	}

	return nullptr;
}

void MK_Actor::apply_current_frame_blend_states(bool flex_editor)
{
	MK_SceneData* data = MK_SceneData::get_singleton();
	float current_time = MK_SceneContext::get_singleton()->get_playback_time();

	MK_Event* current_event = nullptr;
	if (event_ids.size() > 0)
	{
		current_event = data->get_event_by_id(event_ids[0]);
	}
	if (!current_event && !flex_editor)
	{
		UtilityFunctions::print("No events for actor!");
		return;
	}

	for (int i = 0; i < flexes_count; i++)
	{
		// flex states used for this frame are taken from the flex editor
		if (flex_editor)
		{
			actor_mesh_instance->set_blend_shape_value(i, frame_editor_flex_states[i]);
		}
		else
		{
			float flex_value = 0.0f;

			for (int j = 0; j < event_ids.size(); j++)
			{
				MK_Event* eve = data->get_event_by_id(event_ids[j]);
				if(eve)
				{
					//UtilityFunctions::print("Getting flex state");
					flex_value += eve->get_flex_state(i, current_time);
				}
			}

			flex_value = Math::clamp(flex_value, 0.0f, 1.0f);

			actor_mesh_instance->set_blend_shape_value(i, flex_value);
		}

	}
}

void MK_Actor::add_event(MK_Event* eve)
{
	event_ids.push_back(eve->get_event_id());
}

void MK_Actor::remove_event(int event_id)
{
	for (int i = 0; i < event_ids.size(); i++)
	{
		if (event_ids[i] == event_id)
		{
			event_ids.erase(std::next(event_ids.begin(), i));
		}
	}
}

void MK_Actor::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_actor_id"), &MK_Actor::get_actor_id);
	ClassDB::bind_method(D_METHOD("set_actor_id", "id"), &MK_Actor::set_actor_id);
	ClassDB::bind_method(D_METHOD("get_actor_name"), &MK_Actor::get_actor_name);
	ClassDB::bind_method(D_METHOD("set_actor_name", "name"), &MK_Actor::set_actor_name);
	ClassDB::bind_method(D_METHOD("get_flex_count"), &MK_Actor::get_flex_count);
	ClassDB::bind_method(D_METHOD("get_flex_id_by_name", "flex_name"), &MK_Actor::get_flex_id_by_name);
	ClassDB::bind_method(D_METHOD("get_flex_name_by_id"), &MK_Actor::get_flex_name_by_id);
	ClassDB::bind_method(D_METHOD("get_actor_root_node"), &MK_Actor::get_actor_root_node);
	ClassDB::bind_method(D_METHOD("get_frame_editor_flex_state", "flex_id"), &MK_Actor::get_frame_editor_flex_state);
	ClassDB::bind_method(D_METHOD("set_frame_editor_flex_state", "flex_id", "value"), &MK_Actor::set_frame_editor_flex_state);
	ClassDB::bind_method(D_METHOD("get_event_count"), &MK_Actor::get_event_count);
	ClassDB::bind_method(D_METHOD("get_event_id", "idx"), &MK_Actor::get_event_id);

}