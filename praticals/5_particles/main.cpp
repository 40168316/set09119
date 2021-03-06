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
vector<ParticleSpring> springs;
ParticleSpring ps;

//static vector<unique_ptr<Entity>> SceneList;
static unique_ptr<Entity> floorEnt;
int xcoord, ycoord, zcoord, xposition, yposition, zposition;

// Creation of partical method
unique_ptr<Entity> CreateParticle(int xposition, int yposition, int zposition, int xcoord, int ycoord, int zcoord) {
  // Creation of ent which is a new entity which stores all the information like posiition and rotation
  unique_ptr<Entity> ent(new Entity());
  //ent->SetPosition(vec3(0, 5.0 + (double)(rand() % 200) / 20.0, 0));
  // Set the position of the entity
  ent->SetPosition(vec3(xposition, yposition, zposition));
  // Create a new physics component which deals with all the physics
  unique_ptr<Component> physComponent(new cPhysics());
  // Create a sphere object
  unique_ptr<cShapeRenderer> renderComponent(new cShapeRenderer(cShapeRenderer::SPHERE));
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

// Update method
bool update(double delta_time) 
{
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
		  unique_ptr<Entity> particle = CreateParticle(xposition = x * 5, yposition = 9, zposition = z * 5, xcoord = x, ycoord = 9, zcoord = z);
		  // Get the physics components of the particle
		  auto p = static_cast<cPhysics *>(particle->GetComponents("Physics")[0]);
		  SceneList.push_back(move(particle));
		  // Create the spring
	  }
  }
  floorEnt = unique_ptr<Entity>(new Entity());
  floorEnt->AddComponent(unique_ptr<Component>(new cPlaneCollider()));

  phys::SetCameraPos(vec3(-20.0f, 30.0f, -20.0f));												  
  phys::SetCameraTarget(vec3(0, 10.0f, 0));														  
  InitPhysics();																				  
  return true;																					  
}

// Render method
bool render() {
  // For everything in the scenelist render
  for (auto &e : SceneList) {
    e->Render();
  }
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