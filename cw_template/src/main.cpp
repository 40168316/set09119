#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <graphics_framework.h>
#include <phys_utils.h>
#include "main.h"

using namespace std;
using namespace graphics_framework;
using namespace glm;

bool load_content() {
	phys::Init();
	return true;
}

bool update(float delta_time) {
	//static float rot = 0.0f;
	//rot += 0.2f * delta_time;
	//phys::SetCameraPos(rotate(vec3(15.0f, 12.0f, 15.0f), rot, vec3(0, 1.0f, 0)));
	phys::SetCameraPos(vec3(30.0f, 10.0f, 30.0f));
	phys::Update(delta_time);

	return true;
}

bool render() {
	// Drawing a Sphere with X, Y, Z and Scale then Color
	float stiffness = 1.0f;
	for (float x = 0.0; x < 7; x++)
	{
		for (float z = 0.0; z < 7; z++)
		{
			phys::DrawSphere(glm::vec3(x * 3.0f, 4.0f, z * 3.0f), 0.2f, BLUE);
		}
	}
	phys::DrawScene();
	return true;
}

void main() {
	// Create application
	app application;
	// Set load content, update and render methods
	application.set_load_content(load_content);
	application.set_update(update);
	application.set_render(render);
	// Run application
	application.run();
}