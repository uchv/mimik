extends Control

# base class for mimik window content scenes. 
# implemented in GDScript as it simplifies virtual functions

# gets called by MimikWindow upon adding the scene
# override this!
func deploy_content():
	pass


# gets called by MimikWindow, usually every frame
func update_content():
	pass
