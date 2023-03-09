#ifndef MIMIK_TIMELINE
#define MIMIK_TIMELINE

#include <vector>

#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/h_scroll_bar.hpp>
#include <godot_cpp/classes/v_scroll_bar.hpp>

#include "mk_curveanimation.h"
#include "mk_curvehandle.h"
#include "mk_scenecontext.h"
#include "mk_scenedata.h"

using namespace godot;


class MK_Timeline : public Control
{
	GDCLASS(MK_Timeline, Control);

public:
	void draw_background();
	void draw_timepointer();	// vertical line to show playback time
	void draw_timescale();
	void draw_xaxis(float y);	// y in local coordinates
	virtual void draw_y_axes();		// different timelines have different needs for y axes
	void draw_curve(MK_CurveAnimation* curve, float event_time); 
	void draw_container_box(float start_time, float end_time, int line, String box_name = "");		// @TODO replace with a node
	
	MK_CurveAnimation* get_active_curve();
	void set_active_curve(MK_CurveAnimation* curve);


	// takes in position in local godot coordinates
	void add_curve_point(Vector2 pos);
	// sets up handles for current curve
	void setup_point_handles();
	void update_point_handles_pos();

	// takes in a vector2 with coordinates based in seconds(x) and amplitude (y, -1 to 1) and returns local screen coords in this timeline
	virtual Vector2 convert_to_screen_pos(Vector2 pos_native);
	// takes in a vector2 with local screen position and returns coords based in seconds(x) and amplitude (y, -1 to 1)
	virtual Vector2 convert_from_screen_pos(Vector2 pos_screen);

	void zoom_timeline(bool in);
	void pan_timeline(bool right);




private:
	// timeline can draw as many curves as needed through public draw_curve() - however, only one can be edited (with point handles)
	MK_CurveAnimation* cur_curve = nullptr;
	std::vector<MK_CurveHandle*> point_handles;

	const int num_timestamps_max = 10;	// number of time stamps allowed on screen at once

	float current_timepointer_pos = 0.0f;

protected:
	float left_border_time = 0.0f;
	float right_border_time = 60.0f;

	// base implementations of init_timeline & update_timeline should be called from derived classes
	virtual void init_timeline();	
	virtual void update_timeline();
	virtual void draw_timeline();

	MK_SceneContext* scene_context = nullptr;
	MK_SceneData* scene_data = nullptr;

	static void _bind_methods();
};

#endif // MIMIK_TIMELINE