#ifndef MIMIK_CURVEHANDLE
#define MIMIK_CURVEHANDLE

#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/core/class_db.hpp>


using namespace godot;


class MK_CurveHandle: public Control
{
	GDCLASS(MK_CurveHandle, Control);

public:
	MK_CurveHandle();
	//~MK_CurveHandle();

	Vector2 get_handle_pos() { return point_position; }
	void set_handle_pos(Vector2 new_pos, bool clamp_position);	// needs local position!
	//void set_selected(bool selected) { is_selected = selected; queue_redraw(); }
	int get_point_index() { return point_index; }
	void set_point_index(int idx) { point_index = idx; }
	bool has_handle_changed() { return has_changed; }

protected:
	void _notification(int p_what);
	static void _bind_methods();
	

private:
	bool has_changed = false;

	// in window local coordinates
	Vector2 point_position;

	int point_index = -1;
	//bool is_selected = false;

	// stuff for checking mouse input
	Vector2 latest_mouse_position;
	bool was_mouse_down = false;
	bool is_dragged = false;
	bool is_hovered = false;

};

#endif // MIMIK_CURVEHANDLE