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
int x = 0 , y = 0, z = 0;

// Creation of partical method
unique_ptr<Entity> CreateParticle(int x, int y, int z) {
  // Creation of ent which is a new entity which stores all the information like posiition and rotation
  unique_ptr<Entity> ent(new Entity());
  //ent->SetPosition(vec3(0, 5.0 + (double)(rand() % 200) / 20.0, 0));
  // Set the position of the entity

  ent->SetPosition(vec3(x, y, z));
  // Create a new physics component which deals with all the physics
  unique_ptr<Component> physComponent(new cPhysics());
  // Create a sphere object
  unique_ptr<cShapeRenderer> renderComponent(new cShapeRenderer(cShapeRenderer::SPHERE));
  // Set the colour of the sphere
  renderComponent->SetColour(phys::RandomColour());
  phys::RGBAInt32 colour = BLACK;
  if (z <2 || z >28 || x < 2 || x > 28) {
	  renderComponent->SetColour(colour = BLUE);
  }
  else if (z < 7 || z >23 || x < 7 || x > 23)
  {
	  renderComponent->SetColour(colour = YELLOW);
  }
  else if (z < 12 || z >18 || x < 12 || x > 18) 
  {
	  renderComponent->SetColour(colour = GREEN);
  }
  else 
  {
	  renderComponent->SetColour(colour = RED);
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

  // Update spring forces
  for (int i = 0; i < 7 - 1; i++)
  {
	  // Looping through all the particals and adding the spring forces
	  auto b = SceneList[i].get()->GetComponents("Physics");
	  auto p = static_cast<cPhysics *>(b[0]);
	  springs[i + 1].updateForce(p, 1.0);
  }

  for (int i = 7; i < 14 - 1; i++)
  {
	  // Looping through all the particals and adding the spring forces
	  auto b = SceneList[i].get()->GetComponents("Physics");
	  auto p = static_cast<cPhysics *>(b[0]);
	  springs[i + 1].updateForce(p, 1.0);
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

  for (x = 0; x < 7; x++) {
	  for (int z = 0; z < 7; z++) {
		  if (x < 1 || x > 34 ) {
			  auto c = SceneList[x,z].get()->GetComponents("Physics");
			  auto r = static_cast<cPhysics *>(c[0]);
			  r->position = r->prev_position;
		  }
	  }
  }
  // Update scene																								  
  phys::Update(delta_time);																		  
  return true;																					  
}																								  
																								  
bool load_content() {																			  
  phys::Init();																					  
// The creation of particles - For i, which is particle number
  for (x = 0; x < 7; x++) {
	  // Loop for increasing z
	  for (int z = 0; z < 7; z++) {
		  SceneList.push_back(move(CreateParticle(x * 5, y = 9, z * 5)));
	  }
  }
  cout << SceneList[1]->GetPosition() << endl;
  floorEnt = unique_ptr<Entity>(new Entity());
  floorEnt->AddComponent(unique_ptr<Component>(new cPlaneCollider()));


//Creation of spring
  for (int i = 0; i < 7; i++)
  {
	 // for (int z = 0; z < SceneList.size(); z++)
	  //{
		  auto b = SceneList[i].get()->GetComponents("Physics");
		  auto p = static_cast<cPhysics *>(b[0]); // Get physics component
		  cout << p->position << endl;
		  // Particle spring assigned values of springconstant and restlength
		  ps = ParticleSpring(p, 20.0, 2.0);
		  // Spring is then moved
		  springs.push_back(ps);
	  //}
  }
  
  for (int i = 7; i < 14; i++)
  {
	  // for (int z = 0; z < SceneList.size(); z++)
	  //{
	  auto b = SceneList[i].get()->GetComponents("Physics");
	  auto p = static_cast<cPhysics *>(b[0]); // Get physics component
	  cout << p->position << endl;
	  // Particle spring assigned values of springconstant and restlength
	  ps = ParticleSpring(p, 20.0, 2.0);
	  // Spring is then moved
	  springs.push_back(ps);
	  //}
  }
  

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