#ifndef MIMIK_VIEWPORTSCENE
#define MIMIK_VIEWPORTSCENE

#include <godot_cpp/classes/node3d.hpp>

using namespace godot;

// wrapper for the 3d viewport scene.
// @TODO: delete
class MK_ViewportScene : public Node3D
{
	GDCLASS(MK_ViewportScene, Node3D);

public:
	// sets up camera, lights, etc.
	void setup_basics();

private:
	//Array added_actors

protected:
	static void _bind_methods();
};

#endif // MIMIK_VIEWPORTSCENE