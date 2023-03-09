extends "res://WindowContent/window_content_base.gd"


func deploy_content():
	pass
	
	
func update_content():
	pass


var new_actor: int
func _on_addactor_pressed():
	var dialog = $file_dialog_model
	dialog.visible = true
	return


func _on_button3_pressed():
	SceneContext.queue_timeline_redraw()


# called when user has chosen GLTF model in the file dialog
func _on_file_dialog_model_chosen(dir):
	print("Loading model " + dir)
	new_actor = SceneData.add_actor(dir)
	SceneContext.queue_timeline_redraw()
