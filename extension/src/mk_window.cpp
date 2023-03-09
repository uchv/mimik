#include "mk_window.h"

#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/theme.hpp>
#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/packed_scene.hpp>


MimikWindow::MimikWindow()
{
	// default size
	window_size = Vector2(400.0f, 500.0f);
}

void MimikWindow::init_window(String window_name)
{
	// MimikApplication has already set theme for us 
	Ref<Theme> theme = get_theme();

	// dark gray window handle bar
	window_bar = memnew(ColorRect);
	add_child(window_bar);
	
	window_bar->set_color(theme->get_color("window_bar", "mimik"));
	window_bar->set_anchors_and_offsets_preset(LayoutPreset::PRESET_TOP_LEFT);
	window_bar->set_position(Vector2(0.0f, 0.0f));
	window_bar->set_size(Vector2(window_size.x, window_bar_height));


	// window name in handle bar
	Label* window_label = memnew(Label);
	window_bar->add_child(window_label);

	window_label->add_theme_font_size_override("font_size", theme->get_font_size("window_title", "mimik"));
	window_label->set_text(window_name);
	window_label->set_anchors_and_offsets_preset(LayoutPreset::PRESET_CENTER_LEFT);
	window_label->set_position(Vector2(10.0f, 0.0f));
	
	
	// window body in which content will be placed
	window_body = memnew(ColorRect);
	add_child(window_body);

	window_body->set_color(theme->get_color("window_body", "mimik"));
	window_body->set_anchors_and_offsets_preset(LayoutPreset::PRESET_TOP_LEFT);

	Vector2 window_origin = window_bar->get_global_position();
	window_body->set_position(Vector2(0.0f, window_bar_height));
	window_body->set_size(Vector2(window_size.x, window_size.y - window_bar_height));


	// for window dragging
	is_clicked = false;
	latest_lmb_button_state = false;
	latest_mouse_position = get_global_mouse_position();
}

void MimikWindow::update_window()
{
	// update content first 
	update_content();

	// update window position 
	Input* input_singleton = Input::get_singleton();

	// left mouse dragging
	// no dragging logic if mouse isnt held
	if (!input_singleton->is_mouse_button_pressed(MouseButton::MOUSE_BUTTON_LEFT))
	{
		is_clicked = false;
	}
	else
	{
		// if user isnt already dragging, check if he is trying to drag 
		if (!is_clicked && !latest_lmb_button_state)
		{
			if (!latest_lmb_button_state && window_bar->get_rect().has_point(window_bar->get_local_mouse_position()))
			{
				is_clicked = true;
				is_dragged = true;
			}

		}
		else if (is_clicked && latest_lmb_button_state)
		{
			Vector2 delta = get_global_mouse_position() - latest_mouse_position;

			set_global_position(get_global_position() + delta);
		}
	}	

	latest_lmb_button_state = input_singleton->is_mouse_button_pressed(MouseButton::MOUSE_BUTTON_LEFT);
	latest_mouse_position = get_global_mouse_position();

	// now update window size. size can get changed through set_window_size()
	window_bar->set_size(Vector2(window_size.x, window_bar_height));
	window_body->set_size(Vector2(window_size.x, window_size.y - window_bar_height));
}

Vector2 MimikWindow::get_content_size()
{
	return window_body->get_size() - Vector2(content_margin * 2.0f, content_margin * 2.0f);
}


void MimikWindow::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_window_size"), &MimikWindow::get_window_size);
	ClassDB::bind_method(D_METHOD("set_window_size", "new_size"), &MimikWindow::set_window_size);
	ClassDB::bind_method(D_METHOD("get_window_name"), &MimikWindow::get_window_name);
}

Node* MimikWindow::attach_content_scene(String scene_name)
{
	ResourceLoader* resource_loader = ResourceLoader::get_singleton();

	const String res_path = "res://WindowContent/" + scene_name + ".tscn";

	Ref<PackedScene> content_scene = resource_loader->load(res_path);
	content_node = content_scene->instantiate();
	window_body->add_child(content_node);

	content_node->call("deploy_content");

	return content_node;
}
