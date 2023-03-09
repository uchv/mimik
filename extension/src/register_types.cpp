#include "register_types.h"

#include "mk_application.h"
#include "mk_scenecontext.h"
#include "mk_window.h"
#include "mk_modelviewport.h"
#include "mk_viewportscene.h"
#include "mk_scenecontext.h"
#include "mk_scenedata.h"
#include "mk_flexeditor.h"
#include "mk_actor.h"
#include "mk_flexslider.h"
#include "mk_timeline.h"
#include "mk_animationtimeline.h"
#include "mk_scenetimeline.h"
#include "mk_curveanimation.h"
#include "mk_flexanimation.h"
#include "mk_curvehandle.h"
#include "mk_event.h"
#include "mk_sceneorganizer.h"
#include "mk_lipsync.h"
#include "mk_facseditor.h"
#include "mk_phonemetimeline.h"

#include <godot/gdnative_interface.h>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>
#include <godot_cpp/classes/engine.hpp>

using namespace godot;

static MK_SceneContext* _scene_context;
static MK_SceneData* _scene_data;

void initialize_mimik_types(ModuleInitializationLevel p_level)
{
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}

	ClassDB::register_class<MimikApplication>();

	ClassDB::register_class<MK_SceneContext>();
	ClassDB::register_class<MK_SceneData>();
	
	ClassDB::register_class<MimikWindow>();
	ClassDB::register_class<MK_ModelViewport>();
	ClassDB::register_class<MK_FlexEditor>();
	ClassDB::register_class<MK_FlexAnimation>();
	ClassDB::register_class<MK_SceneOrganizer>();
	ClassDB::register_class<MK_LipSync>();
	ClassDB::register_class<MK_FACSEditor>();
	
	ClassDB::register_class<MK_ViewportScene>();
	
	ClassDB::register_class<MK_Actor>();
	ClassDB::register_class<MK_Event>();
	
	ClassDB::register_class<MK_FlexSlider>();
	
	ClassDB::register_class<MK_Timeline>();
	ClassDB::register_class<MK_AnimationTimeline>();
	ClassDB::register_class<MK_SceneTimeline>();
	ClassDB::register_class<MK_PhonemeTimeline>();
	
	ClassDB::register_class<MK_CurveHandle>();
	ClassDB::register_class<MK_CurveAnimation>();


	// mimik relies on singletons 
	// the singleton pattern is generally unfashionable in modern software architectures
	// however, godot supports them very well and they allow for easy C++ / GDScript interoperability
	_scene_context = memnew(MK_SceneContext);
	Engine::get_singleton()->register_singleton("SceneContext", _scene_context);

	_scene_data = memnew(MK_SceneData);
	Engine::get_singleton()->register_singleton("SceneData", _scene_data);
}


void uninitialize_mimik_types(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}

	Engine::get_singleton()->unregister_singleton("SceneContext");
	memdelete(_scene_context);

	Engine::get_singleton()->unregister_singleton("SceneData");
	memdelete(_scene_data);
}

extern "C"
{

	// Initialization.

	GDNativeBool GDN_EXPORT mimik_library_init(const GDNativeInterface* p_interface, const GDNativeExtensionClassLibraryPtr p_library, GDNativeInitialization* r_initialization)
	{
		GDExtensionBinding::InitObject init_obj(p_interface, p_library, r_initialization);

		init_obj.register_initializer(initialize_mimik_types);
		init_obj.register_terminator(uninitialize_mimik_types);
		init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

		return init_obj.init();
	}
}