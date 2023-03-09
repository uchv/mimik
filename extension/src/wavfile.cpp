#include "wavfile.h"

#include <vector>

#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <../../include/godot_cpp/templates/vector.hpp>


// based on ResourceImporterWAV::import() of engine code
bool WAVFile::import_file_wav(String file_path)
{
	Ref<FileAccess> file = FileAccess::open(file_path, FileAccess::READ);

	if (file->get_error() != 0)
	{
		UtilityFunctions::print("Failed to open file.");
		return false;
	}

	// check RIFF
	PackedByteArray riff = file->get_buffer(4);
	if (riff[0] != 'R' || riff[1] != 'I' 
		|| riff[2] != 'F' || riff[3] != 'F') 
	{
		UtilityFunctions::print(".wav misses RIFF.");
		return false;
	}

	// skip over filesize
	file->get_32();

	// check WAVE
	PackedByteArray wave = file->get_buffer(4);
	// check WAVE
	if (wave[0] != 'W' || wave[1] != 'A'
		|| wave[2] != 'V' || wave[3] != 'E')
	{
		UtilityFunctions::print(".wav misses WAVE.");
		return false;
	}


	int format_bits = 0;
	int format_channels = 0;

	AudioStreamWAV::LoopMode loop_mode = AudioStreamWAV::LOOP_DISABLED;
	uint16_t compression_code = 1;
	bool format_found = false;
	bool data_found = false;
	int format_freq = 0;
	int loop_begin = 0;
	int loop_end = 0;
	int frames = 0;

	std::vector<float> data;

	while (!file->eof_reached()) {
		// chunk 
		PackedByteArray buffer = file->get_buffer(4);

		if (buffer.size() < 4)
			break;

		char chunkID[4];
		chunkID[0] = (char)buffer[0];
		chunkID[1] = (char)buffer[1];
		chunkID[2] = (char)buffer[2];
		chunkID[3] = (char)buffer[3];

		// chunk size 
		uint32_t chunksize = file->get_32();
		uint32_t file_pos = file->get_position(); //save file pos, so we can skip to next chunk safely

		if (file->eof_reached()) {
			break;
		}

		if (chunkID[0] == 'f' && chunkID[1] == 'm' && chunkID[2] == 't' && chunkID[3] == ' ' && !format_found) {
			// IS FORMAT CHUNK 

			// usage of other formats (format codes) are unsupported in current importer version.
			compression_code = file->get_16();
			if (compression_code != 1 && compression_code != 3) {
				UtilityFunctions::print("Format not supported for WAVE file (not PCM). Save WAVE files as uncompressed PCM or IEEE float instead.");
			}

			format_channels = file->get_16();
			if (format_channels != 1 && format_channels != 2) {
				UtilityFunctions::print("Format not supported for WAVE file (not stereo or mono).");
			}

			format_freq = file->get_32(); //sampling rate

			file->get_32(); // average bits/second (unused)
			file->get_16(); // block align (unused)
			format_bits = file->get_16(); // bits per sample

			if (format_bits % 8 || format_bits == 0) {
				UtilityFunctions::print("Invalid amount of bits in the sample (should be one of 8, 16, 24 or 32).");
			}

			if (compression_code == 3 && format_bits % 32) {
				UtilityFunctions::print("Invalid amount of bits in the IEEE float sample (should be 32 or 64).");
			}

			// Don't need anything else, continue 
			format_found = true;
		}

		if (chunkID[0] == 'd' && chunkID[1] == 'a' && chunkID[2] == 't' && chunkID[3] == 'a' && !data_found) {
			UtilityFunctions::print("data");

			// IS DATA CHUNK
			data_found = true;

			if (!format_found) {
				UtilityFunctions::print("'data' chunk before 'format' chunk found.");
				break;
			}

			frames = chunksize;

			if (format_channels == 0) {
				UtilityFunctions::print(format_channels == 0, ERR_INVALID_DATA);
			}
			frames /= format_channels;
			frames /= (format_bits >> 3);

			
			data.resize(frames * format_channels);

			if (compression_code == 1) {
				if (format_bits == 8) {
					for (int i = 0; i < frames * format_channels; i++) {
						// 8 bit samples are UNSIGNED

						data[i] = int8_t(file->get_8() - 128) / 128.f;
					}
				}
				else if (format_bits == 16) {
					for (int i = 0; i < frames * format_channels; i++) {
						//16 bit SIGNED

						data[i] = int16_t(file->get_16()) / 32768.f;
					}
				}
				else {
					for (int i = 0; i < frames * format_channels; i++) {
						//16+ bits samples are SIGNED
						// if sample is > 16 bits, just read extra bytes

						uint32_t s = 0;
						for (int b = 0; b < (format_bits >> 3); b++) {
							s |= ((uint32_t)file->get_8()) << (b * 8);
						}
						s <<= (32 - format_bits);

						data[i] = (int32_t(s) >> 16) / 32768.f;
					}
				}
			}
			else if (compression_code == 3) {
				if (format_bits == 32) {
					for (int i = 0; i < frames * format_channels; i++) {
						//32 bit IEEE Float

						data[i] = file->get_float();
					}
				}
				else if (format_bits == 64) {
					for (int i = 0; i < frames * format_channels; i++) {
						//64 bit IEEE Float

						data[i] = file->get_double();
					}
				}
			}

			if (file->eof_reached()) {
				UtilityFunctions::print("Premature end of file.");
			}
		}

		file->seek(file_pos + chunksize);


		UtilityFunctions::print("Iteration done!");
	}


	// STEP 2, APPLY CONVERSIONS
	UtilityFunctions::print("Step 2");

	bool is16 = format_bits != 8;
	int rate = format_freq;


	int compression = 0; // 0 is no compression p_options["compress/mode"];


	Vector<uint8_t> dst_data;
	AudioStreamWAV::Format dst_format;

	dst_format = is16 ? AudioStreamWAV::FORMAT_16_BITS : AudioStreamWAV::FORMAT_8_BITS;
	dst_data.resize(data.size() * (is16 ? 2 : 1));
	
	{
		uint8_t* w = dst_data.ptrw();

		for (int i = 0; i < data.size(); i++) {
			if (is16) {
				int16_t v = CLAMP(data[i] * 32768, -32768, 32767);
				encode_uint16(v, &w[i * 2]);
				
			}
			else {
				int8_t v = CLAMP(data[i] * 128, -128, 127);
				w[i] = v;
			}
		}
	}
	UtilityFunctions::print("done with dst_data");
	

	PackedByteArray packed_data;
	packed_data.resize(dst_data.size());	// dst data is uint8 -> 1 byte

	for (int i = 0; i < packed_data.size(); i++)
	{
		packed_data[i] = dst_data[i];
	}
	UtilityFunctions::print("done with packed_data");

	// if audiostreamwav has already been created, delete old one
	if (sample)
	{
		memfree(sample);
	}

	UtilityFunctions::print("now setting audioi stream");
	sample = memnew(AudioStreamWAV);
	sample->set_data(packed_data);
	sample->set_format(dst_format);
	sample->set_mix_rate(rate);
	sample->set_loop_mode(loop_mode);
	sample->set_loop_begin(loop_begin);
	sample->set_loop_end(loop_end);
	sample->set_stereo(format_channels == 2);

	UtilityFunctions::print("done setting audioi stream");
	return true;
}



AudioStreamWAV* WAVFile::generate_audio_stream_wav()
{
	return sample;
}

