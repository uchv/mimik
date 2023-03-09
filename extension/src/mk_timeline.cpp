#include "mk_timeline.h"

#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/font.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <string>


// virtual method. base implementation gets singletons and sets up godot state
void MK_Timeline::init_timeline()
{
	scene_context = MK_SceneContext::get_singleton();
	scene_data = MK_SceneData::get_singleton();

	this->set_process(true);
	// make sure nothing gets drawn outside of the timeline
	this->set_clip_contents(true);
}


// virtual method. base implementation does scrolling and handle updating
void MK_Timeline::update_timeline()
{
	bool curve_changed = false;
	bool timeline_changed = scene_context->does_timeline_need_redraw();


	// mouse wheel navigation
	Vector2 mouse_local = get_local_mouse_position();
	if (mouse_local.x > 0.0f && mouse_local.y > 0.0f &&	mouse_local.x < get_size().x && mouse_local.y < get_size().y)
	{
		if (Input::get_singleton()->is_action_just_released("zoom_in"))
		{
			zoom_timeline(true);
			timeline_changed = true;
		}
		else if (Input::get_singleton()->is_action_just_released("zoom_out"))
		{
			zoom_timeline(false);
			timeline_changed = true;
		}
		else if (Input::get_singleton()->is_action_just_released("pan_left"))	// is cursor on timeline
		{
			pan_timeline(false);
			timeline_changed = true;
		}
		else if (Input::get_singleton()->is_action_just_released("pan_right"))	// is cursor on timeline
		{
			pan_timeline(true);
			timeline_changed = true;
		}
	}


	// need to redraw if animation is playing (-> timepointer needs to be drawn)
	if (scene_context->is_playing() || scene_context->get_playback_time() != current_timepointer_pos)
	{
		// need to redraw for time pointer
		timeline_changed = true;
	}


	// did any handles change?
	for (int i = 0; i < point_handles.size(); i++)
	{
		if (point_handles[i]->has_handle_changed())
		{
			curve_changed = true;

			MK_CurveHandle* handle = point_handles[i];


			// if points have swapped order (x axis), rearrange
			int old_idx = handle->get_point_index();
			int new_idx = old_idx;
			if (cur_curve)
			{
				new_idx = cur_curve->modify_point(old_idx, convert_from_screen_pos(handle->get_handle_pos()));
			}


			if (old_idx != new_idx)
			{
				UtilityFunctions::print("Changing indices");
				// cycle through all handles and find the one who occupies the new index
				for (int j = 0; j < point_handles.size(); j++)
				{
					if (point_handles[j]->get_point_index() == new_idx)
					{
						point_handles[j]->set_point_index(old_idx);
						handle->set_point_index(new_idx);

						break;
					}
				}
			}
		}
	}

	if (curve_changed)
	{	
		get_active_curve()->calc_curve();
	}
	if (curve_changed || timeline_changed)
	{
		queue_redraw();
	}
}

// virtual method
void MK_Timeline::draw_timeline()
{
}




void MK_Timeline::draw_background()
{
	draw_rect(Rect2(0.0f, 0.0f, get_size().x, get_size().y), Color(0.495f, 0.495f, 0.495f));
}

void MK_Timeline::draw_timepointer()
{
	float time = scene_context->get_playback_time();

	// important for scene timeline so sidebar doesn't get covered
	if (time < left_border_time || time > right_border_time)
	{
		return;
	}

	float time_xcoord = convert_to_screen_pos(Vector2(time, 0.0f)).x;

	draw_line(Vector2(time_xcoord, 0.0f), Vector2(time_xcoord, get_size().y), Color(0.5f, 0.0f, 0.0f), 2.0f);

	current_timepointer_pos = time;
}

void MK_Timeline::draw_timescale()
{
	// simple line at y = 0
	draw_line(convert_to_screen_pos(Vector2(left_border_time, 0.0f)), convert_to_screen_pos(Vector2(right_border_time, 0.0f)), Color(0.1f, 0.1f, 0.1f), 3.0f);

	// determine the biggest possible segment and then scale down
	float range_total = right_border_time - left_border_time;
	float biggest_possible_segment = range_total / num_timestamps_max;


	// count digits for segment
	int digits = 0;
	int temp = biggest_possible_segment;
	while (temp > 1.0f)
	{
		temp /= 10.0f;
		digits += 1;
	}

	float factor = pow(10.0f, float(digits));
	float time_distance = factor;
	if (time_distance * num_timestamps_max < range_total)
	{
		time_distance *= 2.0f;
	}
	else if ((time_distance * num_timestamps_max) > range_total * 2.0f)
	{
		time_distance *= 0.5f;
	}

	float base_time = round(left_border_time / factor) * factor;


	Ref<Font> font = get_theme_font("MimikTheme");
	for (int i = 0; i < num_timestamps_max; i++)
	{
		// find pretty time spot
		float time = base_time + time_distance * i;

		if (time < left_border_time || time > right_border_time)
		{
			continue;
		}

		float time_x = convert_to_screen_pos(Vector2(time, 0.0f)).x;
		float coord_y = convert_to_screen_pos(Vector2(0.0f, 0.0f)).y;

		// time needs to be converted to godot::String
		std::string tmp = std::to_string((int)time);


		// draw
		draw_line(Vector2(time_x, coord_y + 7.0f), Vector2(time_x, coord_y - 7.0f), Color(0.0f, 0.0f, 0.0f), 2.0f);
		draw_string(font, Vector2(time_x - 4.0f, coord_y + 20.0f), String(tmp.c_str()), HORIZONTAL_ALIGNMENT_FILL, -1.0f, 12, Color(0.0f, 0.0f, 0.0f));
	}
}

void MK_Timeline::draw_xaxis(float y)
{
	draw_line(Vector2(0.0f, y), Vector2(get_size().x, y), Color(0.0f, 0.0f, 0.0f), 3.0f);
}



void MK_Timeline::draw_y_axes()
{
	float zero_x = convert_to_screen_pos(Vector2(0.0f, 0.0f)).x;
	
	if (0.0f >= left_border_time && 0.0f <= right_border_time)
	{
		draw_line(Vector2(zero_x, 0.0f), Vector2(zero_x, get_size().y), Color(0.0f, 0.0f, 0.0f), 3.0f);
	}
}

void MK_Timeline::draw_curve(MK_CurveAnimation* curve, float event_time) 
{
	PackedVector2Array points = curve->get_tesselated_points();
	if (points.size() < 2)
	{
		return;
	} 

	for (int i = 0; i < points.size() - 1; i++)
	{
		float sample_1 = points[i + 1].x + event_time;
		float sample_2 = points[i].x + event_time;
		// no need to draw outside of viewable area
		if (sample_1 < left_border_time || sample_2 > right_border_time)
		{
			continue;
		}
		
		draw_line(convert_to_screen_pos(Vector2(sample_2, CLAMP(points[i].y, 0.0f, 1.0f))), 
			convert_to_screen_pos(Vector2(sample_1, CLAMP(points[i + 1].y, 0.0f, 1.0f))), Color(1.0f, 0.255f, 0.255f), 2.0f);
	}
}


void MK_Timeline::draw_container_box(float start_time, float end_time, int line, String box_name)
{
	const float outline_width = 3.0f;
	const Vector2 outline_offset = Vector2(outline_width, outline_width);
	const Color box_color = Color(0.8359375, 0.21484375, 0.16015625);
	const Color box_color_outline = Color(0.6484375, 0.171875, 0.12890625);

	// top left coord of rect
	Vector2 pos1 = convert_to_screen_pos(Vector2(start_time, line));
	Vector2 pos2 = convert_to_screen_pos(Vector2(end_time, line + 1));

	draw_rect(Rect2(Point2(pos1), Size2(pos2 - pos1)), box_color_outline);
	draw_rect(Rect2(Point2(pos1 + outline_offset), Size2(pos2 - pos1 - 2.0f * outline_offset)), box_color);

	Ref<Font> font = get_theme_font("MimikTheme");
	Vector2 text_pos = pos1 + Vector2(4.0, 15.0);
	
	draw_string(font, text_pos, "Fuck this event", HORIZONTAL_ALIGNMENT_LEFT, pos2.x - pos1.x - outline_width, 12);
}


void MK_Timeline::add_curve_point(Vector2 pos)
{
	if (!cur_curve)
		return;

	UtilityFunctions::print("Add new curve point");
	cur_curve->add_point(convert_from_screen_pos(pos));
	cur_curve->calc_curve();
	setup_point_handles();
	
	queue_redraw();
}


void MK_Timeline::setup_point_handles()
{
	if (!cur_curve)
		return;
	UtilityFunctions::print("Setup point handles");

	// and create handle(s)!
	PackedVector2Array points = cur_curve->get_main_points();

	int old_handle_count = point_handles.size();
	point_handles.resize(points.size());

	/* @TODO
	// delete unneeded handles
	if (points.size() < old_handle_count)
	{
		for (int i = point_handles.size(); i > points.size(); i++)
		{
			memdelete(point_handles[i]);
		}
	}
*/

	for (int i = 0; i < points.size(); i++)
	{
		MK_CurveHandle* handle;
		if (i < old_handle_count)
		{
			handle = point_handles[i];
		}
		else
		{
			handle = memnew(MK_CurveHandle);
			add_child(handle);	
			// full rect so that points can be within timeline bounds
			handle->set_anchors_and_offsets_preset(PRESET_FULL_RECT);
		}


		handle->set_point_index(i);
		handle->set_handle_pos(convert_to_screen_pos(points[i]), false);

		point_handles[i] = handle;
	}
	UtilityFunctions::print("Finished Setup point handles");
}

void MK_Timeline::update_point_handles_pos()
{
	if (!cur_curve)
	{
		return;
	}

	for (int i = 0; i < point_handles.size(); i++)
	{
		Vector2 new_pos = cur_curve->get_main_point_at_index(point_handles[i]->get_point_index());
		point_handles[i]->set_handle_pos(convert_to_screen_pos(new_pos), false);
	}
}

// virtual function
Vector2 MK_Timeline::convert_to_screen_pos(Vector2 pos_native)
{
	return Vector2();
}

// virtual function
Vector2 MK_Timeline::convert_from_screen_pos(Vector2 pos_screen)
{
	return Vector2();
}


MK_CurveAnimation* MK_Timeline::get_active_curve()
{
	return cur_curve;
}

void MK_Timeline::set_active_curve(MK_CurveAnimation* curve)
{
	if (!curve)
	{
		UtilityFunctions::print("Invalid curve!");
	}

	UtilityFunctions::print("Set active curve");

	cur_curve = curve;
	setup_point_handles();
	
	queue_redraw();
}

void MK_Timeline::zoom_timeline(bool in)
{
	// don't zoom in too far
	if (in && right_border_time - left_border_time < 0.5f)
	{
		return;
	}

	const float zoom_speed = 5.0f; 
	float factor = in ? 1.0f : -1.0f;

	// this zoom doesn't just zoom in the middle, it follows wherever the mouse cursor is
	float direction = get_local_mouse_position().x / get_size().x;

	// change the border times
	left_border_time = left_border_time + zoom_speed * direction * factor;
	right_border_time = right_border_time - zoom_speed * (1.0f - direction) * factor;

	update_point_handles_pos();
	emit_signal("scroll_timeline");
}

void MK_Timeline::pan_timeline(bool right)
{
	const float pan_speed = (right_border_time - left_border_time) * 0.05f;
	float factor = right ? 1.0f : -1.0f;

	left_border_time += pan_speed * factor;
	right_border_time += pan_speed * factor;

	update_point_handles_pos();
	emit_signal("scroll_timeline");
}

void MK_Timeline::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("draw_timescale"), &MK_Timeline::draw_timescale);
	ClassDB::bind_method(D_METHOD("draw_xaxis", "y"), &MK_Timeline::draw_xaxis);
	ClassDB::bind_method(D_METHOD("draw_y_axes"), &MK_Timeline::draw_y_axes);
	ClassDB::bind_method(D_METHOD("draw_container_box", "start_time", "end_time", "box_name"), &MK_Timeline::draw_container_box);
	ClassDB::bind_method(D_METHOD("get_active_curve"), &MK_Timeline::get_active_curve);
	ClassDB::bind_method(D_METHOD("add_curve_point", "pos"), &MK_Timeline::add_curve_point);
	ClassDB::bind_method(D_METHOD("setup_point_handles"), &MK_Timeline::setup_point_handles);
	ClassDB::bind_method(D_METHOD("convert_from_screen_pos", "screen_pos"), &MK_Timeline::convert_from_screen_pos);
	ClassDB::bind_method(D_METHOD("convert_to_screen_pos", "native_pos"), &MK_Timeline::convert_to_screen_pos);

	ADD_SIGNAL(MethodInfo("scroll_timeline"));

}
