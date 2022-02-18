#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
#include <vector>

#include <iostream>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp

StudentWorld::StudentWorld(string assetPath) : GameWorld(assetPath), m_l(Level(assetPath)), p(nullptr), numDead(0){}

StudentWorld::~StudentWorld() {
    cleanUp();
}

bool StudentWorld::isBlockingObjectAt(int x, int y) {
    for (vector<Actor*>::iterator it = actorV.begin(); it != actorV.end(); it++) {
        if (x < ((*it)->getX() + SPRITE_WIDTH) && x >= (*it)->getX() - 4 && y < ((*it)->getY() + SPRITE_HEIGHT) && y >= ((*it)->getY() - 4) && (*it)->preventsMovement()) {
            return true;
        }
    }
    return false;
}

void StudentWorld::releaseGoodie(Actor* goodie) {
    actorV.push_back(goodie);
}

void StudentWorld::bonkActor(int x, int y) {
    for (vector<Actor*>::iterator it = actorV.begin(); it != actorV.end(); it++) {
        if (x < ((*it)->getX() + SPRITE_WIDTH) && x >= (*it)->getX() - 4 && y < ((*it)->getY() + SPRITE_HEIGHT) && y >= ((*it)->getY() - 4) && (*it)->preventsMovement()) {
            (*it)->bonk();
        }
    }
}

int StudentWorld::init()
{
    /*int load = m_l.loadLevel(assetPath());
    if (load == Level::load_success)
        return GWSTATUS_CONTINUE_GAME;
    return GWSTATUS_LEVEL_ERROR;*/
    Level lev(assetPath());
    string level_file = "level04.txt";
    Level::LoadResult result = lev.loadLevel(level_file);
    if (result == Level::load_fail_file_not_found) {
        cerr << "Could not find level01.txt data file" << endl;
        return GWSTATUS_LEVEL_ERROR;
    }
    else if (result == Level::load_fail_bad_format) {
        cerr << "level01.txt is improperly formatted" << endl;
        return GWSTATUS_LEVEL_ERROR;
    }
    else if (result == Level::load_success)
    {
        cerr << "Successfully loaded level" << endl;
        Level::GridEntry ge;
        for (int w = 0; w < GRID_WIDTH; w++) {
            for (int h = 0; h < GRID_WIDTH; h++) {
                ge = lev.getContentsOf(w, h); // x=5, y=10
                switch (ge)
                {
                case Level::empty:
                    cout << "Location " << w << "," << h << " is empty" << endl;
                    break;
                case Level::koopa:
                    cout << "Location " << w << "," << h << " starts with a koopa" << endl;
                    break;
                case Level::goomba:
                    cout << "Location " << w << "," << h << " starts with a goomba" << endl;
                    break;
                case Level::peach:
                    cout << "Location " << w << "," << h << " is where Peach starts" << endl;
                    p = new Peach(w * SPRITE_HEIGHT, h * SPRITE_HEIGHT, this);
                    break;
                case Level::flag:
                    cout << "Location " << w << "," << h << " is where a flag is" << endl;
                    break;
                case Level::block:
                    cout << "Location " << w << "," << h << " holds a regular block" << endl;
                    actorV.push_back(new Block(w * SPRITE_WIDTH, h * SPRITE_HEIGHT, nullptr, this));
                    break;
                case Level::star_goodie_block:
                    cout << "Location " << w << "," << h << " has a star goodie block" << endl;
                    break;
                    // etc…
                }
            }
        }
    }
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    // This code is here merely to allow the game to build, run, and terminate after you hit enter.
    // Notice that the return value GWSTATUS_PLAYER_DIED will cause our framework to end the current level.
    if (p->isAlive()) {
        p->doSomething();
    }
    for (vector<Actor*>::iterator it = actorV.begin(); it != actorV.end(); it++) {
        if ((*it)->isAlive()) {
            (*it)->doSomething();
            if (!p->isAlive()) {
                this->playSound(SOUND_PLAYER_DIE);
                return GWSTATUS_PLAYER_DIED;
            }
            if (m_l.getContentsOf(p->getX(), p->getY()) == Level::flag) {
                playSound(SOUND_FINISHED_LEVEL);
                return GWSTATUS_FINISHED_LEVEL;
            }
            if (m_l.getContentsOf(p->getX(), p->getY()) == Level::mario) {
                playSound(SOUND_GAME_OVER);
                return GWSTATUS_PLAYER_WON;
            }
        }
        else {
            numDead++;
        }
    }
    
    for (vector<Actor*>::iterator it = actorV.begin(); it != actorV.end(); it++) {
        if (!((*it)->isAlive())) {
            delete(*it);
            it = actorV.erase(it);
        }
    }

    // Update the score

    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    if (actorV.begin() != actorV.end())
        for (std::vector<Actor*>::iterator it = actorV.begin(); it != actorV.end(); it++) {
            delete(*it);
            it = actorV.erase(it);
        }
}
