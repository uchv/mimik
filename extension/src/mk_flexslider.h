#ifndef MIMIK_FLEXSLIDER
#define MIMIK_FLEXSLIDER

#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/classes/check_box.hpp>
#include <godot_cpp/classes/h_slider.hpp>
#include <godot_cpp/classes/input_event.hpp>

using namespace godot;


class MK_FlexSlider : public Control
{
	GDCLASS(MK_FlexSlider, Control);

public:
	MK_FlexSlider();

	void set_flex_id(int new_id);
	void set_flex_name(String new_name);
	void set_flex_value(float new_value);
	void set_active(bool active);

	int get_flex_id() { return flex_id; }
	String get_flex_name() { return flex_name; }
	float get_flex_value();
	bool get_active() { return is_active; }
	

private:
	int flex_id = -1;
	String flex_name = "";
	float flex_value = 0.0f;
	bool is_active = true;

	// godot nodes
	CheckBox* active_check = nullptr;
	Label* name_label = nullptr;
	HSlider* slider = nullptr;


protected:
	bool mouse_was_clicked = false;
	bool is_clicked = false;

	static void _bind_methods();
};

#endif // MIMIK_FLEXSLIDER