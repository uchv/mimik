#include "mk_flexeditor.h"

void MK_FlexEditor::init_content()
{
	attach_content_scene("window_flexeditor");
}

void MK_FlexEditor::update_content()
{
	content_node->call("update_content");
}

void MK_FlexEditor::_bind_methods()
{
}
