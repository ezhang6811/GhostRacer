#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
class Actor : public GraphObject
{
public:
	Actor(int imageID, double startX, double startY, int dir = 0, double size = 1.0, unsigned int depth = 0)
		: GraphObject(int imageID, double startX, double startY, int dir = 0, double size = 1.0, unsigned int depth = 0);
};

#endif // ACTOR_H_
