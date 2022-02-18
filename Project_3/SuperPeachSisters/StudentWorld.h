#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Level.h"
#include <string>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp
#include "Actor.h"
#include <vector>

class StudentWorld : public GameWorld
{
public:
  StudentWorld(std::string assetPath);
  ~StudentWorld();
  virtual int init();
  virtual int move();
  virtual void cleanUp();
  void bonkActor(int x, int y);
  void releaseGoodie(Actor* Goodie);
  bool isBlockingObjectAt(int x, int y);
private:
	Level m_l;
	std::vector<Actor*> actorV;
	Peach* p;
	int numDead;
};
#endif // STUDENTWORLD_H_

//Hint: Since your StudentWorld class holds all of your game’s actors, it makes sense to add a public
//method to it that can be used by all of your actors to determine if a given slot is occupied by a blocking
//object.For instance : if (studentWorldPtr->isBlockingObjectAt(x, y)) { … }