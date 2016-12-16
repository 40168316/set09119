#pragma once
#include "game.h"
extern bool collisionbool;
extern glm::vec3 collpos;
extern glm::dvec3 force;
class cPhysics : public Component {
public:
	cPhysics();
	~cPhysics();
	glm::vec3 position;
	glm::vec3 prev_position;
	glm::dvec3 forces;
	glm::dvec3 velocity;
	bool makefixed = true;
	double mass;
	virtual void Update(double delta);
	virtual void SetParent(Entity *p);
	virtual void AddImpulse(const glm::vec3 &i);
private:
};
class cParticle : public cPhysics {
public:
  cParticle();
  ~cParticle();
  void Update(double delta);

private:
};

class cRigidBody : public cPhysics {
public:
  cRigidBody();
  ~cRigidBody();
  void Update(double delta);

private:
};

class cCollider : public Component {
public:
  cCollider(const std::string &tag);
  ~cCollider();
  void Update(double delta);

private:
};

class cSphereCollider : public cCollider {

public:
  double radius;
  cSphereCollider();
  ~cSphereCollider();

private:
};

class cPlaneCollider : public cCollider {
public:
  glm::dvec3 normal;
  cPlaneCollider();
  ~cPlaneCollider();

private:
};

// Spring particle class
class ParticleSpring
{
	/** The particle at the other end of the spring. - ie the next particle */
	cPhysics *other;
	// Spring constant which is the stiffness of the spring 
	double springConstant;
	// Dmaping factor, which is set to 40, which is the speed the tramploine returns to its orignial position
	double damp = 40.0;
	// Rest length is the length of the spring when no force is applied 
	double restLength;
public:
	/** Creates a new spring with the given parameters. */
	ParticleSpring(cPhysics *other, double springConstant, double restLength);
	ParticleSpring();
	/** Applies the spring force to the given particle. */
	virtual void updateForce(cPhysics *particle, double duration);
};

struct collisionInfo {
  const cCollider *c1;
  const cCollider *c2;
  const glm::dvec3 position;
  const glm::dvec3 normal;
  const double depth;
};

void InitPhysics();
void ShutdownPhysics();
void UpdatePhysics(const double t, const double dt);
