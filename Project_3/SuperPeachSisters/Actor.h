#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
class StudentWorld;

class Actor : public GraphObject {
public:
	Actor(int ID, int x, int y, int dir, int depth, double size, StudentWorld* sw);
	bool isAlive() const;
	void setDead();
	virtual bool preventsMovement() const;

	virtual void doSomething() = 0;
	virtual bool isDamageable() const = 0;
	virtual void bonk() = 0;

	StudentWorld* getWorld() const;
private:
	bool m_alive; 
	StudentWorld* m_sw;
};

class Inanimate : public Actor {
public:
	Inanimate(int ID, int x, int y, int dir, int depth, double size, StudentWorld* sw);
	virtual bool isDamageable() const;
private:
};

class Animate : public Actor {
public:
	Animate(int ID, int x, int y, int dir, int depth, double size, StudentWorld* sw);
	virtual bool isDamageable() const;
private:
};

class Peach : public Animate {
public:
	Peach(int x, int y, StudentWorld* sw);
	bool isInvicible() const;
	virtual void doSomething();
	virtual void bonk();
private:
	int time_to_recharge_before_next_fire= 0;
	int curr_HP, remaining_Invincibility, remaining_jump_distance;
	bool inv_status;
	std::string power_up;
};

class Enemies : public Animate {
	Enemies(int ID, int x, int y, int dir, int depth, double size, StudentWorld* sw);
};

	class Goomba : public Enemies {

	};

	class Koopa : public Enemies {

	};

	class Piranha : public Enemies {

	};

class Projectiles : public Animate {
	Projectiles(int ID, int x, int y, int dir, int depth, double size, StudentWorld* sw);
};

	class Shell : public Projectiles {

	};

	class Peach_Fireball : public Projectiles {

	};

	class Piranha_Fireball : public Projectiles {

	};

class Goodie : public Animate {
	Goodie(int ID, int x, int y, int dir, int depth, double size, StudentWorld* sw);
	virtual bool isDamageable() const;
};

	class Flower : public Goodie {

	};

	class Mushroom : public Goodie {

	};

	class Star : public Goodie {

	};

class Block : public Inanimate {
public:
	Block(int x, int y, Actor* goodie, StudentWorld* sw);
	virtual void doSomething();
	virtual bool preventsMovement() const;
	virtual void bonk();
private:
	bool containsGoodie;
	Actor* goodie;
};

class Pipe : public Inanimate {
	virtual void bonk();
};

class Flag : public Inanimate {

};

class Mario : public Inanimate {

};
#endif // ACTOR_H_