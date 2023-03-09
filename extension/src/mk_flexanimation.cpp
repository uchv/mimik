#include "mk_flexanimation.h"

#include "mk_timeline.h"

void MK_FlexAnimation::init_content()
{
	attach_content_scene("window_flexanimation");
}

void MK_FlexAnimation::update_content()
{
	content_node->call("update_content");
}

void MK_FlexAnimation::_bind_methods()
{
}
