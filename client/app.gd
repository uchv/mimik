extends MimikApplication


# Called when the node enters the scene tree for the first time.
func _ready():
	print("app.gd ready")
	init_application()


func _process(_delta):
	update_application()



var window_access = {}

# registers window for taskbar, allowing user to show/hide windows in the task bar 
func register_window_taskbar(window: MimikWindow, start_visible: bool = true):
	var popup = $toolbar/container/button_windows/popup_windows as PopupMenu
	
	var window_name = window.get_window_name()
	var id = window_access.size()
	
	popup.add_check_item(window_name, id)
	popup.set_item_checked(popup.get_item_index(id), start_visible)
	window.visible = start_visible
	
	window_access[str(id)] = window

# triggered when a window in the window show/hide popup is pressed
func _on_popup_window_pressed(id):
	var popup = $toolbar/container/button_windows/popup_windows as PopupMenu
	var window = window_access[str(id)]
	
	var index = popup.get_item_index(id)
	var checked = popup.is_item_checked(index)
	
	popup.set_item_checked(index, !checked)
	window.visible = !checked


func _on_button_windows_pressed():
	var popup = $toolbar/container/button_windows/popup_windows as PopupMenu
	
	# set position
	var parent = popup.get_parent()
	popup.position = Vector2(parent.global_position.x, parent.global_position.y + parent.size.y)
	
	popup.visible = true # !popup.visible


func _on_button_exit_pressed():
	get_tree().quit()


	


func _on_button_save_file_pressed():
	var file_dialog = $file_dialog_save as FileDialog
	file_dialog.visible = true


func _on_file_dialog_save_confirmed():
	var file_dialog = $file_dialog_save as FileDialog
	var cur_file = file_dialog.current_dir
	print("saving file " + cur_file)
