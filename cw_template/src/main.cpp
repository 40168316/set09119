#include "game.h"
#include "physics.h"
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <graphics_framework.h>
#include <phys_utils.h>
#include <thread>

using namespace std;
using namespace graphics_framework;
using namespace glm;
#define physics_tick 1.0 / 60.0

static vector<unique_ptr<Entity>> SceneList;
static vector<unique_ptr<Entity>> Planes;
static vector<unique_ptr<Entity>> Balltodrop;

vector<ParticleSpring> springs;
ParticleSpring ps;

// A library of meshes
map<string, mesh> meshes;
//static vector<unique_ptr<Entity>> SceneList;
static unique_ptr<Entity> floorEnt;
int xcoord, ycoord, zcoord, xposition, yposition, zposition;

free_camera freecam;
target_camera targetcam;
int cameraID;
// Used for the free camera
double xpos = 0.0f;
double ypos = 0.0f;
int loops = 0;

// Creation of partical method
unique_ptr<Entity> CreateParticle(int xposition, int yposition, int zposition, int xcoord, int ycoord, int zcoord) {
  // Creation of ent which is a new entity which stores all the information like posiition and rotation
  unique_ptr<Entity> ent(new Entity());
  // Set the position of the entity
  ent->SetPosition(vec3(xposition, yposition, zposition));
  // Create a new physics component which deals with all the physics
  unique_ptr<Component> physComponent(new cPhysics());
  // Create a sphere object
  unique_ptr<cShapeRenderer> renderComponent(new cShapeRenderer(cShapeRenderer::BOX));
  ent->SetScale(vec3(5.0f, 0.5, 5.0f));
  // Set the colour of the sphere
  renderComponent->SetColour(phys::RandomColour());
  phys::RGBAInt32 colour = BLACK;
  if (zcoord <1 || zcoord >5 || xcoord < 1 || xcoord > 5) {
	  renderComponent->SetColour(colour = BLUE);
  }
  else if (zcoord < 2 || zcoord >4 || xcoord < 2 || xcoord > 4)
  {
	  renderComponent->SetColour(colour = YELLOW);
  }
  else if (zcoord < 3 || zcoord >3 || xcoord < 3 || xcoord > 3)
  {
	  renderComponent->SetColour(colour = GREEN);
  }
  else 
  {
	  renderComponent->SetColour(colour = WHITE);
  }
  // Add the physics component to the entity
  ent->AddComponent(physComponent);
  // Add the sphere collider to the entity
  ent->AddComponent(unique_ptr<Component>(new cSphereCollider()));
  // Add the render component to the entity
  ent->AddComponent(unique_ptr<Component>(move(renderComponent)));
  return ent;
}

unique_ptr<Entity> CreateBalltodrop(int xposition, int yposition, int zposition) {
	// Creation of ent which is a new entity which stores all the information like posiition and rotation
	unique_ptr<Entity> ent(new Entity());
	// Set the position of the entity
	ent->SetPosition(vec3(xposition, yposition, zposition));
	// Create a new physics component which deals with all the physics
	unique_ptr<Component> physComponent(new cPhysics());
	// Create a sphere object
	unique_ptr<cShapeRenderer> renderComponent(new cShapeRenderer(cShapeRenderer::SPHERE));
	// Set the colour of the sphere
	renderComponent->SetColour(phys::RandomColour());
	phys::RGBAInt32 colour = BLACK;
	// Add the physics component to the entity
	ent->AddComponent(physComponent);
	// Add the sphere collider to the entity
	ent->AddComponent(unique_ptr<Component>(new cSphereCollider()));
	// Add the render component to the entity
	ent->AddComponent(unique_ptr<Component>(move(renderComponent)));
	return ent;
}

unique_ptr<Entity> CreatePlane(float xposition, float yposition, float zposition) {
	// Creation of ent which is a new entity which stores all the information like posiition and rotation
	unique_ptr<Entity> ent(new Entity());
	//ent->SetPosition(vec3(0, 5.0 + (double)(rand() % 200) / 20.0, 0));
	// Set the position of the entity
	ent->SetPosition(vec3(xposition, yposition, zposition));
	ent->SetScale(vec3(5.0f, 0.5, 5.0f));
	// Create a new physics component which deals with all the physics
	unique_ptr<Component> physComponent(new cPhysics());
	unique_ptr<cShapeRenderer> renderComponent(new cShapeRenderer(cShapeRenderer::BOX));
	// Set the colour of the sphere
	renderComponent->SetColour(phys::RandomColour());
	// Add the physics component to the entity
	ent->AddComponent(physComponent);
	// Add the sphere collider to the entity
	ent->AddComponent(unique_ptr<Component>(new cSphereCollider()));
	// Add the render component to the entity
	ent->AddComponent(unique_ptr<Component>(move(renderComponent)));
	return ent;
}

// Update method
bool update(float delta_time)
{
	// Target Camera Positons
	vec3 v1 = vec3(-20, 20, -20);
	vec3 v2 = vec3(-20, 20, 60);
	vec3 v3 = vec3(60, 20, -20);
	vec3 v4 = vec3(60, 20, 60);

	// Buttons pressed to move target camera
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_1))
	{
		// Move the target camera to position vector1
		targetcam.set_position(v1);
		targetcam.set_target(vec3(10.0f, 15.0f, 10.0f));
		// Set camera ID to 0 to use the target camera
		cameraID = 0;
	}
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_2))
	{
		// Move the target camera to position vector1
		targetcam.set_position(v2);
		targetcam.set_target(vec3(10.0f, 15.0f, 10.0f));
		// Set camera ID to 0 to use the target camera
		cameraID = 0;
	}
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_3))
	{
		// Move the target camera to position vector1
		targetcam.set_position(v3);
		targetcam.set_target(vec3(10.0f, 15.0f, 10.0f));
		// Set camera ID to 0 to use the target camera
		cameraID = 0;
	}
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_4))
	{
		// Move the target camera to position vector1
		targetcam.set_position(v4);
		targetcam.set_target(vec3(10.0f, 15.0f, 10.0f));
		// Set camera ID to 0 to use the target camera
		cameraID = 0;
	}

	if (cameraID == 0)
	{
		glfwSetInputMode(renderer::get_window(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		targetcam.update(delta_time);
		phys::SetCameraTarget(targetcam.get_target());
		phys::SetCameraPos(targetcam.get_position());
	}

	if (glfwGetKey(renderer::get_window(), GLFW_KEY_F))
	{
		cameraID = 1;
		// Mouse cursor is disabled and used to rotate freecam
		glfwSetInputMode(renderer::get_window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}

	if (cameraID == 1)
	{
		//Get mouse position for freecam
		double deltax, deltay;
		glfwGetCursorPos(renderer::get_window(), &deltax, &deltay);
		double tempx = deltax;
		double tempy = deltay;
		deltax -= xpos;
		deltay -= ypos;
		double ratio_width = (double)renderer::get_screen_width() / (double)renderer::get_screen_height();
		double ratio_height = 1.0 / ratio_width;
		deltax *= ratio_width * delta_time / 10;
		deltay *= -ratio_height * delta_time / 10;
		//Rotate freecam based on the mouse coordinates
		freecam.rotate(deltax, deltay);
		// set last cursor pos
		xpos = tempx;
		ypos = tempy;
		//Movement (freecam)
		if (glfwGetKey(renderer::get_window(), GLFW_KEY_W))
		{
			freecam.move(vec3(0.0f, 0.0f, 10.0f) * delta_time);
		}
		if (glfwGetKey(renderer::get_window(), GLFW_KEY_S))
		{
			freecam.move(vec3(0.0f, 0.0f, -10.0f) * delta_time);
		}
		if (glfwGetKey(renderer::get_window(), GLFW_KEY_A))
		{
			freecam.move(vec3(-10.0f, 0.0f, 0.0f) * delta_time);
		}
		if (glfwGetKey(renderer::get_window(), GLFW_KEY_D))
		{
			freecam.move(vec3(10.0f, 0.0f, 0.0f) * delta_time);
		}
		if (glfwGetKey(renderer::get_window(), GLFW_KEY_UP))
		{
			freecam.move(vec3(0.0f, 10.0f, 0.0f) * delta_time);
		}
		if (glfwGetKey(renderer::get_window(), GLFW_KEY_DOWN))
		{
			freecam.move(vec3(0.0f, -10.0f, 0.0f) * delta_time);
		}
		//Update freecam
		freecam.update(delta_time);
		phys::SetCameraTarget(freecam.get_target());
		phys::SetCameraPos(freecam.get_position());
	}

	// Variables used for time
	static double t = 0.0;
	static double accumulator = 0.0;
	//
	accumulator += delta_time;

	// b gets the physics components p which is the actual physics component
	auto b = SceneList[SceneList.size() - 1].get()->GetComponents("Physics");
	auto p = static_cast<cPhysics *>(b[0]);

	// If up key is pressed then add impulse
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_UP))
	{
		// The impulse adds to the y axis
		p->AddImpulse(vec3(0.0f, 200.0f, 0.0f));
	}

	// Update spring forces
	//for (int i = 0; i < 48; i++)
	//{
	//    if ((i != 6 && i != 13 && i != 20 && i != 27 && i != 34 && i !=41))
	//    {
	//	    // Looping through all the particals and adding the spring forces
	//	    auto b = SceneList[i].get()->GetComponents("Physics");
	//	    auto p = static_cast<cPhysics *>(b[0]);
	//	    springs[i + 1].updateForce(p, 1.0);
	//    }
	//}
	for (int i = 7; i < 36; i+=7)
	{
		for (int j = i; j < i+6; j++)
		{
			// Looping through all the particals and adding the spring forces
			auto b = SceneList[j].get()->GetComponents("Physics");
			auto p = static_cast<cPhysics *>(b[0]);
			springs[j + 1].updateForce(p, 1.0);
			//cout << SceneList[j].get()->GetPosition().y << endl;
		}	
	}
	for (int j = 1; j < 6; j++) {
		for (int i = j; i < 42; i += 7) {
			auto b = SceneList[i].get()->GetComponents("Physics");
			auto p = static_cast<cPhysics *>(b[0]);
			//if (i + 6 < 49)
			{
				springs[i + 7].updateForce(p, 1.0);
			}
			//cout << i << endl;
		}
	}

	/*for (int i = 7; i < 11; i++)
	{
		auto b = Planes[i].get()->GetComponents("Physics");
		auto p = static_cast<cPhysics *>(b[0]);
		springs[i + 1].updateForce(p, 1.0);
	}*/

  // If time passed is greater is greater than constant physics tick
  while (accumulator > physics_tick) {
	// Update tick until it is higher than accumulator
    UpdatePhysics(t, physics_tick);
    accumulator -= physics_tick;
    t += physics_tick;
  }



  // Update everything in the scenelist
  for (auto &e : SceneList) {
    e->Update(delta_time);
  }
  for (auto &e : Planes) {
	  e->Update(delta_time);
  }
  for (auto &e : Balltodrop) {
	  e->Update(delta_time);
  }
	// Keep balls at edge of trampoline stationary
	for (int x = 0; x < 49; x++) {
		if (x < 8 || x >40 || x == 13 || x == 14 || x == 20 || x ==21|| x == 27 || x == 28 || x == 34 || x == 35) {
			auto c = SceneList[x].get()->GetComponents("Physics");
			auto r = static_cast<cPhysics *>(c[0]);
			//r->position = r->prev_position;
			r->makefixed = true;
		}
	}	
  // Update scene																								  
  phys::Update(delta_time);																		  
  return true;																					  
}																								  
																								  
bool load_content() {																			  
  phys::Init();	
// The creation of particles - For i, which is particle number
  for (int x = 0; x < 7; x++) {
	  // Loop for increasing z
	  for (int z = 0; z < 7; z++) {
		  // Create a new particle by calling the particle method
		  unique_ptr<Entity> particle = CreateParticle(xposition = x * 5, yposition = 19, zposition = z * 5, xcoord = x, ycoord = 19, zcoord = z);
		  // Get the physics components of the particle
		  auto p = static_cast<cPhysics *>(particle->GetComponents("Physics")[0]);
		  // Add to scene list
		  SceneList.push_back(move(particle));
		  // Create the spring
		  // Old values are 20.0 and 2.0 - might need to change
		  ps = ParticleSpring(p, 40.0, 1.0);
		  // Add the spring ps to the springs vector
		  springs.push_back(ps);
		  //cout << "Coord = " << xcoord << " " << ycoord << " " << zcoord << " at Position = " << xposition  << " " << yposition << " " << zposition << endl;
	  }
  }
  for (float i = 2.5f; i < 31.0f; i += 5.0f) {
	  for (float j = 2.5f; j < 31.0f; j += 5.0f) {
		  // Centre point, Scale
		  //phys::DrawCube(glm::vec3(i, SceneList[t++].get()->GetPosition().y, j), glm::vec3(5.0f, 0.5f, 5.0f));
		  unique_ptr<Entity> plane = CreatePlane(xposition = i, yposition = 19, zposition = j);
		  auto p = static_cast<cPhysics *>(plane->GetComponents("Physics")[0]);
		  // Add to scene list
		  Planes.push_back(move(plane));
	  }
  }

  for (float i = 5; i <24 ; i+=3)
  {
	  unique_ptr<Entity> ball = CreateBalltodrop(xposition = i, yposition = 29, zposition = i);
	  auto p = static_cast<cPhysics *>(ball->GetComponents("Physics")[0]);
	  // Add to scene list
	  Balltodrop.push_back(move(ball));
  }

  floorEnt = unique_ptr<Entity>(new Entity());
  floorEnt->AddComponent(unique_ptr<Component>(new cPlaneCollider()));

  targetcam.set_position(vec3(-20.0f, 30.0f, -20.0f));
  targetcam.set_target(vec3(10.0f, 15.0f, 10.0f));	
  InitPhysics();																				  
  return true;																					  
}

// Render method
bool render() {
	int t = 0;
  // For everything in the scenelist render
  for (auto &e : SceneList) {
    e->Render();
  }
  /*for (auto &e : Planes) {
	  e->Render();
  }*/
  for (auto &e : Balltodrop) {
	  e->Render();
  }
  //for (float i = 2.5f; i < 31.0f; i += 5.0f) {
	 // for (float j = 2.5f; j < 31.0f; j += 5.0f) {
		//  // Centre point, Scale
		//  phys::DrawCube(glm::vec3(i, SceneList[t++].get()->GetPosition().y, j), glm::vec3(5.0f, 0.5f, 5.0f));
	 // }
  //}
  // Draw the scene
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