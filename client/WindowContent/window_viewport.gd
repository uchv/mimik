extends "res://WindowContent/window_content_base.gd"


var actor_count := 0
@onready var line_edit = $Container/ButtonHolder/LineEdit

func deploy_content():
	# make container fill out space
	self.set_anchors_and_offsets_preset(Control.PRESET_FULL_RECT)
	var container := $Container
	container.set_anchors_and_offsets_preset(Control.PRESET_FULL_RECT)
	
	var sub_viewport := $Container/SubViewportContainer/SubViewport
	sub_viewport.set_size(Vector2i(container.size.x, container.size.y - $Container/ButtonHolder.size.y))

	

func update_content():
	# check if scenedata added another actor
	if SceneData.get_actor_count() > actor_count:
		# add another actor
		var actor = SceneData.get_actor_by_id(actor_count)
		var mesh_instance_node = actor.get_actor_root_node()
		$Container/SubViewportContainer/SubViewport/ViewportSceneBase.add_child(mesh_instance_node)	# if yes, 
		mesh_instance_node.set_scale(Vector3(0.03, 0.03, 0.03))			# @TODO readjust scaling 
		
		actor_count += 1
		
	if SceneContext.is_playing():
		line_edit.editable = false
		line_edit.text = str(SceneContext.get_playback_time())
	else: 
		line_edit.editable = true


func _on_playback_button_pressed(button_type):
	# these variables are used to avoid doing anyting until update_content() is called
	# this uncouples logic from godot's input timing and lets the extension do its thing
	match button_type:
		0:
			SceneContext.playback_play()
		1:
			SceneContext.playback_pause()
		2:
			SceneContext.playback_stop()
		


func _on_line_edit_text_changed(new_text):
	var new_time = float(new_text)
	SceneContext.set_playback_time(new_time)
