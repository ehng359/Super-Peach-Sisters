#include "StudentWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <iomanip>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp

StudentWorld::StudentWorld(string assetPath) 
    : GameWorld(assetPath), m_l(Level(assetPath)), p(nullptr), numDead(0), levelCompleted("") {}

StudentWorld::~StudentWorld() {
    cleanUp();
}

void StudentWorld::setLevelStatus(std::string status) {
    levelCompleted = status;
}

void StudentWorld::decPeachLives() {
    p->setLives(p->getHP() - 1);
}

void StudentWorld::incPeachLives() {
    if (p->getHP() == 1)
        p->setLives(p->getHP() + 1);
}

bool StudentWorld::hasPeachPower(int type) const {
    return p->hasPower(type);
}

void StudentWorld::setPeachPower(int type) {
    if (p->hasPower(type) && type != 0)
        return;
    p->setPower(type);
}

int StudentWorld::getPeachHeight() const {
    return p->getY();
}

int StudentWorld::getPeachLocation() const {
    return p->getX();
}

bool StudentWorld::isPosOverlap(int x, int y, Actor const * a) {
    int upper_pos = y + SPRITE_HEIGHT - 1;
    int right_pos = x + SPRITE_WIDTH - 1;
    if (((x <= (a->getX() + SPRITE_WIDTH - 1) && x >= a->getX() && y <= (a->getY() + SPRITE_HEIGHT - 1) && y >= (a->getY()))
        || (right_pos <= a->getX() + SPRITE_WIDTH - 1 && right_pos >= a->getX() && ((y <= (a->getY() + SPRITE_HEIGHT - 1) && y >= a->getY())
            || (upper_pos <= a->getY() + SPRITE_HEIGHT - 1 && upper_pos >= a->getY())))))
        return true;
    return false;
}

bool StudentWorld::isBlockingOrOverlapAt(int x, int y, char type) {
    switch (type) {
    case 'b':
        for (vector<Actor*>::iterator it = actorV.begin(); it != actorV.end(); it++)
            if ((*it)->preventsMovement() && isPosOverlap (x, y, *it))
                return true;
        break;
    case 'o':
        int count = 0;
        for (vector<Actor*>::iterator it = actorV.begin(); it != actorV.end(); it++)
            if (isPosOverlap (x, y, *it) && (*it)->isAlive()) {
                count++;
            }
        if (isPosOverlap(x, y, p))
            count++;
        if (count > 1)
            return true;
        break;
    }
    return false;
}

char StudentWorld::getObjectTypeAt(int x, int y, char notType){
    vector<Actor*>::iterator it = actorV.begin();
    if (isPosOverlap(x, y, p) && notType != 'p') {
        return 'p';
    }
    for (; it != actorV.end(); it++) {
        switch (notType) {
        case 'e':
            break;
        case 'p':
            if (isPosOverlap(x, y, *it) && (*it)->isDamageable() && (*it)->isAlive())
                return 'e';
            break;
        case 'g':
            break;
        }
    }
    return '0';
}


void StudentWorld::releaseGoodie(int x, int y, char goodie) {
    switch (goodie) {
    case 'f':
        actorV.push_back(new Flower(x, y + 8, this));
        break;
    case 'm':
        actorV.push_back(new Mushroom(x, y + 8, this));
        break;
    case 's':
        actorV.push_back(new Star(x, y + 8, this));
        break;
    }
}

void StudentWorld::releaseProjectile(int x, int y, int dir, char type) {
    switch (type) {
    case 's': // case shell
        actorV.push_back(new Shell(x, y, dir, this));
        break;
    case 'p': // case peach
        actorV.push_back(new Peach_Fireball(x, y, dir, this));
        break;
    case 'e': // case enemy
        actorV.push_back(new Piranha_Fireball(x, y, dir, this));
        break;
    }
}

void StudentWorld::bonkActor(int x, int y) {
    for (vector<Actor*>::iterator it = actorV.begin(); it != actorV.end(); it++) {
        if (isPosOverlap(x, y, *it)) {
            (*it)->bonk();
        }
    }
}

void StudentWorld::bonkPeach() {
    p->bonk();
}

void StudentWorld::damageObjectAt(int x, int y) {
    for (vector<Actor*>::iterator it = actorV.begin(); it != actorV.end(); it++) {
        if ((*it)->isDamageable() && isPosOverlap(x, y, *it) && (*it)->isAlive()) {
            (*it)->getDamaged();
        }
    }
}


int StudentWorld::init()
{
    Level lev(assetPath());
    ostringstream newLevel;
    newLevel << "level0" << getLevel() << ".txt";
    string level_file = newLevel.str();
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
        int dir;
        for (int w = 0; w < GRID_WIDTH; w++) {
            for (int h = 0; h < GRID_WIDTH; h++) {
                ge = lev.getContentsOf(w, h); // x=5, y=10
                switch (ge)
                {
                case Level::empty:
                    break;
                case Level::koopa:
                    dir = randInt(0, 180);
                    if (dir > 90)
                        actorV.push_back(new Koopa(w * SPRITE_WIDTH, h * SPRITE_HEIGHT, 180, this));
                    else
                        actorV.push_back(new Koopa(w * SPRITE_WIDTH, h * SPRITE_HEIGHT, 0, this));
                    break;
                case Level::goomba:
                    dir = randInt(0, 180);
                    if (dir > 90)
                        actorV.push_back(new Goomba(w * SPRITE_WIDTH, h * SPRITE_HEIGHT, 180, this));
                    else
                        actorV.push_back(new Goomba(w * SPRITE_WIDTH, h * SPRITE_HEIGHT, 0, this));
                    break;
                case Level::piranha:
                    dir = randInt(0, 180);
                    if (dir > 90)
                        actorV.push_back(new Piranha(w * SPRITE_WIDTH, h * SPRITE_HEIGHT, 180, this));
                    else
                        actorV.push_back(new Piranha(w * SPRITE_WIDTH, h * SPRITE_HEIGHT, 0, this));
                    break;
                case Level::peach:
                    p = new Peach(w * SPRITE_WIDTH, h * SPRITE_HEIGHT, this);
                    break;
                case Level::flag:
                    actorV.push_back(new Flag(w * SPRITE_WIDTH, h * SPRITE_HEIGHT, this));
                    break;
                case Level::block:
                    actorV.push_back(new Block(w * SPRITE_WIDTH, h * SPRITE_HEIGHT, 0, this));
                    break;
                case Level::pipe:
                    actorV.push_back(new Pipe(w * SPRITE_WIDTH, h * SPRITE_HEIGHT, this));
                    break;
                case Level::star_goodie_block:
                    actorV.push_back(new Block(w * SPRITE_WIDTH, h * SPRITE_HEIGHT, 's', this));
                    break;
                case Level::flower_goodie_block:
                    actorV.push_back(new Block(w * SPRITE_WIDTH, h * SPRITE_HEIGHT, 'f', this));
                    break;
                case Level::mushroom_goodie_block:
                    actorV.push_back(new Block(w * SPRITE_WIDTH, h * SPRITE_HEIGHT, 'm', this));
                    break;
                case Level::mario:
                    actorV.push_back(new Mario(w * SPRITE_WIDTH, h * SPRITE_HEIGHT, this));
                    break;
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
            if (!p->isAlive() || getLives() == 0) {
                this->playSound(SOUND_PLAYER_DIE);
                decLives();
                return GWSTATUS_PLAYER_DIED;
            }
            if (levelCompleted == "LVL_COMPLETE") {
                levelCompleted = "";
                playSound(SOUND_FINISHED_LEVEL);
                return GWSTATUS_FINISHED_LEVEL;
            }
            if (levelCompleted == "GAME_COMPLETE") {
                playSound(SOUND_GAME_OVER);
                return GWSTATUS_PLAYER_WON;
            }
        }
        else {
            numDead++;
        }
    }
    for (vector<Actor*>::iterator it = actorV.begin(); it != actorV.end();) {
            if (!((*it)->isAlive())) {
                delete(*it);
                it = actorV.erase(it);
            }
            else
                it++;
    }

    // Update the score
    ostringstream displayText;
    displayText << "Lives: " << getLives() << "  Level: ";
    displayText.fill('0');
    displayText << setw(2) << getLevel();
    displayText << "  Points: " << setw(6) << getScore();
    displayText << " ";
    if (p->hasPower(0))
        displayText << "StarPower! ";
    if (p->hasPower(1))
        displayText << "ShootPower! ";
    if (p->hasPower(2))
        displayText << "JumpPower! ";

    std::string text = displayText.str();
    setGameStatText(text);

    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    if (p != nullptr) {
        delete p;
        p = nullptr;
    }
    if (actorV.begin() != actorV.end())
        for (std::vector<Actor*>::iterator it = actorV.begin(); it != actorV.end();) {
            delete(*it);
            it = actorV.erase(it);
        }
}
