#include "mk_curvehandle.h"

#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

MK_CurveHandle::MK_CurveHandle()
{
}

void MK_CurveHandle::set_handle_pos(Vector2 new_pos, bool clamp_position)
{
	Vector2 new_position;
	if (clamp_position)
	{
		new_position = new_pos.clamp(Vector2(0.0f, 0.0f), Vector2(get_size().x, get_size().y));
	}
	else
	{
		new_position = new_pos;
	}

	point_position = new_position;
	
	queue_redraw();
}


void MK_CurveHandle::_notification(int p_what)
{
	switch (p_what)
	{
		case NOTIFICATION_READY:
			this->set_process(true);
			break;
		case NOTIFICATION_PROCESS:
		{
			// new frame, new state
			has_changed = false;
			
			latest_mouse_position = get_global_mouse_position();

			const float click_radius = 30.0f;
			is_hovered = (point_position.distance_to(get_local_mouse_position()) < click_radius) ? true : false;


			if (Input::get_singleton()->is_mouse_button_pressed(MOUSE_BUTTON_LEFT))
			{
				if ((!was_mouse_down && is_hovered) || is_dragged)
				{
					is_dragged = true;
					//is_selected = true;
					has_changed = true;
					set_handle_pos(get_local_mouse_position(), true);
				}
				was_mouse_down = true;
			}
			else
			{
				is_dragged = false;
				was_mouse_down = false;
			}

			break;
		}
		case NOTIFICATION_DRAW:
		{
			//const float circle_radius = is_selected ? 5.0f : 3.0f;
			const float circle_radius = 4.0f;
			draw_circle(point_position, circle_radius, Color(0.0f, 0.0f, 0.0f));
			break;
		}
	}
}

void MK_CurveHandle::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_handle_pos"), &MK_CurveHandle::get_handle_pos);
	ClassDB::bind_method(D_METHOD("set_handle_pos", "new_position", "clamp_position"), &MK_CurveHandle::set_handle_pos);
	//ClassDB::bind_method(D_METHOD("set_selected", "selected"), &MK_CurveHandle::set_selected);
	ClassDB::bind_method(D_METHOD("get_point_index"), &MK_CurveHandle::get_point_index);
	ClassDB::bind_method(D_METHOD("set_point_index", "idx"), &MK_CurveHandle::set_point_index);
}
