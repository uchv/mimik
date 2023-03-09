#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/classes/audio_stream_wav.hpp>


using namespace godot;

class WAVFile
{
public:
    bool import_file_wav(String file_path);

    AudioStreamWAV* generate_audio_stream_wav();
    
private:
    uint32_t sample_rate = 0;   // called mix rate in AudioStreamWAV
    AudioStreamWAV::Format audio_format;
    //PackedByteArray audio_data;
    
    AudioStreamWAV* sample = nullptr;

    // marshalls.h in godot code base
    static inline unsigned int encode_uint16(uint16_t p_uint, uint8_t* p_arr) {
        for (int i = 0; i < 2; i++) {
            *p_arr = p_uint & 0xFF;
            p_arr++;
            p_uint >>= 8;
        }

        return sizeof(uint16_t);
    }


protected:
};