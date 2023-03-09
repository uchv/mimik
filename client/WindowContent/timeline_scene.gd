extends MK_SceneTimeline

var containers = {}
var line_actor = []	# actor id where index is list position

@onready var rightclick_menu = $rightclick_menu



func _gui_input(event):
	if event is InputEventMouseButton:
		if (event.button_index == MOUSE_BUTTON_RIGHT && !event.pressed):
			# clicked on sidebar?
			if(get_local_mouse_position().x < get_sidebar_width() && 
				get_actor_id_at_coord(get_local_mouse_position()) != -1):
				rightclick_menu.visible = true
				rightclick_menu.position = get_global_mouse_position()


func draw_scenetimeline():
	var line = 0
	var container_num = 0
	line_actor.clear()
	
	for i in range(SceneData.get_actor_count()):
		var actor = SceneData.get_actor_by_id(i) as MK_Actor
		if !actor:
			continue

		var event_count = actor.get_event_count()
		var line_start = line
		
		for j in range(event_count):
			# actor.get_event_id(j)
			var eve_id = actor.get_event_id(j)
			var eve = SceneData.get_event_by_id(eve_id) as MK_Event
			if eve:
				if(containers.size() <= container_num):
					var box = add_container_box(eve_id)
					box.set_container_name(eve.get_event_name())
					box.sync_container(line)
				else:
					var box = containers[str(eve_id)]
					box.sync_container(line)
				container_num += 1
				line += 1
			else:
				event_count -= 1
		
		line = line_start + event_count
		# update line_actor array, which stores which actor occupies which line(s). 
		# currently, one line can only have 1 event. actors without events occupy 1 line.
		for x in range(max(event_count, 1)):
			line_actor.append(i)

		# draw this last to be on top of container boxes
		draw_sidebar_part(line_start, max(event_count, 1), actor.get_actor_name())


func add_container_box(id: int) -> ContainerBox:
	var res = ResourceLoader.load("res://PackedScenes/container_box.tscn") as PackedScene
	var container_node = res.instantiate()
	
	var time_marker = $interactive_time_cursor
	remove_child(time_marker)
	
	add_child(container_node)
	add_child(time_marker)
	
	container_node.event_id = id
	
	containers[str(id)] = container_node
	
	return container_node


func get_actor_id_at_coord(pos) -> int:
	var native_coords = convert_from_screen_pos(pos)
	
	if(native_coords.y >= 0 && native_coords.y < line_actor.size()):
		var id = int(floor(line_actor[native_coords.y]))
		# checks if actor actually exists
		var actor = SceneData.get_actor_by_id(id)
		if(actor):
			return id
	
	return -1


# IF YOU EVER CHANGE THE ORDER OF ITEMS IN THE RIGHT CLICK MENU, REWRITE SCENEDATA.ADD_EVENT(ACTOR_ID, ID)!!!
func _on_rightclick_menu_id_pressed(id):
	if (id == 0 || id == 1):		# CREATE FLEX ANIMATION EVENT
		var actor_id = get_actor_id_at_coord(rightclick_menu.position)
		print("Clicked actor id: " + str(actor_id))
		if actor_id >= 0:
			var event_id = SceneData.add_event(actor_id, id)
			SceneContext.set_focused_event(event_id)
			SceneContext.queue_timeline_redraw()
