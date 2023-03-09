#include "mk_phonemetimeline.h"

#include <godot_cpp/classes/font.hpp>
#include <godot_cpp/variant/utility_functions.hpp>


MK_PhonemeTimeline::MK_PhonemeTimeline()
{
	left_border_time = -5.0f;
	right_border_time = 10.0f;
}


// X --> SECONDS
// Y --> 0 to 1
Vector2 MK_PhonemeTimeline::convert_to_screen_pos(Vector2 pos_native)
{
	const float pixels_per_second = get_size().x / (right_border_time - left_border_time);

	Vector2 final_position;
	final_position.x = (pos_native.x - left_border_time) * pixels_per_second;
	final_position.y = pos_native.y * get_size().y;

	return final_position;
}


Vector2 MK_PhonemeTimeline::convert_from_screen_pos(Vector2 pos_screen)
{
	const float pixels_per_second = get_size().x / (right_border_time - left_border_time);

	Vector2 final_position;
	final_position.x = left_border_time + pos_screen.x / pixels_per_second;
	final_position.y = pos_screen.y / get_size().y;

	return final_position;
}

void MK_PhonemeTimeline::set_current_lipsync_event(int event_id)
{
	MK_Event* new_event = scene_data->get_event_by_id(event_id);
	if (new_event)
	{
		if (new_event->get_event_type() == 1)
		{
			current_event = new_event;
			return;
		}
	}

	current_event = nullptr;
}


void MK_PhonemeTimeline::init_timeline()
{
	// call base
	MK_Timeline::init_timeline();
}


void MK_PhonemeTimeline::process_timeline()
{
	// call base
	MK_Timeline::update_timeline();
}


void MK_PhonemeTimeline::draw_timeline()
{
	draw_background();

	draw_y_axes();
	draw_timescale();
}

void MK_PhonemeTimeline::draw_y_axes()
{
	float zero_x = convert_to_screen_pos(Vector2(0.0f, 0.0f)).x;

	if (0.0f >= left_border_time && 0.0f <= right_border_time)
	{
		draw_line(Vector2(zero_x, 0.0f), Vector2(zero_x, get_size().y), Color(0.0f, 0.0f, 0.0f), 3.0f);
	}

	if (current_event)
	{
		// now also draw the end
		float event_length = current_event->get_end_time() - current_event->get_start_time();

		if (event_length >= left_border_time && event_length <= right_border_time)
		{
			float final_x = convert_to_screen_pos(Vector2(event_length, 0.0f)).x;
			draw_line(Vector2(final_x, 0.0f), Vector2(final_x, get_size().y), Color(0.0f, 0.0f, 0.0f), 3.0f);
		}
	}
}


void MK_PhonemeTimeline::_notification(int p_what)
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

void MK_PhonemeTimeline::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("convert_to_screen_pos", "position_native"), &MK_PhonemeTimeline::convert_to_screen_pos);
	ClassDB::bind_method(D_METHOD("convert_to_screen_pos", "position_native"), &MK_PhonemeTimeline::convert_to_screen_pos);
	ClassDB::bind_method(D_METHOD("set_current_lipsync_event", "event_id"), &MK_PhonemeTimeline::set_current_lipsync_event);
}
