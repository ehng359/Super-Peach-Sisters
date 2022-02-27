#include "Actor.h"
#include "StudentWorld.h"

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
Animate::Animate(int ID, int x, int y, int dir, int depth, double size, StudentWorld * sw)
	: Actor(ID, x, y, dir, depth, size, sw) {}

// Inanimate
Inanimate::Inanimate(int ID, int x, int y, int dir, int depth, double size, StudentWorld * sw)
	: Actor(ID, x, y, dir, depth, size, sw) {}

// Peach
Peach::Peach(int x, int y, StudentWorld* sw) 
	: Animate(IID_PEACH, x, y, 0, 1, 1.0, sw), curr_HP(1), remaining_Invincibility(0), remaining_jump_distance(0), inv_power(false), jump_power(false), shoot_power(false), temp_inv(0) {}

bool Peach::getHP() const {
	return curr_HP;
}
bool Peach::isInvincible() const {
	return inv_power || temp_inv;
}

bool Peach::hasPower(int type) const {
	switch (type) {
	case 0:
		return inv_power;
	case 1:
		return shoot_power;
	case 2:
		return jump_power;
	case 3:
		return inv_power || shoot_power || jump_power;
	}
	return false;
}

void Peach::setPower(int type) {
	switch (type) {
	case 0:
		inv_power = true;
		remaining_Invincibility = 150;
		break;
	case 1:
		shoot_power = true;
		break;
	case 2:
		jump_power = true;
		break;
	}
}

void Peach::setLives(int value) {
	curr_HP = value;
}

void Peach::bonk() {
	if (curr_HP <= 0)
		setDead();
	if (isInvincible())
		return;
	if (hasPower(1) || hasPower(2)) {
		jump_power = false;
		shoot_power = false;
		temp_inv = true;
		remaining_Invincibility = 10;
		curr_HP = 1;
	}
	else {
		curr_HP = 0;
		setDead();
	}
	return;
}

void Peach::doSomething() {
	if (!isAlive())
		return;

	int keyPress, newX, newY;
	if (inv_power || temp_inv) {
		remaining_Invincibility--;
		if (remaining_Invincibility <= 0) {
			inv_power = false;
			temp_inv = false;
			remaining_Invincibility = 0;
		}
	}

	if (shoot_power == true && time_to_recharge_before_next_fire > 0) {
		time_to_recharge_before_next_fire--;
	}

	if (remaining_jump_distance > 0) {
		newY = getY() + 4;
		if (getWorld()->isBlockingOrOverlapAt(getX(), newY, 'b')) {
			getWorld()->bonkActor(getX(), newY);
			remaining_jump_distance = 0;
		}
		else {
			moveTo(getX(), newY);
			remaining_jump_distance--;
		}
	} else {
		if (!(getWorld()->isBlockingOrOverlapAt(getX(), getY() - 3, 'b')) && !(getWorld()->isBlockingOrOverlapAt(getX(), getY(), 'b'))
			&& !(getWorld()->isBlockingOrOverlapAt(getX(), getY() - 2, 'b')) && !(getWorld()->isBlockingOrOverlapAt(getX(), getY() - 1, 'b'))) {
			moveTo(getX(), getY() - 4);
		}
	}

	getWorld()->getKey(keyPress);
	switch (keyPress) {
	case KEY_PRESS_LEFT:
		setDirection(180);
		newX = getX() - 4;
		if (getWorld()->isBlockingOrOverlapAt(newX, getY(), 'b')) {
			getWorld()->bonkActor(newX, getY());
			break;
		}
		moveTo(newX, getY());
		break;
	case KEY_PRESS_RIGHT:
		setDirection(0);
		newX = getX() + 4;
		if (getWorld()->isBlockingOrOverlapAt(newX, getY(), 'b')) {
			getWorld()->bonkActor(newX, getY());
			break;
		}
		moveTo(newX, getY());
		break;
	case KEY_PRESS_UP:
		if (getWorld()->isBlockingOrOverlapAt(getX(), getY() - 1, 'b')) {
			if (jump_power)
				remaining_jump_distance = 12;
			else
				remaining_jump_distance = 8;
			getWorld()->playSound(SOUND_PLAYER_JUMP);
		}
		break;
	case KEY_PRESS_SPACE:
		if (!shoot_power || time_to_recharge_before_next_fire > 0)
			break;
		getWorld()->playSound(SOUND_PLAYER_FIRE);
		time_to_recharge_before_next_fire = 8;
		if (getDirection() == 0)
			newX = getX() + 4;
		else
			newX = getX() - 4;
		getWorld()->releaseProjectile(newX, getY(), getDirection(), 'p');
		break;
	}
}

// Block 
Block::Block(int x, int y, char goodie, StudentWorld* sw) : Inanimate(IID_BLOCK, x, y, 0, 2, 1.0, sw) {
	if (goodie != 0) {
		this->goodie = goodie;
		containsGoodie = true;
	}
	else {
		this->goodie = 0;
		containsGoodie = false;
	}
}

Block::Block(int x, int y, StudentWorld* sw) : Inanimate(IID_PIPE, x, y, 0, 2, 1.0, sw), containsGoodie(false), goodie(0) {}

void Block::bonk() {
	if (containsGoodie) {
		getWorld()->playSound(SOUND_POWERUP_APPEARS);
		getWorld()->releaseGoodie(getX(), getY(), goodie);
		goodie = 0;
		containsGoodie = false;
	}
	else {
		getWorld()->playSound(SOUND_PLAYER_BONK);
	}
	return;
}

void Block::doSomething() {
	return;
}

bool Block::preventsMovement() const {
	return true;
}

// Pipe
Pipe::Pipe(int x, int y, StudentWorld* sw) : Block(x, y, sw) {}


// Goodie
Goodie::Goodie(int ID, int x, int y, int dir, int depth, double size, int p_type, int p_value, StudentWorld* sw) 
	: Animate(ID, x, y, dir, depth, size, sw), type(p_type), point_value(p_value) {}

bool Goodie::isDamageable() const{
	return false;
}

void Goodie::doSomething() {
	if (getWorld()->getObjectTypeAt(getX(), getY(), 'g') == 'p') {
		getWorld()->increaseScore(point_value);
		getWorld()->setPeachPower(type);
		getWorld()->incPeachLives();
		setDead();
		getWorld()->playSound(SOUND_PLAYER_POWERUP);
		return;
	}

	if (!(getWorld()->isBlockingOrOverlapAt(getX(), getY() - 2, 'b'))) {
		moveTo(getX(), getY() - 2);
	}

	if (getDirection() == 0 && getWorld()->isBlockingOrOverlapAt(getX() + 2, getY(), 'b')) {
		setDirection(180);
	}
	else if (getDirection() == 180 && getWorld()->isBlockingOrOverlapAt(getX() - 2, getY(), 'b')) {
		setDirection(0);
	}
	else if (getDirection() == 180)
		moveTo(getX() - 2, getY());
	else
		moveTo(getX() + 2, getY());

}

Star::Star(int x, int y, StudentWorld* sw) : Goodie(IID_STAR, x, y, 0, 1, 1.0, 0, 100, sw) {}

Flower::Flower(int x, int y, StudentWorld* sw) : Goodie(IID_FLOWER, x, y, 0, 1, 1.0, 1, 50, sw){}

Mushroom::Mushroom(int x, int y, StudentWorld* sw) : Goodie(IID_MUSHROOM, x, y, 0, 1, 1.0, 2, 75, sw) {}

// Projectile
Projectile::Projectile(int ID, int x, int y, int dir, int depth, double size, StudentWorld* sw) 
	: Animate(ID, x, y, dir, depth, size, sw) {}

void Projectile::doSomething() {
	if (isFriendly() && getWorld()->getObjectTypeAt(getX(), getY(), 'p') == 'e') {
		getWorld()->damageObjectAt(getX(), getY());
		setDead();
		return;
	}
	else if (!isFriendly() && getWorld()->getObjectTypeAt(getX(), getY(), 'e') == 'p'){
		getWorld()->bonkPeach();
		setDead();
		return;
	}
	if (!(getWorld()->isBlockingOrOverlapAt(getX(), getY() - 2, 'b'))) {
		moveTo(getX(), getY() - 2);
	}
	if (getDirection() == 0 && !(getWorld()->isBlockingOrOverlapAt(getX() + 2, getY(), 'b')))
		moveTo(getX() + 2, getY());
	else if (getDirection() == 180 && !(getWorld()->isBlockingOrOverlapAt(getX() - 2, getY(), 'b')))
		moveTo(getX() - 2, getY());
	else
		setDead();
}

Peach_Fireball::Peach_Fireball(int x, int y, int dir, StudentWorld* sw) : Projectile(IID_PEACH_FIRE, x, y, dir, 1, 1.0, sw) {}

Piranha_Fireball::Piranha_Fireball(int x, int y, int dir, StudentWorld* sw) : Projectile(IID_PIRANHA_FIRE, x, y, dir, 1, 1.0, sw) {}

Shell::Shell(int x, int y, int dir, StudentWorld* sw) : Projectile(IID_SHELL, x, y, dir, 1, 1.0, sw) {}

// Enemy

Enemy::Enemy(int ID, int x, int y, int dir, int depth, double size, StudentWorld* sw) : Animate(ID, x, y, dir, depth, size, sw) {}

bool Enemy::isDamageable() const {
	return true;
}

void Enemy::getDamaged() {
	setDead();
	getWorld()->increaseScore(100);
}

bool Enemy::overlapWithPeach(){
	if (getWorld()->getObjectTypeAt(getX(), getY(), 'e') == 'p') {
		getWorld()->bonkPeach();
		if (getWorld()->hasPeachPower(0)) {
			getWorld()->playSound(SOUND_PLAYER_KICK);
			getDamaged();
		}
		return true;
	}
	return false;
}

void Enemy::doSomething() {
	if (!isAlive())
		return;
	if (overlapWithPeach())
		return;
	if (getDirection() == 0 && getWorld()->isBlockingOrOverlapAt(getX() + 1, getY(), 'b')) {
		setDirection(180);
	}
	else if (getDirection() == 180 && getWorld()->isBlockingOrOverlapAt(getX() - 1, getY(), 'b')) {
		setDirection(0);
	}

	if (getDirection() == 0 && !getWorld()->isBlockingOrOverlapAt(getX() + SPRITE_WIDTH + 1, getY() - 1, 'b')) {
		setDirection(180);
	}
	else if (getDirection() == 180 && !getWorld()->isBlockingOrOverlapAt(getX() - SPRITE_WIDTH, getY() - 1, 'b')) {
		setDirection(0);
	}

	if (getDirection() == 0 && !getWorld()->isBlockingOrOverlapAt(getX() + 1, getY(), 'b'))
		moveTo(getX() + 1, getY());
	else if (getDirection() == 180 && !getWorld()->isBlockingOrOverlapAt(getX() - 1, getY(), 'b'))
		moveTo(getX() - 1, getY());
	return;

}

void Enemy::bonk() {
	if (getWorld()->getObjectTypeAt(getX(), getY(), 'e') == 'p') {
		if (getWorld()->hasPeachPower(0)) {
			getWorld()->playSound(SOUND_PLAYER_KICK);
			getDamaged();
		}
	}
	return;
}
Goomba::Goomba(int x, int y, int dir, StudentWorld* sw) : Enemy(IID_GOOMBA, x, y, dir, 0, 1.0, sw) {}

Koopa::Koopa (int x, int y, int dir, StudentWorld* sw) : Enemy(IID_KOOPA, x, y, dir, 0, 1.0, sw) {}

void Koopa::getDamaged() {
	if (isAlive()) {
		Enemy::getDamaged();
		getWorld()->releaseProjectile(getX(), getY(), getDirection(), 's');
	}
	return;
}

Piranha::Piranha(int x, int y, int dir, StudentWorld* sw) : Enemy(IID_PIRANHA, x, y, dir, 0, 1.0, sw), firing_delay(0) {}

void Piranha::doSomething() {
	if (!isAlive())
		return;
	increaseAnimationNumber();
	if (overlapWithPeach()) {
		return;
	}
	if (getWorld()->getPeachY() >= getY() - 1.5 * SPRITE_HEIGHT && getWorld()->getPeachY() < getY() + 1.5 * SPRITE_HEIGHT) {
		if (getWorld()->getPeachX() > getX())
			setDirection(0);
		else
			setDirection(180);
		if (firing_delay > 0) {
			firing_delay--;
			return;
		}
		else {
			int distance = getWorld()->getPeachX() - getX();
			if (distance > -8 * SPRITE_WIDTH && distance < 8 * SPRITE_WIDTH) {
				getWorld()->releaseProjectile(getX(), getY(), getDirection(), 'e');
				getWorld()->playSound(SOUND_PIRANHA_FIRE);
				firing_delay = 40;
			}
		}
	}
	return;
}

// Objective
Objective::Objective(int ID, int x, int y, int dir, int depth, double size, std::string compType, StudentWorld* sw)
	: Inanimate(ID, x, y, dir, depth, size, sw), completionType(compType){}

void Objective::doSomething() {
	if (!isAlive())
		return;
	else if (getWorld()->getObjectTypeAt(getX(), getY(), 'o') == 'p') {
		getWorld()->increaseScore(1000);
		setDead();
		getWorld()->setLevelStatus(completionType);
	}
}

Flag::Flag(int x, int y, StudentWorld* sw) : Objective(IID_FLAG, x, y, 0, 1, 1.0, "LVL_COMPLETE", sw) {}

Mario::Mario(int x, int y, StudentWorld* sw) : Objective(IID_MARIO, x, y, 0, 1, 1.0, "GAME_COMPLETE", sw) {}
