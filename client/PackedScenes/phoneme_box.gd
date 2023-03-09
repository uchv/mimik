extends Control

# @TODO: Refactor, make this and EventBox inherit ContainerBox and interface with C++ 
class_name PhonemeBox

var current_phoneme := "aa"

#var event_id = -1 : set = _set_event_id
var time_start : float
var time_end : float

var outline_width = 3.0

var is_dragged := false

var resize_left := false
var resize_right := false

var parent_timeline : MK_PhonemeTimeline

@export var color_main : Color
@export var color_border : Color

func _ready():
	parent_timeline = get_parent()
	parent_timeline.scroll_timeline.connect(update_container_position)
	
func _process(_delta):
	if SceneContext.does_timeline_need_redraw():
		update_container_position()
		queue_redraw()
	
func _draw():	
	var offset_v = Vector2(outline_width, outline_width)
	draw_rect(Rect2(Vector2.ZERO, size), color_border)
	draw_rect(Rect2(Vector2.ZERO + offset_v, size - 2 * offset_v), color_main)
		
	# name text
	var text_offset = Vector2(4.0, 15.0)
	draw_string(get_theme_font("MimikTheme"), text_offset, current_phoneme, HORIZONTAL_ALIGNMENT_LEFT, size.x - text_offset.x * 2.0, 12)
	
	# if false, draw a white outline
	if false:
		draw_rect(Rect2(Vector2.ZERO, size), Color.WHITE, false, 2.0)



func _gui_input(event):
	if event is InputEventMouseMotion and is_dragged:
		position.x += event.relative.x
		
		queue_redraw()
	
	if event is InputEventMouseButton:
		if event.button_index == MOUSE_BUTTON_LEFT and event.pressed and event.double_click:
			# @TODO: panel switch menu
			pass
			
		elif event.button_index == MOUSE_BUTTON_LEFT and event.pressed:
			is_dragged = true
		# lmb was released
		elif event.button_index == MOUSE_BUTTON_LEFT and !event.pressed:
			# apply move / rescale changes
			var parent = get_parent() as MK_PhonemeTimeline
			var duration = time_end - time_start
			time_start = parent.convert_from_screen_pos(position).x
			time_end = time_start + duration

			print("New event time: " + str(time_start))
			
			# update state
			is_dragged = false
			


func set_phoneme(phoneme: String) -> void:
	current_phoneme = phoneme


func update_container_position() -> void:
	sync_container(time_start, time_end)

# updates box position to match event start / end times.
# this is needed when scrolling the timeline and called from the timeline
func sync_container(start_time : float, end_time : float):
	time_start = start_time
	time_end = end_time
	
	var pos1 = parent_timeline.convert_to_screen_pos(Vector2(start_time, 0.0))
	var pos2 = parent_timeline.convert_to_screen_pos(Vector2(end_time, 0.0))
	
	position = pos1
	size.x = pos2.x - pos1.x

	queue_redraw()


# resizing
func _on_resize_gui_input(event: InputEvent, right: bool):
	# left click for resizing
	if event is InputEventMouseButton:
		if event.pressed and event.button_index == MOUSE_BUTTON_LEFT:
			if right:
				resize_right = true
			else:
				resize_left = true
		# !event.pressed = released
		elif !event.pressed and event.button_index == MOUSE_BUTTON_LEFT:
			var new_duration = parent_timeline.convert_from_screen_pos(size).x - parent_timeline.convert_from_screen_pos(Vector2.ZERO).x
			if resize_left:
				time_start = time_end - new_duration
			elif resize_right:
				time_end = time_start + new_duration
				
			SceneContext.queue_timeline_redraw()
		
			resize_left = false
			resize_right = false
			
			
	if event is InputEventMouseMotion:
		if resize_right:
			var delta = event.relative.x
			
			size.x += delta
		elif resize_left:
			var delta = event.relative.x
			position.x += delta
			size.x -= delta
