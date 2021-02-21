#include "StudentWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
}

int StudentWorld::init()
{
    player = new GhostRacer(128, 32);

    //add borderline objects

    int N = VIEW_HEIGHT / SPRITE_HEIGHT;
    //left yellow borderlines
    double LEFT_EDGE = ROAD_CENTER - ROAD_WIDTH / 2;
    for (int i = 0; i < N; i++)
    {
        double Y = i * SPRITE_HEIGHT;
        Actor* leftBorder = new BorderLine(LEFT_EDGE, Y, IID_YELLOW_BORDER_LINE);
        actors.push_back(leftBorder);
    }
    //right yellow borderlines
    double RIGHT_EDGE = ROAD_CENTER + ROAD_WIDTH / 2;
    for (int i = 0; i < N; i++)
    {
        double Y = i * SPRITE_HEIGHT;
        Actor* rightBorder = new BorderLine(RIGHT_EDGE, Y, IID_YELLOW_BORDER_LINE);
        actors.push_back(rightBorder);
    }

    int M = VIEW_HEIGHT / (4 * SPRITE_HEIGHT);
    //left white border line
    double LEFT_X = LEFT_EDGE + ROAD_WIDTH / 3;
    for (int i = 0; i < M; i++)
    {
        double Y = i * (4 * SPRITE_HEIGHT);
        Actor* whiteBorder = new BorderLine(LEFT_X, Y, IID_WHITE_BORDER_LINE);
    }
    //right white border line
    double RIGHT_X = RIGHT_EDGE - ROAD_WIDTH / 3;
    for (int i = 0; i < M; i++)
    {
        double Y = i * (4 * SPRITE_HEIGHT);
        Actor* whiteBorder = new BorderLine(RIGHT_X, Y, IID_WHITE_BORDER_LINE);
    }

    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    // This code is here merely to allow the game to build, run, and terminate after you hit enter.
    // Notice that the return value GWSTATUS_PLAYER_DIED will cause our framework to end the current level.
    decLives();
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
}
