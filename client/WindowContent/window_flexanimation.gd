extends "res://WindowContent/window_content_base.gd"


@onready var timeline = $VBoxContainer/MK_Timeline


var handle: MK_CurveHandle 	# there's only one curvehandle node which gets reused for every point

var tl_mode := 0
# 0 = move points
# 1 = add points, move points
# 2 = delete points

var was_lmb_down := false

func deploy_content():
	pass



func _process(_delta):
	if tl_mode == 1:
		if Input.is_mouse_button_pressed(MOUSE_BUTTON_LEFT):
			if was_lmb_down == false:
				timeline.add_curve_point(get_local_mouse_position())
				tl_mode = 0 
				was_lmb_down = true
		else:
			was_lmb_down = false
	if tl_mode == 2:
		pass	# @TODO when point handle refactor

	
func _select_mode(mode_num):
	tl_mode = mode_num
