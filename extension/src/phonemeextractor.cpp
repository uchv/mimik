#include "phonemeextractor.h"

#include <godot_cpp/variant/utility_functions.hpp>
#include <../../include/godot_cpp/templates/vector.hpp>	// @TODO: messy include path

#include <stdlib.h>


String PhonemeExtractor::extract(String file_name)
{
	ps_decoder_t* decoder;
	ps_config_t* config;
	FILE* fh;
	short* buf;
	long file_length;
	size_t nsamples;

	// using C file API as godot's is unfit for these kinds of binary files
	fh = fopen("file_name", "rb");
	if (fh == NULL)
	{
		godot::UtilityFunctions::print("Unable to open input file");
	}

	// input size
	fseek(fh, 0, SEEK_END);
	file_length = ftell(fh);
	if (file_length <= 0)
	{
		godot::UtilityFunctions::print("Couldn't find end of file");
		return "";
	}

	rewind(fh);


	// not initialize and configure the decoder to work with phonemes
	config = ps_config_init(NULL);
	ps_config_set_str(config, "hmm", "ExampleContent/en-us/en-us");
	ps_config_set_str(config, "allphone", "ExampleContent/en-us/en-us.lm.bin");
	ps_config_set_str(config, "dict", "ExampleContent/en-us/cmudict-en-us.dict");
	// this function fills in language models if previous attempts have failed. however, these models are very bad and will only generate random words.
	ps_default_search_args(config);

	if (ps_config_soundfile(config, fh, "al_dadwhatsup.wav") < 0)
		godot::UtilityFunctions::print("Unsupported input file");
	if ((decoder = ps_init(config)) == NULL)
		godot::UtilityFunctions::print("PocketSphinx decoder init failed");

	// Allocate data (skipping header) 
	file_length -= ftell(fh);
	if ((buf = (short*)malloc(file_length)) == NULL)
	{
		godot::UtilityFunctions::print("Unable to allocate [? num] bytes");
	}

	// Read input 
	nsamples = fread(buf, sizeof(buf[0]), file_length / sizeof(buf[0]), fh);
	if (nsamples != file_length / sizeof(buf[0]))
	{
		godot::UtilityFunctions::print("Unable to read [? num] samples");
	}

	// do some magic
	if (ps_start_utt(decoder) < 0)
	{
		godot::UtilityFunctions::print("Failed to start processing");
	}
	if (ps_process_raw(decoder, buf, nsamples, FALSE, TRUE) < 0)
	{
		godot::UtilityFunctions::print("ps_process_raw() failed");
	}
	if (ps_end_utt(decoder) < 0)
	{
		godot::UtilityFunctions::print("Failed to end processing");
	}

	// Print the result 
	const char* result = ps_get_hyp(decoder, NULL);
	if (result != NULL)
	{
		godot::UtilityFunctions::print(result);
	}

	/* Clean up */
	if (fclose(fh) < 0)
		godot::UtilityFunctions::print("Failed to close file");
	free(buf);
	ps_free(decoder);
	ps_config_free(config);

	return String(result);
}

