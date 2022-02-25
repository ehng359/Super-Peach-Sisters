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

  void setLevelStatus(std::string status);

  void bonkActor(int x, int y);
  
  // Altering Peach
  void decPeachLives();
  void incPeachLives();
  void bonkPeach();
  bool hasPeachPower(int type) const;
  void setPeachPower(int type);
  int getPeachLocation() const;
  int getPeachHeight() const;

  void releaseGoodie(int x, int y, char goodie);
  void releaseProjectile(int x, int y, int dir, char type);
  bool isBlockingOrOverlapAt(int x, int y, char type);
  char getObjectTypeAt(int x, int y, char notType);
  void damageObjectAt(int x, int y);

private:
	bool isPosOverlap(int x, int y, Actor const* a);
	Level m_l;
	std::vector<Actor*> actorV;
	Peach* p;
	int numDead;
	std::string levelCompleted;
};
#endif // STUDENTWORLD_H_

//Hint: Since your StudentWorld class holds all of your game’s actors, it makes sense to add a public
//method to it that can be used by all of your actors to determine if a given slot is occupied by a blocking
//object.For instance : if (studentWorldPtr->isBlockingOrOverlapAt(x, y)) { … }