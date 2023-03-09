#ifndef MIMIK_ANIMATIONTIMELINE
#define MIMIK_ANIMATIONTIMELINE

#include "mk_timeline.h"

using namespace godot;


class MK_AnimationTimeline : public MK_Timeline
{
	GDCLASS(MK_AnimationTimeline, MK_Timeline);

public:
	MK_AnimationTimeline();


	Vector2 convert_to_screen_pos(Vector2 pos_native);
	Vector2 convert_from_screen_pos(Vector2 pos_screen);

private:
	float top_border_amp = 1.2f;
	float bottom_border_amp = -0.1f;
	
	void init_timeline();
	void process_timeline();
	void draw_timeline();

	void draw_y_axes();

	// flex id of currently displayed curve
	int cur_curve_flex_id = -1;

protected:
	void _notification(int p_what);
	static void _bind_methods();
};

#endif // MIMIK_ANIMATIONTIMELINE