#ifndef MIMIK_SCENEORGANIZER
#define MIMIK_SCENEORGANIZER

#include "mk_window.h"

#include "mk_scenedata.h"
#include "mk_scenecontext.h"

using namespace godot;


// window for flex animation
class MK_SceneOrganizer : public MimikWindow
{
	GDCLASS(MK_SceneOrganizer, MimikWindow);

public:
	void init_content();
	void update_content();

	String get_window_name() { return "Scene Organizer"; }

private:
	MK_SceneContext* scene_context = nullptr;
	MK_SceneData* scene_data = nullptr;

protected:
	static void _bind_methods();
};

#endif // MIMIK_SCENEORGANIZER