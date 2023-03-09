extends "res://WindowContent/window_content_base.gd"

var current_actor_id := -1
var actor_object: MK_Actor

# instantiated slider nodes of type MK_FlexSlider
var slider_nodes = []
var slider_states = []

# for selection purposes
var selected_slider_nodes = []
var outline_width := 3.0

var is_scrolling := false

# TARGET --> what is the viewport going to display? which flex states should the editor fetch?
# 0 = flex editor state
# 1 = current animation frame
# 2 = auto keyframe
var current_target : int = 0
var previous_target : int = 0


# the sliders are of class MK_FlexSliders and called FlexSlider0, FlexSlider1, etc.

func deploy_content():
	($container/scroll_container as ScrollContainer).get_h_scroll_bar().value_changed.connect(_on_scrollbar_changed)
	($container/scroll_container as ScrollContainer).get_v_scroll_bar().value_changed.connect(_on_scrollbar_changed)


func update_content():
	# check if there's a new focused actor
	var focused_actor_id = SceneContext.get_focused_actor_id()
	
	if current_actor_id != focused_actor_id:
		current_actor_id = focused_actor_id
		actor_object = SceneData.get_actor_by_id(focused_actor_id)
		
		setup_flex_sliders()

	
	# if animation is playing and editor wants to view, fetch flex states from animation
	if (current_target != 0 && SceneContext.is_playing()):
		var cur_time = SceneContext.get_playback_time()
		var cur_event = SceneData.get_event_by_id(SceneContext.get_focused_event_id()) as MK_Event
		
		for s in slider_nodes:
			var flex_id = s.get_flex_id()
			var val = cur_event.get_flex_state(flex_id, cur_time)
			s.set_flex_value(val)
	if(!SceneContext.is_playing()):
		# pass flex changes on
		for x in range(len(slider_nodes)):
			var new_value = slider_nodes[x].get_flex_value()
			if slider_states[x] != new_value:
				if current_target == 0 || current_target == 1:
					actor_object.set_frame_editor_flex_state(slider_nodes[x].get_flex_id(), new_value)
				elif false and current_target == 2:		# @TODO: enable this again!
					# this is bad for performance as it calculates the curve every frame
					var cur_event = SceneData.get_event_by_id(SceneContext.get_focused_event_id()) as MK_Event
					cur_event.set_flex_state(slider_nodes[x].get_flex_id(), SceneContext.get_playback_time(), new_value)
				slider_states[x] = new_value


func _gui_input(event):
	if event is InputEventMouseButton:
		if event.pressed and event.button_index == MOUSE_BUTTON_LEFT:
			# only checks for flex slider selection from labels. all other gui input on sliders
			# got blocked through MOUSE_FILTER_STOP.
			for s in slider_nodes:
				var mouse_pos = s.get_local_mouse_position()
				if (mouse_pos.x > 0 && mouse_pos.y > 0 &&
				mouse_pos.x < s.size.x && mouse_pos.y < s.size.y):
					var flex_id = s.get_flex_id()
					if SceneContext.is_flex_selected(flex_id):
						SceneContext.deselect_flex(flex_id)
					elif Input.is_key_pressed(KEY_CTRL):
						SceneContext.select_flex(flex_id)
					else:
						SceneContext.deselect_all_flexes()
						SceneContext.select_flex(flex_id)
						
					# need to redraw now 
					rebuild_selected_slider_nodes()
					queue_redraw()


func setup_flex_sliders() -> void:
	var needed_slider_count = actor_object.get_flex_count()
	var current_slider_count = len(slider_nodes)
	
	# need to create more sliders or get to just rename existing ones?
	if needed_slider_count > current_slider_count:
		slider_nodes.resize(needed_slider_count)
		slider_states.resize(needed_slider_count)
		
		for x in range(current_slider_count, needed_slider_count):
			var new_slider = MK_FlexSlider.new()
			new_slider.name = "FlexEditor" + str(x)
			$container/scroll_container/grid_container.add_child(new_slider)
			
			slider_nodes[x] = new_slider
			slider_states[x] = 0.0		# sliders always start out with all 0
	elif needed_slider_count < current_slider_count:
		# hide the unneeded sliders
		for x in range(needed_slider_count, current_slider_count):
			slider_nodes[x].hide()
		
	# now go through every slider and set it up
	# get names from json
	var au_names = {}
	var file = FileAccess.open("res://bin/facs_au_info.json", FileAccess.READ)
	var json = file.get_as_text()
	var au_object_array = JSON.parse_string(json)
	for au in au_object_array:
		var nme = au.keys()[0]
		au_names[nme] = au[nme]["name"]
	
	
	for x in range(needed_slider_count):
		var slider = slider_nodes[x] as MK_FlexSlider
		slider.set_flex_id(x)
		
		var blend_shape_name = actor_object.get_flex_name_by_id(x)
		var name_alias = ""
		if au_names.has(blend_shape_name):
			name_alias = au_names[blend_shape_name]
		
		if name_alias == "":
			slider.set_flex_name(blend_shape_name)
		else:
			slider.set_flex_name(name_alias)
		
		slider.set_flex_value(0.0)
		slider.set_active(true)
	
	# now check how many sliders can fit 
	if(slider_nodes.size() > 0):
		const minimum_size_min = 250
		
		var num_columns = floori(size.x / minimum_size_min)
		var new_size = size.x / float(num_columns)
		$container/scroll_container/grid_container.columns = num_columns
		for slider in slider_nodes:
			slider.custom_minimum_size.x = new_size


func rebuild_selected_slider_nodes() -> void:
	selected_slider_nodes.clear()
	
	var selected_flexes_ids = SceneContext.get_selected_flexes()
	for id in selected_flexes_ids:
		for s in slider_nodes:
			if s.get_flex_id() == id:
				selected_slider_nodes.append(s)


func _draw():
	var scroll_container = $container/scroll_container as ScrollContainer
	
	var outline_offset_vec = Vector2(outline_width, outline_width)
	
	for n in selected_slider_nodes:
		var n_pos = n.position
		n_pos.x -= scroll_container.scroll_horizontal - outline_width
		n_pos.y -= scroll_container.scroll_vertical - outline_width
		n_pos -= outline_offset_vec * 0.5
		
		var n_size = n.size - outline_offset_vec
		
		draw_rect(Rect2(n_pos, n_size), Color.DARK_RED, false, outline_width)


func _on_scrollbar_changed(_f):
	queue_redraw()




func _on_record_keyframes_toggled(auto_key_on):
	var options = $container/ColorRect/HBoxContainer/options_target as OptionButton
	if auto_key_on:
		previous_target = options.get_selected_id()
		# can't have automatic be target 
		if previous_target == 2:
			previous_target = 0
		options.select(options.get_item_index(2))
		options.disabled = true
		$container/ColorRect/HBoxContainer/apply_keyframe.visible = false
	else:
		options.disabled = false
		if options.get_selected_id() == 2:
			options.select(options.get_item_index(previous_target))
		$container/ColorRect/HBoxContainer/apply_keyframe.visible = true


func _on_options_target_item_selected(index):
	var options = $container/ColorRect/HBoxContainer/options_target as OptionButton
	
	current_target = options.get_selected_id()
	
	if options.get_selected_id() == 2:
		var check = $container/ColorRect/HBoxContainer/record_keyframes as CheckBox
		check.button_pressed = true


func _on_apply_keyframe_pressed():
	var cur_event = SceneData.get_event_by_id(SceneContext.get_focused_event_id()) as MK_Event
	
	if !cur_event:
		return
		
	for slider in slider_nodes:
		cur_event.set_flex_state(slider.get_flex_id(), SceneContext.get_playback_time(), slider.get_flex_value())
		
		
