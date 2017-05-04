#pragma warning(disable : 4786)

#include "particleSystem.h"

#include <stdio.h>
#include <stdlib.h>
#include <cassert>
#include <cmath>
#include <climits>
#include <ctime>
#include <iostream>

using namespace std;


/***************
 * Constructors
 ***************/

ParticleSystem::ParticleSystem() : bake_fps(30), simulate(false), dirty(false), particleNum(500), pLife(2), mass(1.0), currentTime(0.0)
{
	srand(time(0));

	// add the controlling forces
	forces.push_back(new Gravity(Vec3f(0, -1.0, 0)));
}


/*************
 * Destructor
 *************/

ParticleSystem::~ParticleSystem() 
{
	particles.clear();
	forces.clear();
}


/******************
 * Simulation fxns
 ******************/

/** Start the simulation */
void ParticleSystem::startSimulation(float t)
{
	bake_end_time = -1;
	simulate = true;
	dirty = true;
}

/** Stop the simulation */
void ParticleSystem::stopSimulation(float t)
{
	simulate = false;
	dirty = true;
}

/** Reset the simulation */
void ParticleSystem::resetSimulation(float t)
{
	simulate = false;
	dirty = true;
}

/** Compute forces and update particles **/
void ParticleSystem::computeForcesAndUpdateParticles(float t)
{
	bake_fps = t - currentTime;
	currentTime = t;
	if (simulate) {
		map<float, vector<Particle>>::iterator it = bakedParticles.find(t);
		if (it == bakedParticles.end()) {	// not baked, need simulation
			for (vector<Particle>::iterator iter = particles.begin(); iter != particles.end(); iter++) {
				iter->move(bake_fps);
			}
			bakeParticles(t);
		}
		else {
			particles = bakedParticles[t];
		}
	}
}


/** Render particles */
void ParticleSystem::drawParticles(float t)
{
	if (simulate) {
		for (vector<Particle>::iterator iter = bakedParticles[t].begin(); iter != bakedParticles[t].end(); iter++) {
			iter->draw();
		}
	}
}

/** Adds the current configuration of particles to
  * your data structure for storing baked particles **/
void ParticleSystem::bakeParticles(float t) 
{
	bakedParticles.insert(pair<float, vector<Particle>>(t, particles));
}

/** Clears out your data structure of baked particles */
void ParticleSystem::clearBaked()
{
	bakedParticles.clear();
}

void ParticleSystem::addParticles(Vec3f worldPos, int numOfParticles)
{
	if (simulate) {
		map<float, vector<Particle>>::iterator it = bakedParticles.find(currentTime + bake_fps);
		if (it == bakedParticles.end()) {
			for (int i = 0; i < numOfParticles; i++)
			{
				Particle p(Vec3f(worldPos[0], worldPos[1], worldPos[2]), 1.0, 5.0);

				float mag = rand() % 10 / 10.0 + 0.2;
				float theta = rand() % 360 / 57.3;

				float zSpeed = rand() % 10 / 10.0 + 2;
				float ySpeed = cos(theta) * mag;
				float xSpeed = sin(theta) * mag;

				p.setVelocity(Vec3f(xSpeed, ySpeed, zSpeed));
				
				for (vector<Force*>::iterator iter = forces.begin(); iter != forces.end(); iter++) {
					p.addForce(*iter);
				}

				particles.push_back(p);
			}
		}
	}
}



