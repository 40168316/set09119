// Main class where the project is run from
// Includes which include various files libraries required for this project
#include "game.h"
#include "physics.h"
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <graphics_framework.h>
#include <phys_utils.h>
#include <thread>

// Add in standard, graphics_framework and glm libraries
using namespace std;
using namespace graphics_framework;
using namespace glm;
#define physics_tick 1.0 / 60.0

// Arraylists which use Entity which can store a range of values for objects - eg a spheres position, scale....
static vector<unique_ptr<Entity>> SceneList;
static vector<unique_ptr<Entity>> Balltodrop;
// A vector of springs which is used to connect up individual springs
vector<ParticleSpring> collectionofindividualsprings;
// Individual springs
ParticleSpring individualspring;
// The floor of the scene which uses Entity
static unique_ptr<Entity> floorEnt;
// Global positions for the coordinates and positions of the particles which make up the trampoline
int xcoord, ycoord, zcoord, xposition, yposition, zposition;

// Declartion of the two types of Camera's in the scene
free_camera freecam;
target_camera targetcam;
// Camera ID which is used to switch between freecamera and target camera
int cameraID;
// Variables used for the free cameras position
double xpos = 0.0f;
double ypos = 0.0f;
double cursor_x = 0.0;
double cursor_y = 0.0;

// Variables used to drop balls on to the trampoline
// Boolean which releases a ball from a fixed position when the up button is pressed
bool releaseballs = false;
// Boolean which renders the balls once the up ke is pressed 
bool releaseballstorender = false;
// Integer which is used to count the number of presses of the (up) button which releases the balls - this value is not reset at any point
int buttonreleaseballsnoreset = 0;
// Integer which is used to count the number of presses of the (up) button which releases the balls - this value is reset during the releasing of balls if statements
int buttonreleaseballsreset = 0;

// A vector of vec3 which stores information/posiitons to create a grid
vector <glm::vec3> grid;

// Initialise method which is used for free camera stuff
 bool initialise()
{
	// Set input posiiton and hide the cursor
	glfwSetInputMode(renderer::get_window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwGetCursorPos(renderer::get_window(), &cursor_x, &cursor_y);

	return true;
}

// Method which creates particles which are used to create the trampoline shape. These particles take in positions and coordinates
unique_ptr<Entity> CreateParticle(int xposition, int yposition, int zposition, int xcoord, int ycoord, int zcoord) {
  // Creation of ent which is a new entity which stores all the information like posiition and rotation
  unique_ptr<Entity> ent(new Entity());
  // Set the position of the entity
  ent->SetPosition(vec3(xposition, yposition, zposition));
  // Create a new physics component which deals with all the physics like forces and mass
  unique_ptr<Component> physComponent(new cPhysics());
  // Create a sphere object as a particle
  unique_ptr<cShapeRenderer> renderComponent(new cShapeRenderer(cShapeRenderer::PLANE));
  ent->SetScale(vec3(5.0, 0.0, 5.0));
  // Set the colour of the particle to a deafult black
  phys::RGBAInt32 colour = BLACK;
  // Set the colour of the particles depending on there posiiton
  // If the partciles are on the outside of the trampoline set their colour to blue
  if (zcoord <1 || zcoord >5 || xcoord < 1 || xcoord > 5) {
	  //ent->SetColour(colour = BLUE);
	  ent->colour = BLUE;
  }
  // If the partciles are inbetween certain values then set the colour to yellow
  else if (zcoord < 2 || zcoord >4 || xcoord < 2 || xcoord > 4)
  {
	  renderComponent->SetColour(colour = YELLOW);
  }
  // If the partciles are inbetween certain values then set the colour to yellow
  else if (zcoord < 3 || zcoord >3 || xcoord < 3 || xcoord > 3)
  {
	  renderComponent->SetColour(colour = GREEN);
  }
  // Else then set the remaining particles to white - in this case the only remaining particle should be white
  else 
  {
	  renderComponent->SetColour(colour = WHITE);
  }
  // Add the physics component to the entity
  ent->AddComponent(physComponent);
  // Add the sphere collider to the entity - C	ollider and particle are set to a scale of 0.3
  ent->AddComponent(unique_ptr<Component>(new cPlaneCollider()));
  // Add the render component to the entity
  ent->AddComponent(unique_ptr<Component>(move(renderComponent)));
  // Return the complete entity
  return ent;
}

// Method which creates a ball to be dropped on the trampoline
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
	// Add the physics component to the entity
	ent->AddComponent(physComponent);
	// Add the sphere collider to the entity
	ent->AddComponent(unique_ptr<Component>(new cSphereCollider()));
	/*cSphereCollider *coll = new cSphereCollider();
	coll->radius = 0.3f;
	ent->AddComponent(unique_ptr<Component>(coll));*/
	// Add the render component to the entity
	ent->AddComponent(unique_ptr<Component>(move(renderComponent)));
	// Return the complete entity
	return ent;
}

// Update method which used used to update cameras or update objects int he scene
bool update(float delta_time)
{
	// Target Camera Positons
	vec3 v1 = vec3(-10.0f, 50.0f, -10.0f);
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
		// Move the target camera to position vector2
		targetcam.set_position(v2);
		targetcam.set_target(vec3(10.0f, 15.0f, 10.0f));
		// Set camera ID to 0 to use the target camera
		cameraID = 0;
	}
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_3))
	{
		// Move the target camera to position vector3
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

	// If the camera Id is 0 - ie target cmaera
	if (cameraID == 0)
	{
		// Update the targetcameras position and target
		phys::SetCameraTarget(targetcam.get_target());
		phys::SetCameraPos(targetcam.get_position());
	}

	// If the f button is pressed then 
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_F))
	{
		// Set the camera ID to 1 - ie free camera
		cameraID = 1;
		// Mouse cursor is disabled and used to rotate freecam
		glfwSetInputMode(renderer::get_window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}

	// If the camera ID is 1 - ie free camera
	if (cameraID == 1)
	{
		// Get ratio of pixels depending on the screens height and width
		static double ratio_width = quarter_pi<float>() / static_cast<float>(renderer::get_screen_width());
		static double ratio_height = (quarter_pi<float>() * (static_cast<float>(renderer::get_screen_height()) / static_cast<float>(renderer::get_screen_width()))) / static_cast<float>(renderer::get_screen_height());

		// Create current x and y to store the current x and y mouse values
		double current_x;
		double current_y;
		// Get the current cursor position
		glfwGetCursorPos(renderer::get_window(), &current_x, &current_y);
		// Calculate delta of cursor positions from last frame
		float delta_x = current_x - cursor_x;
		float delta_y = current_y - cursor_y;

		// Multiply deltas by ratios defined above and gets the change in orientation
		delta_x = delta_x * ratio_width;
		delta_y = delta_y * ratio_height * -1;

		// Rotate the free camera by delta x and y
		freecam.rotate(delta_x, delta_y);

		// Movement of the free camera 
		// When W button is pressed move forward
		if (glfwGetKey(renderer::get_window(), GLFW_KEY_W))
		{
			freecam.move(vec3(0.0f, 0.0f, 10.0f) * delta_time);
		}
		// When S button is pressed move backwards
		if (glfwGetKey(renderer::get_window(), GLFW_KEY_S))
		{
			freecam.move(vec3(0.0f, 0.0f, -10.0f) * delta_time);
		}
		// When A button is pressed move left
		if (glfwGetKey(renderer::get_window(), GLFW_KEY_A))
		{
			freecam.move(vec3(-10.0f, 0.0f, 0.0f) * delta_time);
		}
		// When D button is pressed move right
		if (glfwGetKey(renderer::get_window(), GLFW_KEY_D))
		{
			freecam.move(vec3(10.0f, 0.0f, 0.0f) * delta_time);
		}
		// Update the cursors of the freecam
		cursor_x = current_x;
		cursor_y = current_y;
		freecam.update(delta_time);
		
		// Set the target and position of the free camera
		phys::SetCameraTarget(freecam.get_target());
		phys::SetCameraPos(freecam.get_position());
	}

	// Variables used for time
	static double t = 0.0;
	static double accumulator = 0.0;
	accumulator += delta_time;

	// If up key is pressed then 
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_UP))
	{
		// Set the balls that are to drop into the scene to true
		releaseballs = true;
		// Set the balls that are to drop into the scene to true which will allow them to now be rendered, making them visible
		releaseballstorender = true;
	}

	// If release balls variable is true and the up button has been pressed and released then
	if (releaseballs == true && glfwGetKey(renderer::get_window(), GLFW_KEY_UP) == GLFW_RELEASE)
	{
		// If number of button presses is less than 10 then
		if (buttonreleaseballsnoreset < 10) {
			// Get the specific ball - starting at 0 and increasing every time the button is pressed
			auto t = Balltodrop[buttonreleaseballsreset].get()->GetComponents("Physics");
			auto g = static_cast<cPhysics *>(t[0]);
			// Chnage the ball from a fixed position to nonfixed so it can move freely in the scene
			g->makefixed = false;
		}
		// If number of button presses is more then 9 then
		if (buttonreleaseballsnoreset > 9)
		{
			// Get a specific ball - and reset its position to be above the trampoline
			Balltodrop[buttonreleaseballsreset].get()->SetPosition(vec3((buttonreleaseballsreset * 2) + 2, 30, 10));

			auto t = Balltodrop[buttonreleaseballsreset].get()->GetComponents("Physics");
			static_cast<cPhysics *>(t[0]);
		}
		// Increment both variables by 1
		buttonreleaseballsnoreset++;
		buttonreleaseballsreset++;

		// If button presses for release ball equals 10 (it goes up one each time the button is pressed) then 
		if (buttonreleaseballsreset == 10)
		{
			// Reset the value to 0 - as there is only 10 balls pre-created in the load content method, which you will see bellow, 
			//if the amount goes above 10 then it will crash, then the number is just looped back round to solve this problem
			buttonreleaseballsreset = 0;
		}		
		// Set release balls back to false, so it means only one ball is allowed to move pre button press
		releaseballs = false;
	}
	
	// Nested for loops which create the collection of individual springs going along the x-axis
	// For loop which involves the 2-6th rows, particles 7-13, 14-20, 21-27, 28-34, 35-41
	for (int i = 7; i < 36; i+=7)
	{
		// For loop which goes through these individual rows, connecting the particles to create the collection of springs
		for (int j = i; j < i+6; j++)
		{
			// Looping through all the particals in the row
			auto b = SceneList[j].get()->GetComponents("Physics");
			auto p = static_cast<cPhysics *>(b[0]);
			// Updates the force between current particle and the next particle
			collectionofindividualsprings[j + 1].updateForce(p, 1.0);
			
		}	
	}

	// Nested for loops which create the collection of individual springs going along the z-axis
	// For loop which involves the 2-6th columns, particles (1,8,15,22,29,36), (2,9,16,23,30,37).....
	for (int j = 1; j < 6; j++) 
	{
		// For loop which goes through these individual columns, connecting the particles to create the collection of springs
		for (int i = j; i < 42; i += 7) 
		{
			// Looping through all the particals in the row
			auto b = SceneList[i].get()->GetComponents("Physics");
			auto p = static_cast<cPhysics *>(b[0]);
			// Updates the force between current particle and the next particle
			collectionofindividualsprings[i + 7].updateForce(p, 1.0);
		}
	}

  // If time passed is greater is greater than constant physics tick
  while (accumulator > physics_tick) {
	// Update tick until it is higher than accumulator
    UpdatePhysics(t, physics_tick);
    accumulator -= physics_tick;
    t += physics_tick;
  }

  // For loop which loops through all the balls in the trampoline
  for (int x = 0; x < 49; x++) {
	  // Go through every ball in the trampoline
	  auto c = SceneList[x].get()->GetComponents("Physics");
	  auto r = static_cast<cPhysics *>(c[0]);
	  // If particle is x, which are the out most particles, the particles at the edge of the trampoline
	  if (x < 8 || x >40 || x == 13 || x == 14 || x == 20 || x == 21 || x == 27 || x == 28 || x == 34 || x == 35) 
	  {
		  // Set the balls to stay stationary
		  r->makefixed = true;
	  }
	  // If the particle isnt on the outside
	  else
	  {
		  // If a collision has occured then 
		  if (collisionbool == true)
		  {
			  // Make the fixed particles unfixed so they can behave with the springs of the trampoline
			  r->makefixed = false;
		  }
	  }
  }

  for (int b = 0; b < 30; b+=5)
  {
	  for (int c = 0; c < 30; c+=5)
	  {
		  if (collpos.x > b && collpos.x <= b+5 && collpos.z > c && collpos.z <= c+5)
		  {
			  cout << "Tile b " << b/5 << " c " << c/5 << endl;
		  }
	  }
  }
  
  // Update everything in the scenelist
  for (auto &e : SceneList) {
    e->Update(delta_time);
  }
  //Update everything in the ballstodrop list
  for (auto &e : Balltodrop) {
	  e->Update(delta_time);
  }
	
  // Update scene																								  
  phys::Update(delta_time);																		  
  return true;																					  
}																								  

bool load_content() {																			  
  phys::Init();	
  // The creation of particles
  // For loop which increase the value of x upto 7
  for (int x = 0; x < 7; x++) {
	  // For loop which increase the value of z upto 7
	  for (int z = 0; z < 7; z++) {
		  // Create a new particle by calling the create particle method - pass it postions and coordinates
		  unique_ptr<Entity> particle = CreateParticle(xposition = x * 5, yposition = 19, zposition = z * 5, xcoord = x, ycoord = 19, zcoord = z);
		  // Get the physics components of the particle
		  auto p = static_cast<cPhysics *>(particle->GetComponents("Physics")[0]);
		  // Add to scene list
		  SceneList.push_back(move(particle));
		  // Create the individual springs which takes in p, the particle, a springconstant and rest length
		  // Old values are 20.0 and 2.0 - might need to change
		  individualspring = ParticleSpring(p, 40.0, 1.0);
		  // Add the spring to the springs vector, the collection of springs
		  collectionofindividualsprings.push_back(individualspring);
	  }
  }
 
  // For loop which generates the balls to be dropped - it creates 10
  for (int i = 1; i < 20; i+=2)
  {
	  // Create a ball as a unique entity
	  unique_ptr<Entity> ball = CreateBalltodrop(i, 30, 11);
	  // Add the ball into the ball to drop list
	  Balltodrop.push_back(move(ball));
  }

  floorEnt = unique_ptr<Entity>(new Entity());
  floorEnt->AddComponent(unique_ptr<Component>(new cPlaneCollider()));

  // Set the target cameras starting posiitons and targets
  targetcam.set_position(vec3(-20.0f, 30.0f, -20.0f));
  targetcam.set_target(vec3(10.0f, 15.0f, 10.0f));	
  InitPhysics();																				  
  return true;																					  
}

// Render method
bool render() {
  // For everything in the scenelist, render the objects
  for (auto &e : SceneList) {
    e->Render();
  }
  // If the balls are to be rendered is true then
  if (releaseballstorender == true)
  {
	  // For everything in the balltodrop list, render the objects
	  for (auto &e : Balltodrop) {
		  e->Render();
	  }
  }
  // Clear the grid to make sure everytime it is rendered it is updated
  grid.clear();
  // For everything in the scenelist, apply them to the grid by obtaining there positions
  for (auto &e : SceneList) {
	  grid.push_back(e->GetPosition());
  }

  // Draw the scene
  phys::DrawScene();
  // Draw the grid - as a solid
  phys::DrawGrid(&grid[0], 7 * 7, 7 ,phys::solid);
  return true;
}

// Mian method where the program is run from
void main() {
  // Create application
  app application;
  //application.set_initialise(initialise);
  // Set load content, update and render methods
  application.set_load_content(load_content);
  application.set_update(update);
  application.set_render(render);
  // Run application
  application.run();
}