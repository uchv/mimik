#include "mk_flexslider.h"

#include <godot_cpp/classes/h_box_container.hpp>
#include <godot_cpp/classes/margin_container.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/variant/callable.hpp>

MK_FlexSlider::MK_FlexSlider()
{
	// automatically mouse filter is set to stop on slider and checkbox, meaning mouse button events get passed on only when label is clicked
	// -> intended behavior as we want to select the flex when the label is clicked.
	this->set_mouse_filter(MOUSE_FILTER_PASS);

	// set control rect size and create hboxcontainer filling it out
	set_custom_minimum_size(Vector2(200.0f, 30.0f));


	// right margin so slider doesnt touch right window edge
	MarginContainer* margin_container = memnew(MarginContainer);
	margin_container->set_anchors_and_offsets_preset(PRESET_FULL_RECT);
	margin_container->add_theme_constant_override("margin_right", 5);


	HBoxContainer* hbox_container= memnew(HBoxContainer);
	hbox_container->set_anchors_and_offsets_preset(PRESET_FULL_RECT);

	
	active_check = memnew(CheckBox);
	active_check->set_toggle_mode(is_active);
	active_check->set_h_size_flags(SIZE_SHRINK_BEGIN);
	active_check->connect("toggled", Callable(this, "set_active"));


	name_label = memnew(Label);
	name_label->set_h_size_flags(SIZE_EXPAND_FILL);
	name_label->set_clip_text(true);


	slider = memnew(HSlider);
	slider->set_custom_minimum_size(Vector2(120.0f, 0.0f));
	slider->set_h_size_flags(SIZE_SHRINK_END);
	slider->set_v_size_flags(SIZE_SHRINK_CENTER);
	slider->set_min(0.0f);
	slider->set_max(1.0f);
	slider->set_step(0.001f);
	slider->set_value(1.0f);
	slider->connect("value_changed", Callable(this, "set_flex_value"));


	add_child(margin_container);
	margin_container->add_child(hbox_container);
	hbox_container->add_child(active_check);
	hbox_container->add_child(name_label);
	hbox_container->add_child(slider);
}


void MK_FlexSlider::set_flex_id(int new_id)
{
	flex_id = new_id;
}

void MK_FlexSlider::set_flex_name(String new_name)
{
	flex_name = new_name;
	name_label->set_text(new_name);
}

void MK_FlexSlider::set_flex_value(float new_value)
{
	// clamp first
	new_value = UtilityFunctions::clamp(new_value, 0.0f, 1.0f);
	
	flex_value = new_value;
	slider->set_value(new_value);
}

// currently not used
void MK_FlexSlider::set_active(bool active)
{
	is_active = active;
	active_check->set_toggle_mode(active);
}

float MK_FlexSlider::get_flex_value()
{
	return flex_value;

	if (is_active)
	{
		return flex_value;
	}
	else
	{
		return 0.0f;
	}
}


void MK_FlexSlider::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_flex_id"), &MK_FlexSlider::get_flex_id);
	ClassDB::bind_method(D_METHOD("get_flex_name"), &MK_FlexSlider::get_flex_name);
	ClassDB::bind_method(D_METHOD("get_flex_value"), &MK_FlexSlider::get_flex_value);
	ClassDB::bind_method(D_METHOD("get_active"), &MK_FlexSlider::get_active);
	
	ClassDB::bind_method(D_METHOD("set_flex_id", "new_id"), &MK_FlexSlider::set_flex_id);
	ClassDB::bind_method(D_METHOD("set_flex_name", "new_name"), &MK_FlexSlider::set_flex_name);
	ClassDB::bind_method(D_METHOD("set_flex_value", "new_value"), &MK_FlexSlider::set_flex_value);
	ClassDB::bind_method(D_METHOD("set_active", "active"), &MK_FlexSlider::set_active);
}
