#include "mk_curveanimation.h"

#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/control.hpp>


int MK_CurveAnimation::add_point(Vector2 main_point)
{
	// constrain point to curve dimensions
	main_point.x = MAX(main_point.x, 0.0f);
	main_point.y = CLAMP(main_point.y, 0.0f, 1.0f);

	// using a catmull-rom spline
	// control points are calculated automatically using the distance of the two adjacent points * 0.5 as velocity.
	// converting from hermite to bezier is done by taking 1/3 vector of velocity and the mirror of said vector -> calc_curve
	Point point_struct;
	point_struct.main = main_point;

	// get index in points array
	int idx = main_points.size();		// default is back of the array
	
	for (int i = 0; i < main_points.size(); i++)
	{
		if (main_point.x < main_points[i].main.x)
		{
			idx = i;
			break;
		}
	}

	// now insert and calculate control points afterwards
	auto it = main_points.begin() + idx;
	main_points.insert(it, point_struct);


	calc_control_points(idx - 1);
	calc_control_points(idx);
	calc_control_points(idx + 1);

	
	return idx;
}

// position in amp|sec
int MK_CurveAnimation::modify_point(int idx, Vector2 position)
{
	if (idx < 0 || idx >= main_points.size())
		return -1;

	int new_index = idx; 

	if (idx > 0 && position.x < main_points[idx - 1].main.x)
	{
		// swap points
		Point temp = main_points[idx - 1];
		main_points[idx - 1] = main_points[idx];
		main_points[idx] = temp;

		new_index -= 1;
	}
	else if (idx < main_points.size() - 1 && position.x > main_points[idx + 1].main.x)
	{
		// swap points
		Point temp = main_points[idx + 1];
		main_points[idx + 1] = main_points[idx];
		main_points[idx] = temp;

		new_index += 1;
	}
	else
	{
		main_points[idx].main = position;
	}
	
	calc_control_points(idx - 1);
	calc_control_points(idx);
	calc_control_points(idx + 1);
	
	return new_index;
}


// @TODO: implement
void MK_CurveAnimation::remove_point(int point_index)
{

}

Vector2 MK_CurveAnimation::get_main_point_at_index(int idx)
{
	if (idx < 0 || idx >= main_points.size())
	{
		return Vector2();
	}

	return main_points.at(idx).main;
}


// @FEATURE: do quadratic interpolation 
// @FEATURE: Only recalculate the parts of the curve that were changed
void MK_CurveAnimation::calc_curve()
{
	if (main_points.size() < 2)
	{
		return;
	}

	// @TODO: make this a parameter
	const int points_per_segment = 10;


	PackedVector2Array new_points;
	new_points.resize((main_points.size() - 1) * points_per_segment + 1);

	
	for (int i = 0; i < main_points.size() - 1; i++)
	{
		Vector2 p1 = main_points[i].main;
		Vector2 p2 = main_points[i].control_2;
		Vector2 p3 = main_points[i + 1].control_1;
		Vector2 p4 = main_points[i + 1].main;

		for (int j = 0; j < points_per_segment; j++)
		{
			new_points[i * points_per_segment + j] = p1.bezier_interpolate(p2, p3, p4, 1.0f / (float)points_per_segment * (float)j);
		}
	}

	// add the last point as it wasn't added in the loop
	new_points[new_points.size() - 1] = main_points.at(main_points.size() - 1).main;

	// finally assign
	tesselated_points = new_points;
}


PackedVector2Array MK_CurveAnimation::get_main_points()
{
	PackedVector2Array points_array;
	points_array.resize(main_points.size());

	for (int i = 0; i < main_points.size(); i++)
	{
		points_array[i] = main_points[i].main;
	}

	return points_array;
}


float MK_CurveAnimation::get_y_value_from_x(float x)
{
	// check if x value is within range of curve
	if (!(x > main_points[0].main.x && x < main_points[main_points.size() - 1].main.x))
	{
		return 0.0f;
	}

	/*
	// linear
	for (int i = 0; i < main_points.size(); i++)
	{
		if (x > main_points[i].main.x && x < main_points[i + 1].main.x)
		{
			Vector2 a = main_points[i].main;
			Vector2 b = main_points[i + 1].main;


			float factor = (x - a.x) / (b.x - a.x);

			return a.y + ((b.y - a.y) * factor);
		}
	}
	*/

	for (int i = 0; i < tesselated_points.size() - 1; i++)
	{
		// determine if this is the right tesselated point
		if (x > tesselated_points[i].x && x < tesselated_points[i+1].x)
		{
			Vector2 a = tesselated_points[i];
			Vector2 b = tesselated_points[i+1];


			float factor = (x - a.x) / (b.x - a.x);

			return a.y + ((b.y - a.y) * factor);
		}
	}

	// in case everything failed
	return 0.0f;
}

void MK_CurveAnimation::move_curve_timing(float offset)
{
	for (int i = 0; i < main_points.size(); i++)
	{
		main_points[i].main.x += offset;

		// no need for calc_control_points
		main_points[i].control_1.x += offset;
		main_points[i].control_2.x += offset;
	}

	calc_curve();
}

void MK_CurveAnimation::calc_control_points(int idx)
{
	// no point in calculating control points
	if (main_points.size() <= 1)
	{
		return;
	}
		

	// the control points represent bezier control points. they need to be on a straight line so the spline passes through the main points.
	// first point
	if (idx == 0)
	{
		Vector2 AB = main_points[1].main - main_points[0].main;

		main_points[0].control_1 = Vector2();	// no control point needed 
		main_points[0].control_2 = main_points[0].main + 0.333f * AB;

		// control point need to be in order, else there could be 2 y values for 1 x
		main_points[0].control_2.x = Math::min(main_points[0].control_2.x, main_points[1].main.x);
	}
	// last point
	else if (idx == main_points.size() - 1)	
	{
		Vector2 AB = main_points[idx].main - main_points[idx - 1].main;

		main_points[idx].control_1 = main_points[idx].main - 0.333f * AB;
		main_points[idx].control_2 = Vector2();			// no control point needed 

		// clamp
		main_points[idx].control_1.x = Math::max(main_points[idx].control_1.x, main_points[idx - 1].main.x);
	}
	// typical middle point
	else if(idx > 0 && idx < main_points.size() - 1)
	{
		Vector2 AC = main_points[idx + 1].main - main_points[idx - 1].main;

		main_points[idx].control_1 = main_points[idx].main - 0.333f * AC * 0.5f;
		main_points[idx].control_2 = main_points[idx].main + 0.333f * AC * 0.5f;

		// clamp
		main_points[idx].control_1.x = Math::max(main_points[idx].control_1.x, main_points[idx - 1].main.x);
		main_points[idx].control_2.x = Math::min(main_points[idx].control_2.x, main_points[idx + 1].main.x);

	}
}

void MK_CurveAnimation::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("add_point", "main"), &MK_CurveAnimation::add_point);
	ClassDB::bind_method(D_METHOD("modify_point", "idx", "main"), &MK_CurveAnimation::modify_point);
	ClassDB::bind_method(D_METHOD("remove_point", "point_index"), &MK_CurveAnimation::remove_point);
	ClassDB::bind_method(D_METHOD("calc_curve"), &MK_CurveAnimation::calc_curve);
	ClassDB::bind_method(D_METHOD("get_tesselated_points"), &MK_CurveAnimation::get_tesselated_points);
	ClassDB::bind_method(D_METHOD("get_y_value_from_x", "x"), &MK_CurveAnimation::get_y_value_from_x);
	ClassDB::bind_method(D_METHOD("get_main_points"), &MK_CurveAnimation::get_main_points);
	ClassDB::bind_method(D_METHOD("get_main_point_at_index", "idx"), &MK_CurveAnimation::get_main_point_at_index);
}
