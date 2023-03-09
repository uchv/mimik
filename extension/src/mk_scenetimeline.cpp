#include "mk_scenetimeline.h"

#include <godot_cpp/classes/font.hpp>
#include <godot_cpp/variant/utility_functions.hpp>


MK_SceneTimeline::MK_SceneTimeline()
{
}


// X --> SECONDS
// Y --> 1 unit = 1 line
Vector2 MK_SceneTimeline::convert_to_screen_pos(Vector2 pos_native)
{
	const float pixels_per_second = (get_size().x - sidebar_width) / (right_border_time - left_border_time);
	// @TODO: hardcoded
	const float pixels_per_line = get_size().y / 5.0f;

	Vector2 final_position;
	final_position.x = sidebar_width + (pos_native.x - left_border_time) * pixels_per_second;
	final_position.y = pos_native.y * pixels_per_line;

	return final_position;
}


Vector2 MK_SceneTimeline::convert_from_screen_pos(Vector2 pos_screen)
{
	const float pixels_per_second = (get_size().x - sidebar_width) / (right_border_time - left_border_time);
	// @TODO: hardcoded
	const float pixels_per_line = get_size().y / 5.0f;
	
	Vector2 final_position;
	final_position.x = left_border_time + (pos_screen.x - sidebar_width) / pixels_per_second;
	final_position.y = pos_screen.y / pixels_per_line;

	return final_position;
}

void MK_SceneTimeline::init_timeline()
{
	// call base
	MK_Timeline::init_timeline();
}

void MK_SceneTimeline::process_timeline()
{
	// call base
	MK_Timeline::update_timeline();
}

void MK_SceneTimeline::draw_timeline()
{
	draw_background();

	call("draw_scenetimeline");

	draw_y_axes();
	draw_timescale();
}


void MK_SceneTimeline::draw_sidebar_part(int line_start, int line_length, String actor_name)
{
	for (int i = line_start; i < line_start + line_length; i++)
	{
		Color rect_color = i % 2 == 0 ? Color(0.7f, 0.7f, 0.7f) : Color(0.8f, 0.8f, 0.8f);
		
		float top_y = convert_to_screen_pos(Vector2(0.0f, i)).y;
		float bottom_y = convert_to_screen_pos(Vector2(0.0f, i + 1)).y;
		
		draw_rect(Rect2(Point2(0.0f, top_y), Size2(sidebar_width, bottom_y - top_y)), rect_color);
	}
	
	Ref<Font> font = get_theme_font("MimikTheme");
	const float font_height = 16.0f;
	draw_string(font, Vector2(10.0f, convert_to_screen_pos(Vector2(0.0f, line_start)).y + font_height + 5.0f), actor_name, HORIZONTAL_ALIGNMENT_LEFT, -1.0, (int) font_height, Color(0.0f, 0.0f, 0.0f));

}

void MK_SceneTimeline::_notification(int p_what)
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

void MK_SceneTimeline::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("draw_sidebar_part", "line_start", "line_length", "actor_name"), &MK_SceneTimeline::draw_sidebar_part);
	ClassDB::bind_method(D_METHOD("get_sidebar_width"), &MK_SceneTimeline::get_sidebar_width);
}
