#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Actor.h"
#include <string>
#include <vector>

class GhostRacer;
class Actor;
// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    ~StudentWorld();

private:
    GhostRacer* m_player;
    std::vector<Actor*> m_actors;
    Actor* lastYellow;
    Actor* lastWhite;
};

#endif // STUDENTWORLD_H_
