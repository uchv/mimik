#include "mk_modelviewport.h"



void MK_ModelViewport::init_content()
{
	attach_content_scene("window_viewport");
}

void MK_ModelViewport::update_content()
{
	content_node->call("update_content");
}

void MK_ModelViewport::_bind_methods()
{
}
