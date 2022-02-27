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
	virtual void getDamaged() {};
	virtual bool isDamageable() const { return false; };
	virtual void bonk() {};

	StudentWorld* getWorld() const;
private:
	bool m_alive;
	StudentWorld* m_sw;
};

class Inanimate : public Actor {
public:
	Inanimate(int ID, int x, int y, int dir, int depth, double size, StudentWorld* sw);
private:
};

class Animate : public Actor {
public:
	Animate(int ID, int x, int y, int dir, int depth, double size, StudentWorld* sw);
private:
};

class Peach : public Animate {
public:
	Peach(int x, int y, StudentWorld* sw);
	bool isInvincible() const;
	bool getHP() const;
	bool isDamageable() const { return true;  }
	bool hasPower(int type) const; // star = 0, shoot = 1, jump = 2
	void setPower(int type);
	void setLives(int value);

	virtual void doSomething();
	virtual void bonk();
private:
	int time_to_recharge_before_next_fire= 0;
	int curr_HP, remaining_Invincibility, remaining_jump_distance;
	bool inv_power, jump_power, shoot_power, temp_inv;
};

class Enemy : public Animate {
public:
	Enemy(int ID, int x, int y, int dir, int depth, double size, StudentWorld* sw);
	virtual void doSomething();
	virtual bool isDamageable() const;
	virtual bool overlapWithPeach();
	virtual void getDamaged();
	virtual void bonk();
};

	class Goomba : public Enemy {
	public:
		Goomba(int x, int y, int dir, StudentWorld *sw);
	};

	class Koopa : public Enemy {
	public:
		Koopa(int x, int y, int dir, StudentWorld* sw);
		virtual void getDamaged();
	};

	class Piranha : public Enemy {
	public:
		Piranha(int x, int y, int dir, StudentWorld* sw);
		virtual void doSomething();
	private:
		int firing_delay;
	};

class Projectile : public Animate {
public:
	Projectile(int ID, int x, int y, int dir, int depth, double size, StudentWorld* sw);
	virtual void doSomething();
	virtual bool isFriendly() {
		return true;
	}
};

	class Shell : public Projectile {
	public:
		Shell(int x, int y, int dir, StudentWorld* sw);
	};

	class Peach_Fireball : public Projectile {
	public:
		Peach_Fireball(int x, int y, int dir, StudentWorld* sw);
	};

	class Piranha_Fireball : public Projectile {
	public:
		Piranha_Fireball(int x, int y, int dir, StudentWorld* sw);
		virtual bool isFriendly() {
			return false;
		}
	};

class Goodie : public Animate {
public:
	Goodie(int ID, int x, int y, int dir, int depth, double size, int p_type, int p_value, StudentWorld* sw);
	virtual bool isDamageable() const;
	virtual void doSomething();
private:
	int type;
	int point_value;
};

	class Flower : public Goodie {
	public:
		Flower(int x, int y, StudentWorld* sw);
	};

	class Mushroom : public Goodie {
	public:
		Mushroom(int x, int y, StudentWorld* sw);
	};

	class Star : public Goodie {
	public:
		Star(int x, int y, StudentWorld* sw);
	};

class Block : public Inanimate {
public:
	Block(int x, int y, StudentWorld* sw);
	Block(int x, int y, char goodie, StudentWorld* sw);
	virtual void doSomething();
	virtual bool preventsMovement() const;
	virtual void bonk();
private:
	bool containsGoodie;
	char goodie;
};

class Pipe : public Block {
public:
	Pipe(int x, int y, StudentWorld* sw);
};

class Objective : public Inanimate {
public:
	Objective(int ID, int x, int y, int dir, int depth, double size, std::string compType, StudentWorld* sw);
	virtual void doSomething();
private:
	std::string completionType;
};

class Flag : public Objective {
public:
	Flag(int x, int y, StudentWorld* sw);
};

class Mario : public Objective {
public:
	Mario(int x, int y, StudentWorld* sw);
};
#endif // ACTOR_H_