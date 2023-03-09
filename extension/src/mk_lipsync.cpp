#include "mk_lipsync.h"

#include <godot_cpp/variant/utility_functions.hpp>
#include "phonemeextractor.h"


void MK_LipSync::init_content()
{
	attach_content_scene("window_lipsync");

	scene_context = MK_SceneContext::get_singleton();
	scene_data = MK_SceneData::get_singleton();
}

void MK_LipSync::update_content()
{
	content_node->call("update_content");

}

bool MK_LipSync::import_wav(String path)
{
	WAVFile* wav_file = memnew(WAVFile);
	cur_wav_file = wav_file;
	return wav_file->import_file_wav(path);
}

AudioStreamWAV* MK_LipSync::gen_audio_stream()
{
	if (cur_wav_file)
	{
		AudioStreamWAV* wav = cur_wav_file->generate_audio_stream_wav();
		if (!wav)
		{
			UtilityFunctions::print("wav is null");
			return nullptr;
		}
		UtilityFunctions::print("wav aint null");
		return cur_wav_file->generate_audio_stream_wav();
	}

	return nullptr;
}

String MK_LipSync::run_phoneme_extraction(String file_path)
{
	PhonemeExtractor* extractor = memnew(PhonemeExtractor);
	String ret = extractor->extract(file_path);
	
	return ret;
}

void MK_LipSync::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("import_wav", "path"), &MK_LipSync::import_wav);
	ClassDB::bind_method(D_METHOD("gen_audio_stream"), &MK_LipSync::gen_audio_stream);
	ClassDB::bind_method(D_METHOD("run_phoneme_extraction"), &MK_LipSync::run_phoneme_extraction);
}
