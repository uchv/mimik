extends Control

class_name InteractiveTimeCursor

var parent_timeline : MK_Timeline

var mouse_x_offset : float
var is_dragged := false

# Called when the node enters the scene tree for the first time.
func _ready():
	parent_timeline = get_parent()
	parent_timeline.scroll_timeline.connect(update_position)
	position.x = parent_timeline.convert_to_screen_pos(Vector2(0.0, 0.0)).x - size.x / 2.0


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
#	if SceneContext.does_timeline_need_redraw():
#		position.x = get_x_coord(SceneContext.get_playback_time())
#		print("redraw time cursor")
#		queue_redraw()
		
	if !is_dragged and SceneContext.is_playing():
		update_position()


func update_position() -> void:
	position.x = get_x_coord(SceneContext.get_playback_time())

func get_x_coord(time : float) -> float:
	var pos_x = parent_timeline.convert_to_screen_pos(Vector2(time, 0.0)).x
	return pos_x - size.x / 2.0


func _gui_input(event):
	if event is InputEventMouseMotion and is_dragged:
		position.x += event.relative.x
		
	if event is InputEventMouseButton:
		if event.button_index == MOUSE_BUTTON_LEFT and event.pressed:
			is_dragged = true
			mouse_x_offset = get_local_mouse_position().x
		if event.button_index == MOUSE_BUTTON_LEFT and !event.pressed:
			is_dragged = false
			# apply new time
			var adjusted_position = Vector2(position.x + size.x / 2.0, 0.0)
			var new_time = parent_timeline.convert_from_screen_pos(adjusted_position).x
			SceneContext.set_playback_time(new_time)
	
func _draw():
	var pos_x = 0.0 + size.x / 2.0
	draw_line(Vector2(pos_x, 0.0), Vector2(pos_x, size.y), Color.DARK_RED, 3.0)
