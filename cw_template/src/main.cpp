#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <graphics_framework.h>
#include <phys_utils.h>
#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include "particlelib.h"

using namespace std;
using namespace graphics_framework;
using namespace glm;
using namespace phys;

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
	float stiffness = 0.0f;
	phys::RGBAInt32 colour = BLACK;
	for (int x = 0; x < 7; x++)
	{
		cout << "x = " << x << "\n";
		for (int z = 0; z < 7; z++)
		{
			cout << "z = " << z << "\n";
			if (z < 1 || z> 5 || x <1 || x >5)
			{
				 colour = BLUE;
				 stiffness = 1.0f;
			}
			else if (z < 2 || z> 4 || x <2 || x >4)
			{
				 colour = GREEN;
				 stiffness = 0.5f;
			}
			else if (z < 3 || z> 3 || x <3 || x >3)
			{
				colour = YELLOW;
				stiffness = 0.5f;
			}
			else
			{
				colour = RED;
				stiffness = 0.5f;
			}
 			phys::DrawSphere(glm::vec3(x * 3.0f, 4.0f, z * 3.0f), 0.2f, colour);
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