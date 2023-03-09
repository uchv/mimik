// GUI window
#ifndef MIMIK_WINDOW
#define MIMIK_WINDOW

#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/color_rect.hpp>


using namespace godot;

class MimikWindow : public Control
{
	GDCLASS(MimikWindow, Control);

public:
	MimikWindow();

	void init_window(String window_name);
	void update_window();

	virtual void init_content() {};
	virtual void update_content() {};

	virtual String get_window_name() { return ""; }

	void set_window_size(Vector2 new_size) { window_size = new_size; }
	Vector2 get_window_size() { return window_size; }


	bool is_window_dragged() { return is_clicked; };

private:
	// window dragging
	bool is_clicked = false;
	Vector2 latest_mouse_position;
	// true if left mouse button was pressed last frame
	bool latest_lmb_button_state;

	bool is_dragged = false;



protected:
	// godot nodes
	Node* content_node = nullptr;

	ColorRect* window_bar = nullptr;
	ColorRect* window_body = nullptr;


	// attaches packedscene referenced by name, then returns scene
	Node* attach_content_scene(String scene_name);

	Vector2 window_size = Vector2(400, 500);

	Vector2 get_content_size();
	const float window_bar_height = 25.0f;
	float content_margin = 0.0f;

	static void _bind_methods();
};

#endif // MIMIK_WINDOW