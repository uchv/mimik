extends "res://WindowContent/window_content_base.gd"

var current_event_id := -1
var viewing_lipsync_event := false
@onready var audio_player = $AudioStreamPlayer

@onready var no_event_label = $no_event_label
@onready var play_button = $container_content/play_button
@onready var import_button = $container_content/import_button
@onready var phonemes_timeline = $container_content/MarginContainer/phonemes_timeline

var phoneme_blend_states = {}


func deploy_content():
	# load blend shape values for phonemes
	var file = FileAccess.open("res://bin/phoneme_facs.json", FileAccess.READ)
	var json = JSON.parse_string(file.get_as_text())

	for ph in json:
		var phoneme_name = ph.keys()[0]
		var blend_shapes = {}
		for bs in ph[phoneme_name]:
			blend_shapes[bs.keys()[0]] = bs[bs.keys()[0]]
		phoneme_blend_states[ph.keys()[0]] = blend_shapes
	print(phoneme_blend_states)

var is_preview_lipsync := false
var preview_time := 0.0
func update_content():
	if SceneContext.get_focused_event_id() != current_event_id:
		current_event_id = SceneContext.get_focused_event_id()
		phonemes_timeline.set_current_lipsync_event(current_event_id)
		
		var new_event = SceneData.get_event_by_id(current_event_id) as MK_Event
		if new_event:
			if new_event.get_event_type() == 1:
				viewing_lipsync_event = true
			else:
				viewing_lipsync_event = false
		else:
			viewing_lipsync_event = false
			
		# adjust visibility
		no_event_label.visible = !viewing_lipsync_event
		import_button.visible = viewing_lipsync_event
		play_button.visible = viewing_lipsync_event
		phonemes_timeline.visible = viewing_lipsync_event
		
		queue_redraw()



func _on_file_dialog_wav_file_selected(path):
	# parent WindowBody->MK_LipSync.import_wav
	var lipsync_cpp = get_parent().get_parent() as MK_LipSync
	lipsync_cpp.import_wav(path)
	var stream = lipsync_cpp.gen_audio_stream()
	audio_player.stream = stream
	
	#now generate lipsync
#	var phoneme_data = lipsync_cpp.run_phoneme_extraction(path) as String
#	var phonemes_str = []
#	var start_times = []
#	var end_times = []
#	var idx = 0
#	for item in phoneme_data.split(" "):
#		if idx == 0:
#			phonemes_str.append(str(item))
#		if idx == 1:
#			start_times.append(float(item))
#		if idx == 2:
#			end_times.append(float(item))
#
#		idx += 1
#		if idx > 2:
#			idx = 0
#
#	if phonemes_str.size() == start_times.size() and start_times.size() == end_times.size():
#		for x in range(phonemes_str.size()):
#			phonemes_timeline.add_phoneme_box(phonemes_str[x], start_times[x], end_times[x])
			
	
	# @TODO: get phonemes from sphinx
	phonemes_timeline.add_phoneme_box("t", 0.52, 0.65)
	phonemes_timeline.add_phoneme_box("ao", 0.02, 0.08)
	phonemes_timeline.add_phoneme_box("l", 0.08, 0.15)
	phonemes_timeline.add_phoneme_box("iy", 0.37, 0.52)
	phonemes_timeline.add_phoneme_box("aa", 0.28, 0.37)
	phonemes_timeline.add_phoneme_box("r", 0.15, 0.28)
	
	apply_lipsync()
	


func _on_import_wav_pressed():
	$file_dialog_wav.visible = true
	


func _on_play_audio_pressed():
	audio_player.play()
	SceneContext.set_playback_time(0.0)
	SceneContext.playback_play()



# generate curves
func apply_lipsync() -> void:
	var event = SceneData.get_event_by_id(current_event_id) as MK_Event
	var actor = SceneData.get_actor_by_id(SceneContext.get_focused_actor_id()) as MK_Actor
	
	if !actor or !event:
		return
	
	# now apply blend shapes for speech
	var phone_count = phonemes_timeline.get_phoneme_count()
	
	# these are all blend shapes used for speech. @TODO: complete list.
	var speech_flexes = ["AU25R+AU25L", "AU18R+AU18L", "AU22R+AU22L", "AU20R+AU20L", "AU31", "AU26R+AU26L", "AU27R+AU27L"]
	
	for x in range(phone_count):
		var phone = phonemes_timeline.get_phonemes_dict(x)
		
		for flex in speech_flexes:
			var flex_id = actor.get_flex_id_by_name(flex)
			
			var cur_value = get_state_in_phoneme(flex, phone["phoneme"]) * 0.75
			
			event.set_flex_state(flex_id, phone["start_time"], cur_value)
			
			if x == phone_count - 1:
				event.set_flex_state(flex_id, phone["end_time"], 0.0)
			


func get_state_in_phoneme(blend_shape : String, phoneme: String) -> float:
	var bs_dict = phoneme_blend_states.get(phoneme)
	if bs_dict:
		var value = bs_dict.get(blend_shape)
		if value:
			return value
	return 0.0

