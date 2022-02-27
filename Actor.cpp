#include "Actor.h"
#include "StudentWorld.h"

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

// Actor
Actor::Actor(int ID, int x, int y, int dir, int depth, double size, StudentWorld* sw) 
	: GraphObject(ID, x, y, dir, depth, size), m_alive(true), m_sw(sw) {}

bool Actor::preventsMovement() const {	// Returns whether an actor prevents movement.
	return false;
}

void Actor::setDead() {	// Sets an actor's "alive status" to false.
	m_alive = false;
}

StudentWorld* Actor::getWorld() const {	// Returnss a pointer to the game world.
	return m_sw;
}

bool Actor::isAlive() const{	// Returns whether an actor is alive or not.
	return m_alive;
}

// Peach
Peach::Peach(int x, int y, StudentWorld* sw) 
	: Actor(IID_PEACH, x, y, 0, 1, 1.0, sw), curr_HP(1), remaining_Invincibility(0), remaining_jump_distance(0), inv_power(false), jump_power(false), shoot_power(false), temp_inv(0) {}

// Returns Peach's current HP
bool Peach::getHP() const {
	return curr_HP;
}

// Returns true if Peach has some type of invincibility.
bool Peach::isInvincible() const {
	return inv_power || temp_inv;
}

// Returns if Peach has a particular power which is equivalent to the integer type.
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

// Sets Peach's power based on the type (star = 0, flower = 1, mushroom = 2)
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

// Sets the lives for Peach.
void Peach::setLives(int value) {
	curr_HP = value;
}

void Peach::getDamaged() {
	if (!isInvincible() && (hasPower(1) || hasPower(2))) {	// If Peach has a powerup, remove the powerup(s) and 
		jump_power = false;									// grant peach temporary invincibiliity.
		shoot_power = false;
		temp_inv = true;
		remaining_Invincibility = 10;
		curr_HP = 1;
	}
	else if (!isInvincible()){			// If Peach is both not invincible and does not hold a powerup,
		curr_HP = 0;					// set Peach to dead.
		setDead();
	}
}

// The bonk function for Peach.
void Peach::bonk() {
	if (curr_HP <= 0)
		setDead();
	if (isInvincible())					// If Peach is invincible then we will not do anything to Peach,
		return;							
	getDamaged();						// but otherwise, we will directly damage Peach.
	return;
}

void Peach::doSomething() {
	if (!isAlive())		// If Peach isn't alive, return.
		return;

	int keyPress, newX, newY;
	if (inv_power || temp_inv) {				// If Peach currently has any invincibility, decrement the time
		remaining_Invincibility--;				// remaining for it.
		if (remaining_Invincibility <= 0) {
			inv_power = false;
			temp_inv = false;
			remaining_Invincibility = 0;
		}
	}

	if (shoot_power == true && time_to_recharge_before_next_fire > 0) {		// If Peach's fireball is on a cooldown,
		time_to_recharge_before_next_fire--;								// decrement the time remaining for it.
	}

	if (remaining_jump_distance > 0) {									// If Peach is currently jumping:
		newY = getY() + 4;
		if (getWorld()->isBlockingOrOverlapAt(getX(), newY, 'b')) {		// If there is no more room to move upwards,
			getWorld()->bonkActor(getX(), newY);						// bonk the actor blocking the movement and
			remaining_jump_distance = 0;								// pre-emptively end the jump.
		}
		else {
			moveTo(getX(), newY);		// Otherwise, if Peach can continue moving upwards, update her position
			remaining_jump_distance--;	// and decrement the amount of distance needed to travel.
		}
	} else {
		if (!(getWorld()->isBlockingOrOverlapAt(getX(), getY() - 3, 'b')) && !(getWorld()->isBlockingOrOverlapAt(getX(), getY(), 'b'))				// If there is a blocking object one to three units
			&& !(getWorld()->isBlockingOrOverlapAt(getX(), getY() - 2, 'b')) && !(getWorld()->isBlockingOrOverlapAt(getX(), getY() - 1, 'b'))) {	// underneath Peach, have y-location be set to the ground.
			moveTo(getX(), getY() - 4);
		}
	}

	getWorld()->getKey(keyPress);	// For each different possible key inputs (LEFT, RIGHT, UP, DOWN, SPACE):
	switch (keyPress) {
	case KEY_PRESS_LEFT:	// Change the direction to face left and calculate position four units in that direction.
		setDirection(180);
		newX = getX() - 4;
		if (getWorld()->isBlockingOrOverlapAt(newX, getY(), 'b')) {	// If there are any blocks preventing movement in that location
			getWorld()->bonkActor(newX, getY());					// prevent Peach from going into that space covered by said actor.
			break;
		}
		moveTo(newX, getY());	// Otherwise, allow Peach to move into that space.
		break;
	case KEY_PRESS_RIGHT:
		setDirection(0);	// Change the direction to face left and calculate position four units in that direction.
		newX = getX() + 4;
		if (getWorld()->isBlockingOrOverlapAt(newX, getY(), 'b')) {	// If there are any blocks preventing movement in that location
			getWorld()->bonkActor(newX, getY());					// prevent Peach from going into that space covered by said actor.
			break;
		}
		moveTo(newX, getY());	// Otherwise, allow Peach to move into that space.
		break;
	case KEY_PRESS_UP:
		if (getWorld()->isBlockingOrOverlapAt(getX(), getY() - 1, 'b')) {	// If there is any objects preventing falling directly underneath Peach:
			if (jump_power)													// If Peach has the mushroom power, set the remaining jump distance to 12 (6 blocks distance)
				remaining_jump_distance = 12;								// Else set the remaining jump distance to 8 (4 blocks distance)
			else
				remaining_jump_distance = 8;
			getWorld()->playSound(SOUND_PLAYER_JUMP);						// and play the associated player jumping sound.
		}
		break;
	case KEY_PRESS_SPACE:
		if (!shoot_power || time_to_recharge_before_next_fire > 0)	// If Peach doesn't have the "Shoot" power or has "Shoot" on a cooldown, don't continue any further.
			break;
		getWorld()->playSound(SOUND_PLAYER_FIRE);	// Else, play the sound associated to firing a Peach fireball and set the fireball on a cooldown.
		time_to_recharge_before_next_fire = 8;
		if (getDirection() == 0)		// Based on the direction that Peach is facing, release a fireball in that direction, four units (half a block) in front of Peach.
			newX = getX() + 4;
		else
			newX = getX() - 4;
		getWorld()->releaseProjectile(newX, getY(), getDirection(), 'p');	// Release a fireball in the direction of Peach.
		break;
	}
}

// Block 
Block::Block(int x, int y, char goodie, StudentWorld* sw) : Actor(IID_BLOCK, x, y, 0, 2, 1.0, sw) {
	if (goodie != 0) {			// For each block, if the goodie parameter has an associated value, there is an item inside of the block.
		this->goodie = goodie;	// Set the block to have a goodie.
		containsGoodie = true;	// Change the boolean to match whether the block contains something.
	}
	else {
		this->goodie = 0;		// If the block doesn't contain an associated goodie,
		containsGoodie = false;	// Change the boolean to match whether the block contains anything (nothing).
	}
}

// Dedicated constructor specifically for the Pipe (since it containss exact same functionality without but without goodies).
Block::Block(int x, int y, StudentWorld* sw) : Actor(IID_PIPE, x, y, 0, 2, 1.0, sw), containsGoodie(false), goodie(0) {}

// Specific bonk function for Blocks/Pipes.
void Block::bonk() {
	if (containsGoodie) {									//  If a block contains any goodie, release it and change the block's
		getWorld()->playSound(SOUND_POWERUP_APPEARS);		// status to match having released the goodie from its contents.
		getWorld()->releaseGoodie(getX(), getY(), goodie);	// Play the sound of the powerup releasing.
		goodie = 0;
		containsGoodie = false;
	}
	else {													// Else if there is no goodie contained inside the block/pipe,
		getWorld()->playSound(SOUND_PLAYER_BONK);			// simply play the sound of the player bonking the block.
	}
	return;
}

void Block::doSomething() {	// A block does nothing every tick.
	return;
}

bool Block::preventsMovement() const {	// A block prevents any movement onto its position.
	return true;
}

// Pipe
Pipe::Pipe(int x, int y, StudentWorld* sw) : Block(x, y, sw) {}	// Pipes retain the same functionality as the blocks, minus the goodies.


// Goodie
Goodie::Goodie(int ID, int x, int y, int dir, int depth, double size, int p_type, int p_value, StudentWorld* sw) 
	: Actor (ID, x, y, dir, depth, size, sw), type(p_type), point_value(p_value) {}	// Initializes each actor with a particular point value and type.

void Goodie::doSomething() {										// For each tick of a goodie:
	if (getWorld()->getObjectTypeAt(getX(), getY(), 'g') == 'p') {	// If the goodie is overlapping with Peach:
		getWorld()->increaseScore(point_value);						// increase the overall point value and give Peach the power of it's type.
		getWorld()->setPeachPower(type);							// Increment the lives of Peach and remove the goodie from the game world
		getWorld()->incPeachLives();								// whilst playing the powerup noise.
		setDead();
		getWorld()->playSound(SOUND_PLAYER_POWERUP);
		return;
	}

	if (!(getWorld()->isBlockingOrOverlapAt(getX(), getY() - 2, 'b'))) {	// If there is no blocking object 2 units below the goodie, decrease its current
		moveTo(getX(), getY() - 2);											// y location.
	}

	if (getDirection() == 0 && getWorld()->isBlockingOrOverlapAt(getX() + 2, getY(), 'b')) {	// If facing right and there is a blocking object, turn around.
		setDirection(180);
	}
	else if (getDirection() == 180 && getWorld()->isBlockingOrOverlapAt(getX() - 2, getY(), 'b')) { // If facing left and there is a blocking object, turn around.
		setDirection(0);
	}
	else if (getDirection() == 180)	// Otherwise, depending on the direction, continue moving forwards until it encounters a block or Peach.
		moveTo(getX() - 2, getY());
	else
		moveTo(getX() + 2, getY());

}

Star::Star(int x, int y, StudentWorld* sw) : Goodie(IID_STAR, x, y, 0, 1, 1.0, 0, 100, sw) {}

Flower::Flower(int x, int y, StudentWorld* sw) : Goodie(IID_FLOWER, x, y, 0, 1, 1.0, 1, 50, sw){}

Mushroom::Mushroom(int x, int y, StudentWorld* sw) : Goodie(IID_MUSHROOM, x, y, 0, 1, 1.0, 2, 75, sw) {}

// Projectile
Projectile::Projectile(int ID, int x, int y, int dir, int depth, double size, StudentWorld* sw) 
	: Actor(ID, x, y, dir, depth, size, sw) {}

void Projectile::doSomething() {														// For each tick of existing projectiles:
	if (isFriendly() && getWorld()->getObjectTypeAt(getX(), getY(), 'p') == 'e') {		// If the projectile is friendly (hurts enemies)
		getWorld()->damageObjectAt(getX(), getY());										// and encounters an enemy, damage the object
		setDead();																		// at that location.
		return;
	}
	else if (!isFriendly() && getWorld()->getObjectTypeAt(getX(), getY(), 'e') == 'p'){	// If the projectile is unfriendly (hurts Peach)
		getWorld()->damagePeach();														// and the projectile encounters Peach, attempt
		setDead();																		// to damage Peach.
		return;
	}
	if (!(getWorld()->isBlockingOrOverlapAt(getX(), getY() - 2, 'b'))) {	// If there is no blocking object underneath the projectile,
		moveTo(getX(), getY() - 2);											// it will fall by 2 units for each tick.
	}
	if (getDirection() == 0 && !(getWorld()->isBlockingOrOverlapAt(getX() + 2, getY(), 'b')))			// If there is no blocking object in the direction that
		moveTo(getX() + 2, getY());																		// the projectile is moving in, move the projectile forward
	else if (getDirection() == 180 && !(getWorld()->isBlockingOrOverlapAt(getX() - 2, getY(), 'b')))	// by 2 units.
		moveTo(getX() - 2, getY());
	else																								// Else, if there is a blocking object, remove the projectile from the game.
		setDead();
}

Peach_Fireball::Peach_Fireball(int x, int y, int dir, StudentWorld* sw) : Projectile(IID_PEACH_FIRE, x, y, dir, 1, 1.0, sw) {}

Piranha_Fireball::Piranha_Fireball(int x, int y, int dir, StudentWorld* sw) : Projectile(IID_PIRANHA_FIRE, x, y, dir, 1, 1.0, sw) {}

Shell::Shell(int x, int y, int dir, StudentWorld* sw) : Projectile(IID_SHELL, x, y, dir, 1, 1.0, sw) {}

// Enemy

Enemy::Enemy(int ID, int x, int y, int dir, int depth, double size, StudentWorld* sw) : Actor(ID, x, y, dir, depth, size, sw) {}

bool Enemy::isDamageable() const {	// Enemy is a damageable type of actor.
	return true;
}

void Enemy::getDamaged() {	// For each enemy, if it is damaged, it will be set to dead and increase the player's score.
	setDead();
	getWorld()->increaseScore(100);
}

bool Enemy::overlapWithPeach(){
	if (getWorld()->getObjectTypeAt(getX(), getY(), 'e') == 'p') {	// Checks whether or not an enemy is overlapping with Peach,
		getWorld()->bonkPeach();									// and if it is, bonk Peach, but if Peach holds star power,
		if (getWorld()->hasPeachPower(0)) {							// play the kick sound and damage the enemy in question.
			getWorld()->playSound(SOUND_PLAYER_KICK);
			getDamaged();
		}
		return true;												// Return true if Peach overlapped with an enemy.
	}
	return false;													// Return false otherwise.
}

void Enemy::doSomething() {	// During every tick:
	if (!isAlive())				// If an enemy is dead or has overlapped with Peach, have them make no more moves during the current tick.
		return;
	if (overlapWithPeach())
		return;
	if (getDirection() == 0 && getWorld()->isBlockingOrOverlapAt(getX() + 1, getY(), 'b')) {		// If either direction has been blocked by a block or pipe, preventing movement,
		setDirection(180);																			// set the movement direction to the other way.
	}
	else if (getDirection() == 180 && getWorld()->isBlockingOrOverlapAt(getX() - 1, getY(), 'b')) {
		setDirection(0);
	}

	if (getDirection() == 0 && !getWorld()->isBlockingOrOverlapAt(getX() + SPRITE_WIDTH + 1, getY() - 1, 'b')) {	// If there are no more blocking objects on the ground moving further
		setDirection(180);																							// in the direction, change the direction to the other way.
	}
	else if (getDirection() == 180 && !getWorld()->isBlockingOrOverlapAt(getX() - SPRITE_WIDTH, getY() - 1, 'b')) {
		setDirection(0);
	}

	if (getDirection() == 0 && !getWorld()->isBlockingOrOverlapAt(getX() + 1, getY(), 'b'))			// If there are no objects blocking forward movement in the current direction,
		moveTo(getX() + 1, getY());																	// move 1 unit in that direction each tick.
	else if (getDirection() == 180 && !getWorld()->isBlockingOrOverlapAt(getX() - 1, getY(), 'b'))
		moveTo(getX() - 1, getY());
	return;

}

void Enemy::bonk() {
	if (getWorld()->getObjectTypeAt(getX(), getY(), 'e') == 'p') {	// If an Enemy overlaps with Peach,
		if (getWorld()->hasPeachPower(0)) {							// and if Peach has star power, play the associated "kick" sound
			getWorld()->playSound(SOUND_PLAYER_KICK);				// and damage the enemy.
			getDamaged();
		}
	}
	return;
}
Goomba::Goomba(int x, int y, int dir, StudentWorld* sw) : Enemy(IID_GOOMBA, x, y, dir, 0, 1.0, sw) {}

Koopa::Koopa (int x, int y, int dir, StudentWorld* sw) : Enemy(IID_KOOPA, x, y, dir, 0, 1.0, sw) {}

void Koopa::getDamaged() {
	if (isAlive()) {			// If a Koopa is alive and gets damaged,
		Enemy::getDamaged();	// it will be damaged just like any other enemy.
		getWorld()->releaseProjectile(getX(), getY(), getDirection(), 's');	// The death of each Koopa is followed by the introduction of a new Shell projectile,
	}																		// moving in the direction of the Koopa.
	return;
}

Piranha::Piranha(int x, int y, int dir, StudentWorld* sw) : Enemy(IID_PIRANHA, x, y, dir, 0, 1.0, sw), firing_delay(0) {}

void Piranha::doSomething() {	// For each tick of the game:
	if (!isAlive())				// If the Piranha is dead, do nothing.
		return;
	increaseAnimationNumber();	// Change the animation number to make the Piranha open/close its jaws.
	if (overlapWithPeach()) {	// If the Piranha is overlapping with Peach, have the Piranha perform no other action for the remainder of the tick.
		return;
	}
	if (getWorld()->getPeachY() >= getY() - 1.5 * SPRITE_HEIGHT && getWorld()->getPeachY() < getY() + 1.5 * SPRITE_HEIGHT) {
		if (getWorld()->getPeachX() > getX())							// If Peach is within 1.5 * SPRITE_HEIGHT of the Piranha's y-location,
			setDirection(0);											// have Piranha turn towards Peach's location and if the Piranha is on a
		else															// firing cooldown, decrement the cooldown.
			setDirection(180);
		if (firing_delay > 0) {
			firing_delay--;
			return;
		}
		else {																		// Otherwise, if Peach is within 8 * SPRITE_WIDITH of the Piranha
			int distance = getWorld()->getPeachX() - getX();						// have Piranha introduce a new fireball into the game world.
			if (distance > -8 * SPRITE_WIDTH && distance < 8 * SPRITE_WIDTH) {
				getWorld()->releaseProjectile(getX(), getY(), getDirection(), 'e');
				getWorld()->playSound(SOUND_PIRANHA_FIRE);							// Play the sound associated with Piranha shooting a fireball and set the
				firing_delay = 40;													// firing cooldown to 40 ticks.
			}
		}
	}
	return;
}

// Objective
Objective::Objective(int ID, int x, int y, int dir, int depth, double size, std::string compType, StudentWorld* sw)
	: Actor(ID, x, y, dir, depth, size, sw), completionType(compType){}

void Objective::doSomething() {											// Each objective has its own completion type where flag indicates level completion whereas
	if (!isAlive())														// Mario indicates game completion.
		return;
	else if (getWorld()->getObjectTypeAt(getX(), getY(), 'o') == 'p') {	// If the flag/mario are overlapping with one another, increase the score by 1000,
		getWorld()->increaseScore(1000);								// remove the flag from the game world and set the level status to the type provided
		setDead();														// by the objective!
		getWorld()->setLevelStatus(completionType);
	}
}

Flag::Flag(int x, int y, StudentWorld* sw) : Objective(IID_FLAG, x, y, 0, 1, 1.0, "LVL_COMPLETE", sw) {}

Mario::Mario(int x, int y, StudentWorld* sw) : Objective(IID_MARIO, x, y, 0, 1, 1.0, "GAME_COMPLETE", sw) {}
