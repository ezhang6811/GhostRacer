#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
class Actor : public GraphObject
{
public:
	Actor(int imageID, double startX, double startY, int dir = 0, double size = 1.0, unsigned int depth = 0)
		: GraphObject(imageID, startX, startY, dir, size, depth) {}
	virtual void doSomething() = 0;
};

class BorderLine : public Actor
{
public:
	BorderLine(double startX, double startY, int colorID);
	virtual void doSomething();
};

class GhostRacer : public Actor
{
public:
	GhostRacer(double startX, double startY);
	virtual void doSomething();
};

#endif // ACTOR_H_
