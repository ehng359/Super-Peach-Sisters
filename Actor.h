#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
class StudentWorld;

// The base actor class for all objects: blocks, enemies, projectiles, etc.
class Actor : public GraphObject {
public:
	Actor(int ID, int x, int y, int dir, int depth, double size, StudentWorld* sw);
	bool isAlive() const;	// Checks whether an actor is alive (all start off alive).
	void setDead();			// Sets an actor's state to dead.

	virtual bool preventsMovement() const;	// Checks whether an actor prevents movement (blocks/pipes)

	virtual void doSomething() = 0;		// Has each actor do something every turn.
	virtual void getDamaged() {};		// Damages an actor if it's damageable.
	virtual bool isDamageable() const { return false; };	// Returns whether an actor is damageable.
	virtual void bonk() {};

	StudentWorld* getWorld() const;		// Returns the student world for actors to access its public methods.
private:
	bool m_alive;
	StudentWorld* m_sw;
};

class Peach : public Actor {
public:
	Peach(int x, int y, StudentWorld* sw);
	bool isInvincible() const;		// Checks whether or not Peach is invincible.
	bool getHP() const;				// Returns Peach's current HP.
	bool isDamageable() const { return true;  }		// Returns true if any method call attempts to determine whether Peach is damageable.
	bool hasPower(int type) const;	// Returns whether Peach has a particular powerup (star = 0, shoot = 1, jump = 2)
	void setPower(int type);		// Gives Peach a power (star = 0, shoot = 1, jump = 2)
	void setLives(int value);		// Adjusts Peach's current HP.

	virtual void getDamaged();
	virtual void doSomething();
	virtual void bonk();
private:
	int time_to_recharge_before_next_fire= 0;
	int curr_HP, remaining_Invincibility, remaining_jump_distance;
	bool inv_power, jump_power, shoot_power, temp_inv;
};

class Enemy : public Actor {
public:
	Enemy(int ID, int x, int y, int dir, int depth, double size, StudentWorld* sw);
	virtual bool isDamageable() const;	// Returns true if any method call attempts to determine whether an enemy is damageable.
	virtual bool overlapWithPeach();	// Returns whether or not an enemy is overlapping with Peach.
	virtual void getDamaged();			// Damages an enemy.

	virtual void doSomething();
	virtual void bonk();
};

	class Goomba : public Enemy {
	public:
		Goomba(int x, int y, int dir, StudentWorld *sw);
	};

	class Koopa : public Enemy {
	public:
		Koopa(int x, int y, int dir, StudentWorld* sw);
		virtual void getDamaged();		//	Adjusts the getDamaged() for Koopa to spawn a shell.
	};

	class Piranha : public Enemy {
	public:
		Piranha(int x, int y, int dir, StudentWorld* sw);
		virtual void doSomething();
	private:
		int firing_delay;	// Sets firing delay for the Piranha.
	};

class Projectile : public Actor {
public:
	Projectile(int ID, int x, int y, int dir, int depth, double size, StudentWorld* sw);
	virtual void doSomething();
	virtual bool isFriendly() {		// Returns whether or not a projectile is friendly to Peach.
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
		virtual bool isFriendly() {		// Returns that a Piranha_Fireball is not friendly to Peach (can damage Peach)
			return false;
		}
	};

class Goodie : public Actor {
public:
	Goodie(int ID, int x, int y, int dir, int depth, double size, int p_type, int p_value, StudentWorld* sw);
	virtual void doSomething();
private:
	int type;			// Establishes the type of the powerup.
	int point_value;	// Designates point value for whenever Peach picks up a powerup.
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

class Block : public Actor {
public:
	Block(int x, int y, StudentWorld* sw);					// Constructor specifically for the Pipe.
	Block(int x, int y, char goodie, StudentWorld* sw);		// Constructor specifically for the Block with potential "goodie".
	virtual void doSomething();
	virtual bool preventsMovement() const;					// Returns true for a Block preventing movement.
	virtual void bonk();
private:
	bool containsGoodie;
	char goodie;
};

class Pipe : public Block {
public:
	Pipe(int x, int y, StudentWorld* sw);
};

class Objective : public Actor {
public:
	Objective(int ID, int x, int y, int dir, int depth, double size, std::string compType, StudentWorld* sw);
	virtual void doSomething();
private:
	std::string completionType;		// If a flag is captured, the level is completed but if Mario is saved, the game is won.
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