#include "mk_event.h"

#include "mk_scenedata.h"

#include <godot_cpp/variant/utility_functions.hpp>

MK_Event::MK_Event()
{

}

MK_Event::~MK_Event()
{
}

void MK_Event::set_actor_id(int a_id)
{
	actor_id = a_id;

	// now setup animation storage
	if (animation_curves.size() > 0)
	{
		for (int i = 0; i < animation_curves.size(); i++)
		{
			memdelete(animation_curves[i]);
		}
	}
	UtilityFunctions::print("Setting up curves");

	MK_Actor* actor = MK_SceneData::get_singleton()->get_actor_by_id(actor_id);

	animation_curves.resize(actor->get_flex_count());

	
	for (int i = 0; i < actor->get_flex_count(); i++)
	{
		MK_CurveAnimation* curve = memnew(MK_CurveAnimation);
		curve->add_point(Vector2(start_time, 0.0f));
		curve->add_point(Vector2(end_time, 0.0f));
		curve->calc_curve();

		animation_curves[i] = curve;
	}
}


// sets event time. no need to change curves as they always start at 0
void MK_Event::set_event_time(float new_time)
{
	float length = end_time - start_time;

	start_time = new_time;
	end_time = new_time + length;
}


void MK_Event::set_event_length(float new_length, bool change_back)
{
	float old_length = end_time - start_time;
	float offset = new_length - old_length;

	// update curves
	// curves don't get trimmed. AnimationTimeline displays where the event cuts off the animation
	// if !change_back, move the curve points 

	if (change_back)
	{
		end_time += offset;
	}
	else
	{
		start_time -= offset;

		// move curves
		for (int i = 0; i < animation_curves.size(); i++)
		{
			MK_CurveAnimation* curve = animation_curves[i];
			if (curve)
			{
				curve->move_curve_timing(offset);
			}
		}
	}
}

float MK_Event::get_flex_state(int flex_id, float time)
{
	if (flex_id < 0 || flex_id >= animation_curves.size())
	{
		return 0.0f;
	}

	// curve time is from zero to x.
	float curve_time = Math::clamp(time, start_time, end_time) - start_time;

	return animation_curves[flex_id]->get_y_value_from_x(curve_time);
}

void MK_Event::set_flex_state(int flex_id, float time, float state)
{
	if (flex_id < 0 || flex_id >= animation_curves.size())
	{
		return;
	}

	animation_curves[flex_id]->add_point(Vector2(time - start_time, state));
	animation_curves[flex_id]->calc_curve();
}

MK_CurveAnimation* MK_Event::get_animation_for_flex(int flex_id)
{
	if (flex_id < 0 || flex_id >= animation_curves.size())
	{
		return nullptr;
	}

	return animation_curves[flex_id];
}

void MK_Event::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_start_time"), &MK_Event::get_start_time);
	ClassDB::bind_method(D_METHOD("get_end_time"), &MK_Event::get_end_time);

	ClassDB::bind_method(D_METHOD("get_event_id"), &MK_Event::get_event_id);

	ClassDB::bind_method(D_METHOD("get_event_type"), &MK_Event::get_event_type);
	ClassDB::bind_method(D_METHOD("set_event_type", "type"), &MK_Event::set_event_type);

	ClassDB::bind_method(D_METHOD("get_event_name"), &MK_Event::get_event_name);
	ClassDB::bind_method(D_METHOD("set_event_name", "name"), &MK_Event::set_event_name);

	ClassDB::bind_method(D_METHOD("set_event_time", "new_time"), &MK_Event::set_event_time);
	ClassDB::bind_method(D_METHOD("set_event_length", "new_length", "right"), &MK_Event::set_event_length);
	
	ClassDB::bind_method(D_METHOD("get_flex_state", "flex_id", "time"), &MK_Event::get_flex_state);
	ClassDB::bind_method(D_METHOD("set_flex_state", "flex_id", "time", "state"), &MK_Event::set_flex_state);
}