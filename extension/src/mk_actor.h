#ifndef MIMIK_ACTOR
#define MIMIK_ACTOR

#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/mesh_instance3d.hpp>

#include "mk_event.h"


using namespace godot;

class MK_Actor : public Object
{
	GDCLASS(MK_Actor, Object);

public:

	// array of float values 0.0f - 1.0f

	void load_actor(String model_path);
	bool is_actor_loaded() { return is_loaded; }

	int get_actor_id() { return id; }
	void set_actor_id(int new_id);

	String get_actor_name() { return actor_name; }
	void set_actor_name(String name) { actor_name = name; }

	int get_flex_count() { return flexes_count; }
	int get_flex_id_by_name(String name);
	String get_flex_name_by_id(int flex_id);

	int get_frame_editor_flex_state(int flex_id);
	void set_frame_editor_flex_state(int flex_id, float value);

	Node* get_actor_root_node();

	void apply_current_frame_blend_states(bool flex_editor);

	void add_event(MK_Event* eve);
	void remove_event(int event_id);
	
	int get_event_id(int array_idx) { return event_ids[array_idx]; }
	int get_event_count() { return event_ids.size(); }


private:
	bool is_loaded = false;

	String actor_name = "An Actor";

	MeshInstance3D* actor_mesh_instance; // mesh_instance is needed for manipulating blend shapes
	Node* actor_root_node;

	
	float* frame_editor_flex_states;
	float* current_frame_flex_states;
	
	// actor ids start from 0 and leave no integers out
	int id = -1;
	int flexes_count;

	std::vector<int> event_ids;
	

protected:
	static void _bind_methods();
};

#endif // MIMIK_ACTOR