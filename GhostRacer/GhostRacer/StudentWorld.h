#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Actor.h"
#include <string>
#include <vector>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class GhostRacer;
class Actor;

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    ~StudentWorld();

    GhostRacer* getPlayer() { return m_player; }

private:
    GhostRacer* m_player;
    std::vector<Actor*> m_actors;

    //pointers to determine road borders last added to screen
    Actor* lastYellow;
    Actor* lastWhite;
};

//values for border positions derived from global constants
const double LEFT_EDGE = ROAD_CENTER - ROAD_WIDTH / 2;
const double RIGHT_EDGE = ROAD_CENTER + ROAD_WIDTH / 2;
const double LEFT_X = LEFT_EDGE + ROAD_WIDTH / 3;
const double RIGHT_X = RIGHT_EDGE - ROAD_WIDTH / 3;

//function that checks for overlap of two Actor objects
bool createNewActor(int genChance);

#endif // STUDENTWORLD_H_