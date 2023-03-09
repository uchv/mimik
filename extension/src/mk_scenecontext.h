#ifndef MIMIK_SCENECONTEXT
#define MIMIK_SCENECONTEXT

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/node.hpp>

using namespace godot;

class MK_SceneContext : public Object
{
	GDCLASS(MK_SceneContext, Object);

public:
	MK_SceneContext();
	static MK_SceneContext* get_singleton();
	static MK_SceneContext* singleton;

	void update_context();
	

	void playback_play();
	void playback_pause();
	void playback_stop();

	bool is_playing() { return is_currently_playing; }
	float get_playback_time() { return current_time; }
	void set_playback_time(float new_time);

	void queue_timeline_redraw() { redraw_timeline = true; }
	bool does_timeline_need_redraw();

	// returns id of currently focused actor. if there is no actor, returns -1
	int get_focused_actor_id() { return focused_actor_id; };
	// this also sets a new focused event belonging to actor
	void set_focused_actor(int actor_id);
	
	int get_focused_event_id() { return focused_event_id; };
	// this only sets event if it belongs to focused actor!!
	void set_focused_event(int event_id);

	void select_flex(int flex_id);
	void deselect_flex(int flex_id);
	void deselect_all_flexes();
	bool is_flex_selected(int flex_id);
	PackedInt32Array get_selected_flexes();

	// needed for delta time. making this a singleton doesn't work as get_process_delta_time() will just return 0. @TODO
	Node* timer_node = nullptr;
protected:
	static void _bind_methods();

private:
	int focused_actor_id = -1;
	int focused_event_id = -1;

	bool is_currently_playing = false;
	float current_time = 0.0f;

	bool redraw_timeline = false;
	bool redraw_timeline_done = false;

	// ids of flexes currently selected in flex editor
	std::vector<int> selected_flexes;	
};

#endif // MIMIK_SCENECONTEXT