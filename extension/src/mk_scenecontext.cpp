#include "mk_scenecontext.h"

#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/scene_tree.hpp>

#include "mk_scenedata.h"


MK_SceneContext* MK_SceneContext::singleton = nullptr;

MK_SceneContext::MK_SceneContext()
{
	singleton = this;
}

MK_SceneContext* MK_SceneContext::get_singleton()
{
	return singleton;
}

void MK_SceneContext::update_context()
{
	if (redraw_timeline_done)
	{
		redraw_timeline_done = false;
		redraw_timeline = false;
	}

	if (is_currently_playing && timer_node)
	{
		// @TODO: if time is going over range, reset!
		current_time += timer_node->get_process_delta_time();
	}
}


void MK_SceneContext::playback_play()
{
	is_currently_playing = true;
}


void MK_SceneContext::playback_pause()
{
	is_currently_playing = !is_currently_playing;
}


void MK_SceneContext::playback_stop()
{
	current_time = 0.0f;
	is_currently_playing = false;

	// @TODO: still need to update timelines after this, otherwise time pointer just stays there
}

void MK_SceneContext::set_playback_time(float new_time)
{
	if (new_time > 0)
	{
		current_time = new_time;
	}
	else
	{
		current_time = 0.0f;
	}
}

bool MK_SceneContext::does_timeline_need_redraw()
{
	redraw_timeline_done = redraw_timeline ? true : false;

	return redraw_timeline; 
}


void MK_SceneContext::set_focused_actor(int actor_id)
{
	focused_actor_id = actor_id;

	MK_SceneData* data = MK_SceneData::get_singleton();
	MK_Actor* actor = data->get_actor_by_id(focused_actor_id);

	if (actor && actor->get_event_count() > 0)
	{
		set_focused_event(actor->get_event_id(0));
	}
}

void MK_SceneContext::set_focused_event(int event_id)
{
	MK_SceneData* data = MK_SceneData::get_singleton();
	MK_Actor* actor = data->get_actor_by_id(focused_actor_id);

	if (actor)
	{
		for (int i = 0; i < actor->get_event_count(); i++)
		{
			if (actor->get_event_id(i) == event_id)
			{
				focused_event_id = event_id;
				UtilityFunctions::print("Successfully focused event");
				return;
			}
		}
	}
}

void MK_SceneContext::select_flex(int flex_id)
{
	selected_flexes.push_back(flex_id);
}

void MK_SceneContext::deselect_flex(int flex_id)
{
	for (int i = 0; i < selected_flexes.size(); i++)
	{
		if (selected_flexes[i] == flex_id)
		{
			selected_flexes.erase(std::next(selected_flexes.begin(), i));
			break;
		}
	}
}

void MK_SceneContext::deselect_all_flexes()
{
	selected_flexes.clear();
}

bool MK_SceneContext::is_flex_selected(int flex_id)
{
	for (int i = 0; i < selected_flexes.size(); i++)
	{
		if (selected_flexes[i] == flex_id)
		{
			return true;
		}
	}
	return false;
}

PackedInt32Array MK_SceneContext::get_selected_flexes()
{
	PackedInt32Array flexes;

	for (int i = 0; i < selected_flexes.size(); i++)
	{
		flexes.append(selected_flexes[i]);
	}
	
	return flexes;
}



void MK_SceneContext::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("playback_play"), &MK_SceneContext::playback_play);
	ClassDB::bind_method(D_METHOD("playback_pause"), &MK_SceneContext::playback_pause);
	ClassDB::bind_method(D_METHOD("playback_stop"), &MK_SceneContext::playback_stop);
	ClassDB::bind_method(D_METHOD("is_playing"), &MK_SceneContext::is_playing);
	ClassDB::bind_method(D_METHOD("get_playback_time"), &MK_SceneContext::get_playback_time);
	ClassDB::bind_method(D_METHOD("set_playback_time", "new_time"), &MK_SceneContext::set_playback_time);

	ClassDB::bind_method(D_METHOD("queue_timeline_redraw"), &MK_SceneContext::queue_timeline_redraw);
	ClassDB::bind_method(D_METHOD("does_timeline_need_redraw"), &MK_SceneContext::does_timeline_need_redraw);

	ClassDB::bind_method(D_METHOD("get_focused_actor_id"), &MK_SceneContext::get_focused_actor_id);
	ClassDB::bind_method(D_METHOD("set_focused_actor"), &MK_SceneContext::set_focused_actor);

	ClassDB::bind_method(D_METHOD("get_focused_event_id"), &MK_SceneContext::get_focused_event_id);
	ClassDB::bind_method(D_METHOD("set_focused_event", "id"), &MK_SceneContext::set_focused_event);

	ClassDB::bind_method(D_METHOD("select_flex", "flex_id"), &MK_SceneContext::select_flex);
	ClassDB::bind_method(D_METHOD("deselect_flex", "flex_id"), &MK_SceneContext::deselect_flex);
	ClassDB::bind_method(D_METHOD("deselect_all_flexes"), &MK_SceneContext::deselect_all_flexes);
	ClassDB::bind_method(D_METHOD("is_flex_selected", "flex_id"), &MK_SceneContext::is_flex_selected);
	ClassDB::bind_method(D_METHOD("get_selected_flexes"), &MK_SceneContext::get_selected_flexes);
}
