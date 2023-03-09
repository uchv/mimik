#include "mk_animationtimeline.h"

#include "mk_scenedata.h"

#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/variant/utility_functions.hpp>


MK_AnimationTimeline::MK_AnimationTimeline()
{
}

void MK_AnimationTimeline::init_timeline()
{
	// call base
	MK_Timeline::init_timeline();

}

void MK_AnimationTimeline::process_timeline()
{
	// call base
	MK_Timeline::update_timeline();


	// check for curve
	PackedInt32Array flexes = scene_context->get_selected_flexes();
	if (flexes.size() > 0)
	{
		int flex = flexes[flexes.size() - 1];
		
		if (cur_curve_flex_id != flex)
		{
			cur_curve_flex_id = flex;

			MK_Event* cur_event = scene_data->get_event_by_id(scene_context->get_focused_event_id());
			if (cur_event)
			{
				MK_CurveAnimation* new_curve = cur_event->get_animation_for_flex(flex);

				if (new_curve)
				{
					UtilityFunctions::print("Set new curve");
					set_active_curve(new_curve);
				}
			}
		}
	}
}

void MK_AnimationTimeline::draw_timeline()
{
	draw_background();

	// y axis at beginning and end of frame
	draw_y_axes();
	draw_xaxis(convert_to_screen_pos(Vector2(0.0f, 1.0f)).y);
	draw_timescale();

	// curve
	MK_CurveAnimation* curve = get_active_curve();
	if (curve)
	{
		// in the animation timeline, display without any event time offset -> 0.0f as event_time
		draw_curve(curve, 0.0f);
	}

	draw_timepointer();
}


// draws vertical line where the event begins and where it ends.
void MK_AnimationTimeline::draw_y_axes()
{
	MK_Event* cur_event = scene_data->get_event_by_id(scene_context->get_focused_event_id());

	if (!cur_event)
	{
		return;
	}

	float event_length = cur_event->get_end_time() - cur_event->get_start_time();

	float first_x = convert_to_screen_pos(Vector2(0.0f, 0.0f)).x;
	float final_x = convert_to_screen_pos(Vector2(event_length, 0.0f)).x;

	if (0.0f >= left_border_time && 0.0f <= right_border_time)
	{
		draw_line(Vector2(first_x, 0.0f), Vector2(first_x, get_size().y), Color(0.0f, 0.0f, 0.0f), 3.0f);
	}
	if (event_length >= left_border_time && event_length <= right_border_time)
	{
		draw_line(Vector2(final_x, 0.0f), Vector2(final_x, get_size().y), Color(0.0f, 0.0f, 0.0f), 3.0f);
	}
}


Vector2 MK_AnimationTimeline::convert_to_screen_pos(Vector2 pos_native)
{
	const float pixels_per_second = get_size().x / (right_border_time - left_border_time);
	const float pixels_per_amp = get_size().y / (top_border_amp - bottom_border_amp);

	Vector2 final_position;
	final_position.x = (pos_native.x - left_border_time) * pixels_per_second;

	final_position.y = (pos_native.y - top_border_amp) * -pixels_per_amp;

	return final_position;
}

Vector2 MK_AnimationTimeline::convert_from_screen_pos(Vector2 pos_screen)
{
	const float pixels_per_second = get_size().x / (right_border_time - left_border_time);
	const float pixels_per_amp = get_size().y / (top_border_amp - bottom_border_amp);

	Vector2 final_position;
	final_position.x = left_border_time + pos_screen.x / pixels_per_second;
	final_position.y = (pos_screen.y - top_border_amp * pixels_per_amp) / -pixels_per_amp;

	return final_position;
}

void MK_AnimationTimeline::_notification(int p_what)
{
	switch (p_what)
	{
		case NOTIFICATION_READY:
			init_timeline();
			break;
		case NOTIFICATION_PROCESS:
			process_timeline();
			break;
		case NOTIFICATION_DRAW:
			draw_timeline();
			break;
	}
}

void MK_AnimationTimeline::_bind_methods()
{
}
