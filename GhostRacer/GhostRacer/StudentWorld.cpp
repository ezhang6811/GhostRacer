#include "StudentWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>
#include <sstream>

using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath), m_player(nullptr), lastYellow(nullptr), lastWhite(nullptr), soulsToSave(0), bonusScore(0)
{
}

int StudentWorld::init()
{
    m_player = new GhostRacer(this, 128, 32);

    //add borderline objects

    int N = VIEW_HEIGHT / SPRITE_HEIGHT;
    // yellow borderlines
    for (int i = 0; i < N; i++)
    {
        double Y = i * SPRITE_HEIGHT;
        Actor* leftBorder = new BorderLine(this, LEFT_EDGE, Y, IID_YELLOW_BORDER_LINE);
        addActor(leftBorder);
        Actor* rightBorder = new BorderLine(this, RIGHT_EDGE, Y, IID_YELLOW_BORDER_LINE);
        addActor(rightBorder);
        lastYellow = rightBorder;
    }

    int M = VIEW_HEIGHT / (4 * SPRITE_HEIGHT);
    //left white border line
    for (int i = 0; i < M; i++)
    {
        double Y = i * (4 * SPRITE_HEIGHT);
        Actor* whiteBorder = new BorderLine(this, LEFT_X, Y, IID_WHITE_BORDER_LINE);
        addActor(whiteBorder);
        lastWhite = whiteBorder;
        whiteBorder = new BorderLine(this, RIGHT_X, Y, IID_WHITE_BORDER_LINE);
        addActor(whiteBorder);
        lastWhite = whiteBorder;
    }

    soulsToSave = getLevel() * 2 + 5;
    bonusScore = 5000;

    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    m_player->doSomething();

    //tell all actors in the world to do something, delete dead actors.
    for (int i = 0; i < m_actors.size(); i++)
    {
        if (m_actors[i]->isAlive())
        {
           m_actors[i]->doSomething();
           if (!m_player->isAlive())
           {
               decLives();
               return GWSTATUS_PLAYER_DIED;
           }
           if (soulsToSave == 0)
           {
               increaseScore(bonusScore);
               return GWSTATUS_FINISHED_LEVEL;
           }
        }
        else
        {
            delete m_actors[i];
            m_actors.erase(m_actors.begin() + i);
        }
    }

    //add new border lines

    double delta_y;
    int new_border_y = VIEW_HEIGHT - SPRITE_HEIGHT;
    delta_y = new_border_y - lastYellow->getY();
    if (delta_y >= SPRITE_HEIGHT)
    {
        //add yellow border lines
        Actor* yellowBorder = new BorderLine(this, LEFT_EDGE, new_border_y, IID_YELLOW_BORDER_LINE);
        addActor(yellowBorder);
        yellowBorder = new BorderLine(this, RIGHT_EDGE, new_border_y, IID_YELLOW_BORDER_LINE);
        addActor(yellowBorder);
        lastYellow = yellowBorder;
    }

    delta_y = new_border_y - lastWhite->getY();
    if (delta_y >= SPRITE_HEIGHT * 4)
    {
        //add white border lines
        Actor* whiteBorder = new BorderLine(this, LEFT_X, new_border_y, IID_WHITE_BORDER_LINE);
        addActor(whiteBorder);
        whiteBorder = new BorderLine(this, RIGHT_X, new_border_y, IID_WHITE_BORDER_LINE);
        addActor(whiteBorder);
        lastWhite = whiteBorder;
    }

    //add Zombie Cabs
    int vehicleChance = max(100 - getLevel() * 10, 20);
    if (chanceNewActor(vehicleChance))
    {
        bool laneChosen[3] = { false, false, false };
        int cur_lane = randInt(1, 3);

        int addZombieCab = -1;
        double vehicleStartX;
        double vehicleStartY;
        int vehicleVertSpeed = getPlayer()->getVertSpeed();
        for (int i = 0; i < 3; i++)
        {
            if (!closestToBottom((cur_lane + i) % 3) || closestToBottom((cur_lane + i) % 3)->getY() > VIEW_HEIGHT / 3)
            {
                addZombieCab = (cur_lane + i) % 3;
                vehicleStartY = SPRITE_HEIGHT / 2;
                vehicleVertSpeed += randInt(2, 4);
                break;
            }
            if (!closestToTop((cur_lane + i) % 3) || closestToTop((cur_lane + i) % 3)->getY() < VIEW_HEIGHT * 2 / 3)
            {
                addZombieCab = (cur_lane + i) % 3;
                vehicleStartY = VIEW_HEIGHT - SPRITE_HEIGHT / 2;
                vehicleVertSpeed -= randInt(2, 4);
                break;
            }
        }

        if (addZombieCab)
        {
            if (addZombieCab == 1)
                vehicleStartX = ROAD_CENTER - ROAD_WIDTH / 3;
            else if (addZombieCab == 2)
                vehicleStartX = ROAD_CENTER;
            else if (addZombieCab == 3)
                vehicleStartX = ROAD_CENTER + ROAD_WIDTH / 3;

            Actor* zombieCab = new ZombieCab(this, vehicleStartX, vehicleStartY, vehicleVertSpeed);
            addActor(zombieCab);
        }

    }

    //add oil slicks
    int oilSlickChance = max(150 - getLevel() * 10, 40);
    if (chanceNewActor(oilSlickChance))
    {
        Actor* oilSlick = new OilSlick(this, randInt(LEFT_EDGE, RIGHT_EDGE), VIEW_HEIGHT);
        addActor(oilSlick);
    }

    //add Zombie Peds
    int zombiePedChance = max(100 - getLevel() * 10, 40);
    if (chanceNewActor(zombiePedChance))
    {
        Actor* zombiePed = new ZombiePedestrian(this, randInt(0, VIEW_WIDTH), VIEW_HEIGHT);
        addActor(zombiePed);
    }

    //add Human Peds
    int humanPedChance = max(200 - getLevel() * 10, 30);
    if (chanceNewActor(zombiePedChance))
    {
        Actor* humanPed = new HumanPedestrian(this, randInt(0, VIEW_WIDTH), VIEW_HEIGHT);
        addActor(humanPed);
    }

    //add holy water refills
    int holyWaterChance = 100 + 10 * getLevel();
    if (chanceNewActor(holyWaterChance))
    {
        Actor* holyWaterGoodie = new HolyWaterGoodie(this, randInt(LEFT_EDGE, RIGHT_EDGE), VIEW_HEIGHT);
        addActor(holyWaterGoodie);
    }

    //add lost souls
    int lostSoulChance = 100;
    if (chanceNewActor(lostSoulChance))
    {
        Actor* lostSoul = new LostSoul(this, randInt(LEFT_EDGE, RIGHT_EDGE), VIEW_HEIGHT);
        addActor(lostSoul);
    }


    //update game text
    //score, lvl, souls2save, lives, health, sprays, bonus
    ostringstream gameText;
    bonusScore--;

    gameText << "Score: " << getScore() << "  Lvl: " << getLevel() << "  Souls2Save: " << soulsToSave << "  Lives: " << getLives();
    gameText << "  Health: " << getPlayer()->getHP() << "  Sprays: " << getPlayer()->getHolyWater() << "  Bonus: " << bonusScore;

    setGameStatText(gameText.str());

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

void StudentWorld::addActor(Actor* a)
{
    m_actors.push_back(a);
}

void StudentWorld::recordSoulSaved()
{
    soulsToSave--;
}

int StudentWorld::findLane(Actor* a)
{
    if (a->getX() < LEFT_X)
        return 1;
    else if (a->getX() < RIGHT_X)
        return 2;
    return 3;
}

Actor* StudentWorld::closestAbove(Actor* a)
{
    double shortestDist = VIEW_HEIGHT;
    Actor* closestActor = nullptr;

    int lane = findLane(a);

    for (int i = 0; i < m_actors.size(); i++)
    {
        if (m_actors[i]->isCollisionAvoidanceWorthy() && findLane(m_actors[i]) == lane && m_actors[i]->getY() > a->getY())
        {
            if (m_actors[i]->getY() - a->getY() < shortestDist)
            {
                closestActor = m_actors[i];
                shortestDist = m_actors[i]->getY() - a->getY();
            }
        }
    }
    return closestActor;
}

Actor* StudentWorld::closestBelow(Actor* a)
{
    double shortestDist = VIEW_HEIGHT;
    Actor* closestActor = nullptr;

    int lane = findLane(a);

    for (int i = 0; i < m_actors.size(); i++)
    {
        if (m_actors[i]->isCollisionAvoidanceWorthy() && findLane(m_actors[i]) == lane && m_actors[i]->getY() < a->getY())
        {
            if (a->getY() - m_actors[i]->getY() < shortestDist)
            {
                closestActor = m_actors[i];
                shortestDist = a->getY() - m_actors[i]->getY();
            }
        }
    }
    if (findLane(m_player) == lane && a->getY() - m_player->getY() < shortestDist)
        closestActor = m_player;

    return closestActor;
}

Actor* StudentWorld::closestToTop(int lane)
{
    Actor* topActor = nullptr;
    double biggestY = 0;
    for (int i = 0; i < m_actors.size(); i++)
    {
        if (m_actors[i]->isCollisionAvoidanceWorthy() && findLane(m_actors[i]) == lane)
        {
            if (m_actors[i]->getY() > biggestY)
            {
                topActor = m_actors[i];
                biggestY = m_actors[i]->getY();
            }
        }
    }
    return topActor;
}

Actor* StudentWorld::closestToBottom(int lane)
{
    Actor* bottomActor = nullptr;
    double smallestY = VIEW_HEIGHT;
    for (int i = 0; i < m_actors.size(); i++)
    {
        if (m_actors[i]->isCollisionAvoidanceWorthy() && findLane(m_actors[i]) == lane)
        {
            if (m_actors[i]->getY() < smallestY)
            {
                bottomActor = m_actors[i];
                smallestY = m_actors[i]->getY();
            }
        }
    }

    if (findLane(m_player) == lane && m_player->getY() < smallestY)
        bottomActor = m_player;

    return bottomActor;
}

bool StudentWorld::sprayFirstAppropriateActor(Actor* a)
{
    for (int i = 0; i < m_actors.size(); i++)
    {
        if (a->overlap(m_actors[i]) && m_actors[i]->beSprayedIfAppropriate())
        {
            return true;
        }
    }
    return false;
}

bool chanceNewActor(int genChance)
{
    int n = randInt(0, genChance);
    return (n == 0);
}