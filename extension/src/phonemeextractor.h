#ifndef MK_PHONEMEEXTRACTOR
#define MK_PHONEMEEXTRACTOR

#include <godot_cpp/variant/string.hpp>
#include "../ext/pocketsphinx/include/pocketsphinx.h"

using namespace godot;

class PhonemeExtractor
{
public:
	String extract(String file_name);
};


#endif // MK_PHONEMEEXTRACTOR