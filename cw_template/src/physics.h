#pragma once
#include "game.h"
extern bool collisionbool;
class cPhysics : public Component {
public:
	cPhysics();
	~cPhysics();
	glm::vec3 position;
	glm::vec3 prev_position;
	glm::dvec3 forces;
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


class ParticleSpring
{
	/** The particle at the other end of the spring. */
	cPhysics *other;

	/** Holds the sprint constant. */
	double springConstant;

	/** Holds the rest length of the spring. */
	double restLength;

public:

	/** Creates a new spring with the given parameters. */
	ParticleSpring(cPhysics *other,
		double springConstant, double restLength);

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
