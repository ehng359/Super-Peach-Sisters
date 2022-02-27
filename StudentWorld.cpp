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
    : GameWorld(assetPath), p(nullptr), levelCompleted("") {}

StudentWorld::~StudentWorld() {
    cleanUp();  // Calls the clean up function in order to remove remaining actors from the actor container.
}

void StudentWorld::setLevelStatus(std::string status) {
    levelCompleted = status;    // Sets the level status to the status held by the objective (LVL_COMPLETE for flags and GAME_COMPLETE for Mario).
}

void StudentWorld::decPeachLives() {
    p->setLives(p->getHP() - 1);        // Decrements Peach's current lives by 1.
}

void StudentWorld::incPeachLives() {
    if (p->getHP() == 1)                // Incrementss Peach's current lives by 1.
        p->setLives(p->getHP() + 1);
}

bool StudentWorld::hasPeachPower(int type) const {
    return p->hasPower(type);           //  Returns true if Peach has the power associated to the integer type and false otherwise.
}                                       // Star = 0, Shoot = 1, Jump = 2.

void StudentWorld::setPeachPower(int type) {    // Sets Peach's power depending on the associated integer type.
    if (p->hasPower(type) && type != 0)         // Star = 0, Shoot = 1, Jump = 2.
        return;                                 // Function returns automatically if Peach already has the associated power (unless if it
    p->setPower(type);                          // is star).
}

void StudentWorld::damagePeach() {  // Attempts to directly damage Peach.
    p->getDamaged();
}

int StudentWorld::getPeachY() const {   // Returns Peach's y-location.
    return p->getY();
}

int StudentWorld::getPeachX() const {   // Returns Peach's x-location.
    return p->getX();
}

// Helper function that compares the hitbox surrounding an x and y coordinate for one actor and the x and y coordinate for actor a.
bool StudentWorld::isPosOverlap(int x, int y, Actor const * a) {
    int upper_pos = y + SPRITE_HEIGHT - 1;
    int right_pos = x + SPRITE_WIDTH - 1;   // Calculates the upper position as well as the right most position.
    if (((x <= (a->getX() + SPRITE_WIDTH - 1) && x >= a->getX() && ((y <= (a->getY() + SPRITE_HEIGHT - 1) && y >= (a->getY()))              // If bottom-left and upper-left corners are within the actor's range or
        || (upper_pos <= a->getY() + SPRITE_HEIGHT - 1 && upper_pos >= a->getY())))                                                         // the bottom-right and upper-right corners of x are within the actor's range,
        || (right_pos <= a->getX() + SPRITE_WIDTH - 1 && right_pos >= a->getX() && ((y <= (a->getY() + SPRITE_HEIGHT - 1) && y >= a->getY())// return true to indicate overlap and false otherwise.
        || (upper_pos <= a->getY() + SPRITE_HEIGHT - 1 && upper_pos >= a->getY())))))
        return true;
    return false;
}

bool StudentWorld::isBlockingOrOverlapAt(int x, int y, char type) {
    switch (type) {
    case 'b':   // In the case of blocking: 
        for (vector<Actor*>::iterator it = actorV.begin(); it != actorV.end(); it++)    // For each actor, if the actor prevents movement and is overlapping
            if ((*it)->preventsMovement() && isPosOverlap (x, y, *it))                  // with an actor's potentially new location, then return true (indicating
                return true;                                                            // that the area is being blocked).
        break;
    case 'o':   // In the case of overlap:
        int count = 0;                                                                  // For each actor,
        for (vector<Actor*>::iterator it = actorV.begin(); it != actorV.end(); it++)
            if (isPosOverlap (x, y, *it) && (*it)->isAlive()) {                         // increment a count if an actor is within range of x and y.
                count++;
            }
        if (isPosOverlap(x, y, p))  // If the actor at position x, y is overlapping with Peach, increment by 1.
            count++;
        if (count > 1)  // If that count is greater than 1, meaning there are more than 1 actors in that spot, return overlap to be true.
            return true;
        break;
    }
    return false;   // Otherwise if there is no blocking nor overlap (depending on the type), return having blocking or overlap to be false.
}

char StudentWorld::getObjectTypeAt(int x, int y, char notType){
    vector<Actor*>::iterator it = actorV.begin();
    if (isPosOverlap(x, y, p) && notType != 'p') {  // If the x, y location is overlapping with Peach's location and the "filter" notType isn't 'p' for Peach,
        return 'p';                                 // return 'p'.
    }
    for (; it != actorV.end(); it++) {  // For each actor in the game:
        switch (notType) {
        case 'p':
            if (isPosOverlap(x, y, *it) && (*it)->isDamageable() && (*it)->isAlive())   // If the position x, y is overlapping with an actor, and such actor is alive/damageable,
                return 'e';                                                             // return 'e' for enemy as it directly matches an "enemy's" descriptors.
            break;
        default:
            break;
        }
    }
    return '0'; // If no such object was found, then we return char '0' to indicate no different object type found.
}


void StudentWorld::releaseGoodie(int x, int y, char goodie) {
    switch (goodie) {   // Depending on the goodie type ('f' for flower, 'm' for mushroom and 's' for star),
    case 'f':
        actorV.push_back(new Flower(x, y + 8, this));   // Push back a new goodie item onto the actor vector altogether,
        break;                                          // one block above the item block.
    case 'm':
        actorV.push_back(new Mushroom(x, y + 8, this));
        break;
    case 's':
        actorV.push_back(new Star(x, y + 8, this));
        break;
    }
}

void StudentWorld::releaseProjectile(int x, int y, int dir, char type) {
    switch (type) { // Depending on the projectile type ('s' for shell, 'p' for peach_fireball, and 'e' for piranha_fireball)
    case 's': // case shell
        actorV.push_back(new Shell(x, y, dir, this));   // Push back a new projectile item onto the actor vector altogether,
        break;                                          // made in the direction of the parent actor introducing the object.
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
        if (isPosOverlap(x, y, *it)) {  // For each actor in the vector, if it overlaps with the coordinate provided,
            (*it)->bonk();              // bonk the actor.
        }
    }
}

void StudentWorld::bonkPeach() {
    p->bonk();  // Bonk Peach.
}

void StudentWorld::damageObjectAt(int x, int y) {
    for (vector<Actor*>::iterator it = actorV.begin(); it != actorV.end(); it++) {  // For each actor in the vector, if it overlaps with the coordinate provided,
        if ((*it)->isDamageable() && isPosOverlap(x, y, *it) && (*it)->isAlive()) { // damage the actor.
            (*it)->getDamaged();
        }
    }
}

int StudentWorld::init()
{
    Level lev(assetPath());
    ostringstream newLevel;
    newLevel << "level0" << getLevel() << ".txt";   // Loads the current level of the game.
    string level_file = newLevel.str();
    Level::LoadResult result = lev.loadLevel(level_file);
    if (result == Level::load_fail_file_not_found) {                                // If the whole level cannot be found,
        cerr << "Could not find level0" << getLevel() << ".txt data file" << endl;  // return a level error.
        return GWSTATUS_LEVEL_ERROR;
    }
    else if (result == Level::load_fail_bad_format) {                               // If the level is improperly formatted (i.e no surrounding blocks)
        cerr << "level0" << getLevel() << ".txt is improperly formatted" << endl;   // return a level error.
        return GWSTATUS_LEVEL_ERROR;
    }
    else if (result == Level::load_success)     // If the load is otherwise successful:
    {
        cerr << "Successfully loaded level" << endl;
        Level::GridEntry ge;
        int dir;
        for (int w = 0; w < GRID_WIDTH; w++) {              // For each row and column of the level file's grid,
            for (int h = 0; h < GRID_WIDTH; h++) {          // push each actor (block, enemy, Peach, flag, etc) into the
                ge = lev.getContentsOf(w, h); // x=5, y=10  // overarching world to load up the game/assets.
                switch (ge)
                {
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
    return GWSTATUS_CONTINUE_GAME;  // Return to continue the game.
}

int StudentWorld::move()
{
    if (p->isAlive()) {     // If Peach is alive, have her perform some action.
        p->doSomething();
    }
    for (vector<Actor*>::iterator it = actorV.begin(); it != actorV.end(); it++) {
        if ((*it)->isAlive()) {                         // For each actor in the container,
            (*it)->doSomething();                       // if the actor is alive, have the actor do something specific to their roles.
            if (!p->isAlive() || getLives() == 0) {     // If Peach dies or total lives drop to 0, decrement the gameLives and return the
                this->playSound(SOUND_PLAYER_DIE);      // status indicating that the player died.
                decLives();
                return GWSTATUS_PLAYER_DIED;
            }
            if (levelCompleted == "LVL_COMPLETE") {     // If the status of the level becomes completed,
                levelCompleted = "";                    // play the sound for the having finished a level
                playSound(SOUND_FINISHED_LEVEL);        // and return the status indicating passing the level.
                return GWSTATUS_FINISHED_LEVEL;
            }
            if (levelCompleted == "GAME_COMPLETE") {
                playSound(SOUND_GAME_OVER);             // If the status of the level indicates that the game has been completed,
                return GWSTATUS_PLAYER_WON;             // play the sound for having finished the game and return an indication for
            }                                           // having beaten it.
        }
    }
    for (vector<Actor*>::iterator it = actorV.begin(); it != actorV.end();) {   // For each dead actor, delete their allocated memory
            if (!((*it)->isAlive())) {                                          // and remove them from the vector container.
                delete(*it);
                it = actorV.erase(it);
            }
            else
                it++;
    }

    // Update the player information including lives, current level, total points, and currently holding powerups.
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

    // Continue the game.
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    if (p != nullptr) { // If Peach has not already been deleted and set to nullptr,
        delete p;       // delete Peach and set the pointer to nullptr.
        p = nullptr;
    }
    if (actorV.begin() != actorV.end())
        for (std::vector<Actor*>::iterator it = actorV.begin(); it != actorV.end();) {  // For each actor in the vector container,
            delete(*it);                                                                // delete the actor.
            it = actorV.erase(it);
        }
}
