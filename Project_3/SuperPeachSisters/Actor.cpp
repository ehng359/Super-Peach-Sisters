#include "Actor.h"
#include "StudentWorld.h"

#include <iostream>

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

// Actor
Actor::Actor(int ID, int x, int y, int dir, int depth, double size, StudentWorld* sw) 
	: GraphObject(ID, x, y, dir, depth, size), m_alive(true), m_sw(sw) {}

bool Actor::preventsMovement() const {
	return false;
}

void Actor::setDead() {
	m_alive = false;
}

StudentWorld* Actor::getWorld() const {
	return m_sw;
}

bool Actor::isAlive() const{
	return m_alive;
}

// Animate
Animate::Animate(int ID, int x, int y, int dir, int depth, double size, StudentWorld* sw) 
	: Actor(ID, x, y, dir, depth, size, sw) {}

bool Animate::isDamageable() const {
	return true;
}


// Inanimate
Inanimate::Inanimate(int ID, int x, int y, int dir, int depth, double size, StudentWorld* sw)
	: Actor(ID, x, y, dir, depth, size, sw) {}

bool Inanimate::isDamageable() const {
	return false;
}


// Peach
Peach::Peach(int x, int y, StudentWorld* sw) 
	: Animate(IID_PEACH, x, y, 0, 1, 1.0, sw), curr_HP(1), inv_status(false), remaining_Invincibility(0), remaining_jump_distance(0){}

bool Peach::isInvicible() const {
	return inv_status;
}

void Peach::bonk() {
	return;
}

void Peach::doSomething() {
	if (!isAlive())
		return;

	int keyPress, newX, newY;
	if (inv_status == true) {
		remaining_Invincibility--;
		if (remaining_Invincibility == 0)
			inv_status = false;
	}

	if (power_up == "Shoot" && time_to_recharge_before_next_fire > 0) {
		time_to_recharge_before_next_fire--;
	}

	if (remaining_jump_distance > 0) {
		newY = getY() + 4;
		if (getWorld()->isBlockingObjectAt(getX(), newY)) {
			remaining_jump_distance = 0;
			getWorld()->bonkActor(getX(), newY);
		}
		else {
			moveTo(getX(), newY);
			remaining_jump_distance--;
		}
	}

	if (remaining_jump_distance == 0) {
		if (!(getWorld()->isBlockingObjectAt(getX(), getY() - 3))) {
			moveTo(getX(), getY() - 4);
		}
	}

	getWorld()->getKey(keyPress);
	switch (keyPress) {
	case KEY_PRESS_LEFT:
		setDirection(180);
		newX = getX() - 4;
		if (getWorld()->isBlockingObjectAt(newX, getY())) {
			bonk();
			break;
		}
		moveTo(newX, getY());
		break;
	case KEY_PRESS_RIGHT:
		setDirection(0);
		newX = getX() + 4;
		if (getWorld()->isBlockingObjectAt(newX, getY())) {
			bonk();
			break;
		}
		moveTo(newX, getY());
		break;
	case KEY_PRESS_UP:
		if (getWorld()->isBlockingObjectAt(getX(), getY() - 1)) {
			if (power_up == "jump")
				remaining_jump_distance = 12;
			else
				remaining_jump_distance = 8;
			std::cout << "hello" << std::endl;
			getWorld()->playSound(SOUND_PLAYER_JUMP);
		}
		break;
	case KEY_PRESS_SPACE:
		if (power_up != "Shoot" || time_to_recharge_before_next_fire > 0)
			break;
		getWorld()->playSound(SOUND_PLAYER_FIRE);
		time_to_recharge_before_next_fire = 8;
		if (getDirection() == 0)
			newX = getX() + 4;
		else
			newX = getX() - 4;

		break;
	}
}

// Block 
Block::Block(int x, int y, Actor* goodie, StudentWorld* sw) : Inanimate(IID_BLOCK, x, y, 0, 2, 1.0, sw) {
	if (goodie != nullptr) {
		this->goodie = goodie;
		containsGoodie = true;
	}
	else {
		this->goodie = nullptr;
		containsGoodie = false;
	}
}

void Block::bonk() {
	if (containsGoodie) {
		getWorld()->playSound(SOUND_POWERUP_APPEARS);
		getWorld()->releaseGoodie(goodie);
		goodie = nullptr;
		containsGoodie = false;
	}
	else {
		getWorld()->playSound(SOUND_PLAYER_BONK);
	}
	return;
}

void Block::doSomething() 
{
	return;
}

bool Block::preventsMovement() const {
	return true;
}

// Pipe

