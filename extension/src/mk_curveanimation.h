#ifndef MIMIK_CURVEANIMATION
#define MIMIK_CURVEANIMATION

#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <vector>

using namespace godot;


class MK_CurveAnimation : public Resource
{
	GDCLASS(MK_CurveAnimation, Resource);

	struct Point {
		Vector2 main;			// placed by user
		Vector2 control_1;		// automatically generated
		Vector2 control_2;		// automatically generated 
	};

public:
	int add_point(Vector2 main);	// returns index of point
	int modify_point(int idx, Vector2 main);		// returns new index (->useful when points get reordered)
	void remove_point(int point_index);

	Vector2 get_main_point_at_index(int idx);
	
	float get_y_value_from_x(float x);

	void move_curve_timing(float offset);

	void calc_curve();

	// gets vector of points to use for drawing curve. run calc_curve() first, unless curve and view haven't been updated
	PackedVector2Array get_tesselated_points() { return tesselated_points; }
	PackedVector2Array get_main_points();

private:
	// user-placed points + automatically generated control points 
	std::vector<Point> main_points;

	PackedVector2Array tesselated_points;

	void calc_control_points(int idx);

protected:
	static void _bind_methods();
};

#endif // MIMIK_CURVEANIMATION