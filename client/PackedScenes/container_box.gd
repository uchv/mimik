extends Control

class_name ContainerBox

var container_name := "Container"

var event_id = -1 : set = _set_event_id
var container_event: MK_Event
var event_type := 0

var outline_width = 3.0

var is_dragged := false

var resize_left := false
var resize_right := false

@export var color_1_main : Color
@export var color_1_border : Color
@export var color_2_main : Color
@export var color_2_border : Color


func _draw():
	# rectangle
	var color_border
	var color_main
	
	if(event_type == 0):	# regular event
		color_border = color_1_border
		color_main = color_1_main
	else:
		color_border = color_2_border
		color_main = color_2_main
	
	
	var offset_v = Vector2(outline_width, outline_width)
	draw_rect(Rect2(Vector2.ZERO, size), color_border)
	draw_rect(Rect2(Vector2.ZERO + offset_v, size - 2 * offset_v), color_main)
		
	# name text
	var text_offset = Vector2(4.0, 15.0)
	draw_string(get_theme_font("MimikTheme"), text_offset, container_name, HORIZONTAL_ALIGNMENT_LEFT, size.x - text_offset.x * 2.0, 12)
	
	# if this event is selected, draw a white outline
	if container_event.get_event_id() == SceneContext.get_focused_event_id():
		draw_rect(Rect2(Vector2.ZERO, size), Color.WHITE, false, 2.0)



func _gui_input(event):
	if event is InputEventMouseMotion and is_dragged:
		position.x += event.relative.x
		
		queue_redraw()
	
	if event is InputEventMouseButton:
		if event.button_index == MOUSE_BUTTON_LEFT and event.pressed:
			is_dragged = true
			SceneContext.set_focused_event(container_event.get_event_id())
		# lmb was released
		elif event.button_index == MOUSE_BUTTON_LEFT:
			# apply event position change (not doing this every frame allows
			# for smoother dragging)
			var parent = get_parent() as MK_SceneTimeline
			var new_event_time = parent.convert_from_screen_pos(position).x

			container_event.set_event_time(new_event_time)
			print("New event time: " + str(container_event.get_start_time()))
		
			SceneContext.queue_timeline_redraw()
			
			# update state
			is_dragged = false
			


func set_container_name(new_name: String) -> void:
	container_name = new_name


# updates box position to match event start / end times.
# this is needed when scrolling the timeline and called from the timeline
func sync_container(line: int):
	if container_event:
		var parent = get_parent() as MK_SceneTimeline
		var pos1 = parent.convert_to_screen_pos(Vector2(container_event.get_start_time(), line))
		var pos2 = parent.convert_to_screen_pos(Vector2(container_event.get_end_time(), line + 1))
		
		#pos1.x -= 200.0
		#pos2.x -= 200.0
		
		position = pos1
		set_size(pos2 - pos1)
		
		event_type = container_event.get_event_type()
		
		queue_redraw()


func _set_event_id(id) -> void:
	event_id = id
	container_event = SceneData.get_event_by_id(id)
	

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
			if resize_left || resize_right:
				# if resizing, adjust event length (performance heavy)
				var parent = get_parent() as MK_SceneTimeline
				if container_event and parent:
					var new_event_length = parent.convert_from_screen_pos(size).x - parent.convert_from_screen_pos(Vector2.ZERO).x
					container_event.set_event_length(new_event_length, resize_right)
					
					print("New event length: " + str(container_event.get_end_time() - container_event.get_start_time()))
					
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
