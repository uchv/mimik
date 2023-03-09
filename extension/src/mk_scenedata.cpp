#include "mk_scenedata.h"

#include "mk_scenecontext.h"

#include <godot_cpp/variant/utility_functions.hpp>


MK_SceneData* MK_SceneData::singleton = nullptr;

MK_SceneData::MK_SceneData()
{
	singleton = this;

	// pray to god scene context already exists by this point
	scene_context = MK_SceneContext::get_singleton();
}

MK_SceneData* MK_SceneData::get_singleton()
{
	return singleton;
}

void MK_SceneData::update_data()
{
	make_current_frame_blend_states();
}



int MK_SceneData::add_actor(String model_path)
{
	MK_Actor* new_actor = memnew(MK_Actor);
	new_actor->load_actor(model_path);

	if (!new_actor->is_actor_loaded())
	{
		UtilityFunctions::print("Failed to load actor. Abort.");
		return -1;
	}

	int actor_id = array_actors.size();
	new_actor->set_actor_id(actor_id);
	UtilityFunctions::print("Done setting Actor ID");

	array_actors.push_back(new_actor);

	// notify scenecontext as new actors get automatically focused
	MK_SceneContext::get_singleton()->set_focused_actor(actor_id);

	return actor_id;
}

int MK_SceneData::add_event(int actor_id, int type)
{
	MK_Actor* actor = get_actor_by_id(actor_id);
	if (!actor)
	{
		return -1;
	}

	MK_Event* new_event = memnew(MK_Event);
	new_event->set_actor_id(actor_id);
	new_event->set_event_type(type);

	int new_id;
	if (array_events.size() == 0)
	{
		new_id = 0;
		new_event->set_event_id(new_id);
		array_events.push_back(new_event);
	}
	else
	{
		int last_id = array_events[array_events.size() - 1]->get_event_id();
		new_id = last_id + 1;

		new_event->set_event_id(new_id);
		array_events.push_back(new_event);
	}

	actor->add_event(new_event);

	MK_SceneContext* context = MK_SceneContext::get_singleton();

	context->set_focused_actor(actor->get_actor_id());
	context->set_focused_event(new_id);
	
	return new_id;
}

void MK_SceneData::remove_event(int event_id)
{
	for (int i = 0; i < array_events.size(); i++)
	{
		MK_Event* eve = array_events[i];
		if (eve && eve->get_event_id() == event_id)
		{
			int a_id = eve->get_actor_id();
			
			MK_Actor* actor = get_actor_by_id(a_id);
			if (actor)
			{
				actor->remove_event(event_id);
			}

			memfree(eve);
		}
	}
}

MK_Actor* MK_SceneData::get_actor_by_id(int actor_id)
{
	if (actor_id < 0 || actor_id >= array_actors.size())
		return nullptr;
	
	return Object::cast_to<MK_Actor>(array_actors[actor_id]);
}

MK_Event* MK_SceneData::get_event_by_id(int event_id)
{
	for (int i = 0; i < array_events.size(); i++)
	{
		MK_Event* eve = array_events[i];
		if (eve && eve->get_event_id() == event_id)
		{
			return eve;
		}
	}

	return nullptr;
}

Vector2i MK_SceneData::get_event_start_end(int event_id)
{
	return Vector2i();
}

PackedFloat32Array MK_SceneData::get_current_frame_blend_states(int actor_id)
{

	return PackedFloat32Array();
}

// @TODO: should this be done in MK_SceneData?
void MK_SceneData::make_current_frame_blend_states()
{
	for (int i = 0; i < array_actors.size(); i++)
	{
		MK_Actor* actor = Object::cast_to<MK_Actor>(array_actors[i]);
		if (actor)
		{
			bool use_flex_editor_state = !scene_context->is_playing();
			actor->apply_current_frame_blend_states(use_flex_editor_state);
		}
	}
}


void MK_SceneData::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_scene_length_seconds"), &MK_SceneData::get_scene_length_seconds);
	ClassDB::bind_method(D_METHOD("get_scene_length_frames"), &MK_SceneData::get_scene_length_frames);
	ClassDB::bind_method(D_METHOD("get_event_start_end", "event_id"), &MK_SceneData::get_event_start_end);
	
	ClassDB::bind_method(D_METHOD("add_actor", "model_path"), &MK_SceneData::add_actor);
	ClassDB::bind_method(D_METHOD("get_actor_count"), &MK_SceneData::get_actor_count);
	ClassDB::bind_method(D_METHOD("get_event_count"), &MK_SceneData::get_event_count);
	ClassDB::bind_method(D_METHOD("get_actor_by_id", "actor_id"), &MK_SceneData::get_actor_by_id);

	ClassDB::bind_method(D_METHOD("add_event", "actor_id"), &MK_SceneData::add_event);
	ClassDB::bind_method(D_METHOD("get_event_by_id", "event_id"), &MK_SceneData::get_event_by_id);

	ClassDB::bind_method(D_METHOD("get_current_frame_blend_states", "actor_id"), &MK_SceneData::get_current_frame_blend_states);
}
