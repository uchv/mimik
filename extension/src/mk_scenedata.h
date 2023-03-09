#ifndef MIMIK_SCENEDATA
#define MIMIK_SCENEDATA

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/object.hpp>

#include "mk_event.h"
#include "mk_actor.h"
#include "mk_scenecontext.h"

using namespace godot;

class MK_SceneData : public Object
{
	GDCLASS(MK_SceneData, Object);

public:
	MK_SceneData();
	static MK_SceneData* get_singleton();

	void update_data();

	//void load_scene();
	//void unload_scene();
	
	float get_scene_length_seconds() { return (float)scene_length_frames / FRAMES_PER_SECOND; };
	int get_scene_length_frames() { return scene_length_frames; }

	// returns id
	int add_actor(String model_path);
	// @TODO
	void remove_actor() {};
	int add_event(int actor_id, int type);
	void remove_event(int event_id);

	int get_actor_count() { return array_actors.size(); }
	int get_event_count() { return array_events.size(); }

	MK_Actor* get_actor_by_id(int actor_id);
	MK_Event* get_event_by_id(int event_id);

	// returns starting and end point of given event in frames
	Vector2i get_event_start_end(int event_id);

	// gets all blend shape states of current frame. 
	PackedFloat32Array get_current_frame_blend_states(int actor_id);



protected:
	static void _bind_methods();

private:
	static MK_SceneData* singleton;
	
	
	void make_current_frame_blend_states();
	

	MK_SceneContext* scene_context = nullptr;
	
	
	// currently, only working with 60 fps is possible.
	const float FRAMES_PER_SECOND = 60.0f;
	int scene_length_frames = 0;

	std::vector<MK_Actor*> array_actors;

	std::vector<MK_Event*> array_events;
	
	// @TODO: refactor
	PackedFloat32Array current_frame_blend_shape_states;
	
};

#endif // MIMIK_SCENEDATA