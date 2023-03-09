#ifndef MIMIK_PHONEMETIMELINE
#define MIMIK_PHONEMETIMELINE

#include "mk_timeline.h"

using namespace godot;


class MK_PhonemeTimeline: public MK_Timeline
{
	GDCLASS(MK_PhonemeTimeline, MK_Timeline);

public:
	MK_PhonemeTimeline();

	Vector2 convert_to_screen_pos(Vector2 pos_native);
	Vector2 convert_from_screen_pos(Vector2 pos_screen);

	void set_current_lipsync_event(int event_id);

private:
	void init_timeline();
	void process_timeline();
	void draw_timeline();

	void draw_y_axes();

	MK_Event* current_event = nullptr;

protected:
	void _notification(int p_what);
	static void _bind_methods();
};

#endif // MIMIK_PHONEMETIMELINE