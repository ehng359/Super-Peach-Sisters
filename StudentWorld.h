#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Level.h"
#include <string>
#include <vector>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp
class Actor;
class Peach;
class Goodie;
class StudentWorld : public GameWorld
{
public:
  StudentWorld(std::string assetPath);
  ~StudentWorld();

  virtual int init();
  virtual int move();
  virtual void cleanUp();

  void setLevelStatus(std::string status);	// Adjusts the current level status dependent on whether the flag/mario has been reached.

  void bonkActor(int x, int y);
  
  // Altering Peach
  void decPeachLives();	// Decrements Peaches lives.
  void incPeachLives();	// Increments Peaches lives.
  void bonkPeach();		// Directly calls bonk() on Peach.
  bool hasPeachPower(int type) const;	// Determines whetehr or not Peach has a particular power (star = 0, shoot = 1, jump = 2)
  void setPeachPower(int type);	// Sets a Peach power depending on the integer input provided (star = 0, shoot = 1, jump = 2)
  void damagePeach();	// Directly calls getDamaged() on Peach.
  int getPeachX() const;	// Returns Peach's x location.
  int getPeachY() const;	// Returns Peach's y location.

  void releaseGoodie(int x, int y, char goodie);	// Releases a goodie above an item block that was recently bonked.
  void releaseProjectile(int x, int y, int dir, char type);	// Releases a projectile to move inside the game world.
  bool isBlockingOrOverlapAt(int x, int y, char type);	// Determines whether an object is blocking or overlapping (depending on the type indicated).
  char getObjectTypeAt(int x, int y, char notType);	// Returns the object type of an actor at a location based on characteristics and "non-type" filter.
  void damageObjectAt(int x, int y);	// Damages an object at the location x, y, if even damageable.

private:
	bool isPosOverlap(int x, int y, Actor const* a);	// Helper function to determine collision/hitboxes (considering the left-bottom, right-bottom, left-top and right-top ranges).
	std::vector<Actor*> actorV;	// Container for all actors in the game.
	Peach* p;	// Pointer for holding Peach.
	std::string levelCompleted;	// Indicator for whether or not a level (or the game) has been completed.
};
#endif // STUDENTWORLD_H_