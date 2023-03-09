#ifndef MIMIK_SCENETIMELINE
#define MIMIK_SCENETIMELINE

#include "mk_timeline.h"

using namespace godot;


class MK_SceneTimeline : public MK_Timeline
{
	GDCLASS(MK_SceneTimeline, MK_Timeline);

public:
	MK_SceneTimeline();

	Vector2 convert_to_screen_pos(Vector2 pos_native);
	Vector2 convert_from_screen_pos(Vector2 pos_screen);

	float get_sidebar_width() { return sidebar_width; }

private:
	void init_timeline();
	void process_timeline();
	void draw_timeline();

	void draw_sidebar_part(int line_start, int line_length, String actor_name);

	float sidebar_width = 200.0f;


protected:
	void _notification(int p_what);
	static void _bind_methods();
};

#endif // MIMIK_SCENETIMELINE