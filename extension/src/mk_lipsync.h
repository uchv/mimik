#ifndef MIMIK_LIPSYNC
#define MIMIK_LIPSYNC

#include "mk_window.h"

#include "mk_scenedata.h"
#include "mk_scenecontext.h"
#include "wavfile.h"

#include <godot_cpp/classes/audio_stream_wav.hpp>

using namespace godot;


// window for flex animation
class MK_LipSync: public MimikWindow
{
	GDCLASS(MK_LipSync, MimikWindow);

public:
	void init_content();
	void update_content();

	String get_window_name() { return "Lip Synchronization"; }


private:
	MK_SceneContext* scene_context = nullptr;
	MK_SceneData* scene_data = nullptr;

	bool import_wav(String path);
	AudioStreamWAV* gen_audio_stream();

	String run_phoneme_extraction(String file_name);

	WAVFile* cur_wav_file;

protected:
	static void _bind_methods();
};

#endif // MIMIK_LIPSYNC