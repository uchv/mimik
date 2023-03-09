extends MK_PhonemeTimeline

# list of phoneme nodes, need to be ordered after appearance!
var phoneme_nodes = []


func add_phoneme_box(phoneme, start, end) -> void:
	var res = ResourceLoader.load("res://PackedScenes/phoneme_box.tscn") as PackedScene
	var container_node = res.instantiate()
	
	# time marker needs to be always drawn on top
	var time_marker = $interactive_time_cursor
	remove_child(time_marker)
	
	add_child(container_node)
	add_child(time_marker)
	
	
	container_node.set_phoneme(phoneme)
	container_node.sync_container(start, end)
	
	# put in array at the right spot
	if phoneme_nodes.size() == 0:
		phoneme_nodes.append(container_node)
	elif start < phoneme_nodes[0].time_start:
		phoneme_nodes.push_front(container_node)
	elif start > phoneme_nodes.back().time_start:
		phoneme_nodes.append(container_node)
	else:
		for x in range(phoneme_nodes.size() - 1):
			if (start > phoneme_nodes[x].time_start && start < phoneme_nodes[x+1].time_start):
				phoneme_nodes.insert(x + 1, container_node)
				break


func get_phoneme_count() -> int:
	return phoneme_nodes.size()


# { "phoneme": "aa", "start_time": 0.01, "end_time": 0.09 }
# idx is in order of time
func get_phonemes_dict(idx : int) -> Dictionary:
	if idx < 0 or idx >= phoneme_nodes.size():
		return {}
		
	var node = phoneme_nodes[idx]
	var phone_dict = {
		"phoneme": node.current_phoneme, 
		"start_time": node.time_start, 
		"end_time": node.time_end
	}
	return phone_dict
			
