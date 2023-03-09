#ifndef MIMIK_EVENT
#define MIMIK_EVENT

#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/core/class_db.hpp>

#include "mk_curveanimation.h"

using namespace godot;

enum MK_EventType
{
	EVENT_REGULAR = 0,
	EVENT_LIPSYNC
};

class MK_Event : public Object
{
	GDCLASS(MK_Event, Object);

public:
	MK_Event();
	~MK_Event();


	void set_event_id(int id) { event_id = id; };
	int get_event_id() { return event_id; }

	int get_actor_id() { return actor_id; }
	void set_actor_id(int a_id);

	int get_event_type() { return (int)event_type; }
	void set_event_type(int type) { event_type = (MK_EventType)type; }

	String get_event_name() { return event_name; }
	void set_event_name(String name) { event_name = name; }
	
	float get_start_time() { return start_time; }
	float get_end_time() { return end_time; }
	
	void set_event_time(float new_time);
	void set_event_length(float new_length, bool change_back = true);		// change_back: whether to move back (end) time or start time

	float get_flex_state(int flex_id, float time);
	void set_flex_state(int flex_id, float time, float state);

	MK_CurveAnimation* get_animation_for_flex(int flex_id);


private:
	// id of actor who this event belongs to
	int actor_id;

	int event_id;

	MK_EventType event_type = MK_EventType::EVENT_REGULAR;

	String event_name = "Event";

	float start_time = 0.0f;
	float end_time = 10.0f;

	std::vector<MK_CurveAnimation*> animation_curves;

protected:
	static void _bind_methods();
};

#endif // MIMIK_EVENT