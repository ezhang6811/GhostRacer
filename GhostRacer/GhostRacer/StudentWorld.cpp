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
: GameWorld(assetPath), m_player(nullptr), lastYellow(nullptr), lastWhite(nullptr)
{
}

int StudentWorld::init()
{
    m_player = new GhostRacer(this, 128, 32);

    //add borderline objects

    int N = VIEW_HEIGHT / SPRITE_HEIGHT;
    //left yellow borderlines
    for (int i = 0; i < N; i++)
    {
        double Y = i * SPRITE_HEIGHT;
        Actor* leftBorder = new BorderLine(this, LEFT_EDGE, Y, IID_YELLOW_BORDER_LINE);
        m_actors.push_back(leftBorder);
        lastYellow = leftBorder;
    }
    //right yellow borderlines
    for (int i = 0; i < N; i++)
    {
        double Y = i * SPRITE_HEIGHT;
        Actor* rightBorder = new BorderLine(this, RIGHT_EDGE, Y, IID_YELLOW_BORDER_LINE);
        m_actors.push_back(rightBorder);
        lastYellow = rightBorder;
    }

    int M = VIEW_HEIGHT / (4 * SPRITE_HEIGHT);
    //left white border line
    for (int i = 0; i < M; i++)
    {
        double Y = i * (4 * SPRITE_HEIGHT);
        Actor* whiteBorder = new BorderLine(this, LEFT_X, Y, IID_WHITE_BORDER_LINE);
        m_actors.insert(m_actors.begin(), whiteBorder);
        lastWhite = whiteBorder;
    }
    //right white border line
    for (int i = 0; i < M; i++)
    {
        double Y = i * (4 * SPRITE_HEIGHT);
        Actor* whiteBorder = new BorderLine(this, RIGHT_X, Y, IID_WHITE_BORDER_LINE);
        m_actors.insert(m_actors.begin(), whiteBorder);
        lastWhite = whiteBorder;
    }

    setGameStatText("hihihihihih");

    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    m_player->doSomething();
    //tell all actors in the world to do something
    for (int i = 0; i < m_actors.size(); i++)
    {
        m_actors[i]->doSomething();
        if (!m_player->isAlive())
            return GWSTATUS_PLAYER_DIED;
        //check if ghost racer has saved required number of souls
    }

    //delete actors that have died
    for (int i = 0; i < m_actors.size(); i++)
    {
        if (!m_actors[i]->isAlive())
        {
            delete m_actors[i];
            m_actors.erase(m_actors.begin() + i);
        }
    }

    //add new actors
    double delta_y;
    int new_border_y = VIEW_HEIGHT - SPRITE_HEIGHT;
    delta_y = new_border_y - lastYellow->getY();
    if (delta_y >= SPRITE_HEIGHT)
    {
        //add yellow border lines
        Actor* yellowBorder = new BorderLine(this, LEFT_EDGE, new_border_y, IID_YELLOW_BORDER_LINE);
        m_actors.push_back(yellowBorder);
        yellowBorder = new BorderLine(this, RIGHT_EDGE, new_border_y, IID_YELLOW_BORDER_LINE);
        m_actors.push_back(yellowBorder);
        lastYellow = yellowBorder;
    }

    delta_y = new_border_y - lastWhite->getY();
    if (delta_y >= SPRITE_HEIGHT * 4)
    {
        //add white border lines
        Actor* whiteBorder = new BorderLine(this, LEFT_X, new_border_y, IID_WHITE_BORDER_LINE);
        m_actors.push_back(whiteBorder);
        whiteBorder = new BorderLine(this, RIGHT_X, new_border_y, IID_WHITE_BORDER_LINE);
        m_actors.push_back(whiteBorder);
        lastWhite = whiteBorder;
    }

    //add lost souls
    int lostSoulChance = 100;
    if (createNewActor(lostSoulChance))
    {
        Actor* lostSoul = new LostSoul(this, randInt(LEFT_EDGE, RIGHT_EDGE), VIEW_HEIGHT);
        m_actors.push_back(lostSoul);
    }

    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    delete m_player;
    while (!m_actors.empty())
    {
        delete m_actors[0];
        m_actors.erase(m_actors.begin());
    }
}

StudentWorld::~StudentWorld()
{
    cleanUp();
}

bool createNewActor(int genChance)
{
    int n = randInt(0, genChance);
    return (n == 0);
}