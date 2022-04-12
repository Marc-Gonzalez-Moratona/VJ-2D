#include <cmath>
#include <iostream>
#include <GL/glew.h>
#include <GL/glut.h>
#include "Player.h"
#include "Game.h"


#define JUMP_ANGLE_STEP 4
#define JUMP_HEIGHT 68
#define FALL_STEP 4
#define DASH_ANGLE_STEP 12
#define DASH_HEIGHT 72

enum PlayerAnims
{
	STAND_LEFT, STAND_RIGHT, MOVE_LEFT, MOVE_RIGHT,
	LOOK_DOWN_LEFT, LOOK_DOWN_RIGHT, LOOK_UP_LEFT, LOOK_UP_RIGHT,
	JUMP_LEFT, JUMP_RIGHT, CLIMB_LEFT, CLIMB_RIGHT
};

enum SpringAnims
{
	OPEN, CLOSED
};

enum WallAnims
{
	WALL_EXIST
};

enum StrawberryAnims
{
	STRAWBERRY_EXIST
};

enum WingAnims
{
	WING_UP, WING_MEDIUM, WING_DOWN
};

enum RopeAnims {
	ROPE_LEFT, ROPE_MIDDLE, ROPE_RIGHT
};

enum BaloonAnims {
	BALLOON_EXIST
};

enum PlatformAnims {
	FULL, TOUCHED, BROKEN
};


void Player::init(const glm::ivec2& tileMapPos, ShaderProgram& shaderProgram)
{
	bJumping = false;
	bClimbing = false;
	bClimbJumping = false;
	bDashing = false;
	bGodMode = false;
	bDashMode = false;
	bSlowMode = false;
	bGrabbing = false;
	bChanging = false;
	bWallBroken = false;
	bStrawberryCollected = false;
	strawberryUp = true;
	dashDirection = 0;
	platformFrames = {0,0,0,0,0,0,0,0,0,0,0,0};
	bBalloonsCollected = { false, false };
	setInitialPosition(24, 12 * 24);
	spritesheet.loadFromFile("images/sprite.png", TEXTURE_PIXEL_FORMAT_RGBA);
	sprite = Sprite::createSprite(glm::ivec2(24, 24), glm::vec2(0.25, 0.25), &spritesheet, &shaderProgram);
	sprite->setNumberAnimations(12);

	sprite->setAnimationSpeed(STAND_LEFT, 8);
	sprite->addKeyframe(STAND_LEFT, glm::vec2(0.f, 0.f));

	sprite->setAnimationSpeed(STAND_RIGHT, 8);
	sprite->addKeyframe(STAND_RIGHT, glm::vec2(0.25f, 0.f));

	sprite->setAnimationSpeed(MOVE_LEFT, 8);
	sprite->addKeyframe(MOVE_LEFT, glm::vec2(0.f, 0.f));
	sprite->addKeyframe(MOVE_LEFT, glm::vec2(0.f, 0.25f));
	sprite->addKeyframe(MOVE_LEFT, glm::vec2(0.f, 0.5f));

	sprite->setAnimationSpeed(MOVE_RIGHT, 8);
	sprite->addKeyframe(MOVE_RIGHT, glm::vec2(0.25f, 0.f));
	sprite->addKeyframe(MOVE_RIGHT, glm::vec2(0.25f, 0.25f));
	sprite->addKeyframe(MOVE_RIGHT, glm::vec2(0.25f, 0.5f));

	sprite->setAnimationSpeed(LOOK_DOWN_LEFT, 8);
	sprite->addKeyframe(LOOK_DOWN_LEFT, glm::vec2(0.5f, 0.f));

	sprite->setAnimationSpeed(LOOK_DOWN_RIGHT, 8);
	sprite->addKeyframe(LOOK_DOWN_RIGHT, glm::vec2(0.75f, 0.f));

	sprite->setAnimationSpeed(LOOK_UP_LEFT, 8);
	sprite->addKeyframe(LOOK_UP_LEFT, glm::vec2(0.5f, 0.25f));

	sprite->setAnimationSpeed(LOOK_UP_RIGHT, 8);
	sprite->addKeyframe(LOOK_UP_RIGHT, glm::vec2(0.75f, 0.25f));

	sprite->setAnimationSpeed(JUMP_LEFT, 8);
	sprite->addKeyframe(JUMP_LEFT, glm::vec2(0.f, 0.75f));

	sprite->setAnimationSpeed(JUMP_RIGHT, 8);
	sprite->addKeyframe(JUMP_RIGHT, glm::vec2(0.25f, 0.75f));

	sprite->setAnimationSpeed(CLIMB_LEFT, 8);
	sprite->addKeyframe(CLIMB_LEFT, glm::vec2(0.5f, 0.5f));

	sprite->setAnimationSpeed(CLIMB_RIGHT, 8);
	sprite->addKeyframe(CLIMB_RIGHT, glm::vec2(0.75f, 0.5f));

	sprite->changeAnimation(1);
	tileMapDispl = tileMapPos;
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));

	tilesheet.loadFromFile("images/tilesheet.png", TEXTURE_PIXEL_FORMAT_RGBA);


	wall = Sprite::createSprite(glm::ivec2(48, 48), glm::vec2(0.25f, 0.25f), &tilesheet, &shaderProgram);
	wall->setNumberAnimations(1);
	wall->setAnimationSpeed(WALL_EXIST, 8);
	wall->addKeyframe(WALL_EXIST, glm::vec2(0.5f, 0.f));
	wall->changeAnimation(0);

	balloon[0] = Sprite::createSprite(glm::ivec2(24, 24), glm::vec2(0.125f, 0.125f), &tilesheet, &shaderProgram);
	balloon[0]->setNumberAnimations(1);
	balloon[0]->setAnimationSpeed(BALLOON_EXIST, 8);
	balloon[0]->addKeyframe(BALLOON_EXIST, glm::vec2(0.f, 0.125f));
	balloon[0]->changeAnimation(0);

	balloon[1] = Sprite::createSprite(glm::ivec2(24, 24), glm::vec2(0.125f, 0.125f), &tilesheet, &shaderProgram);
	balloon[1]->setNumberAnimations(1);
	balloon[1]->setAnimationSpeed(BALLOON_EXIST, 8);
	balloon[1]->addKeyframe(BALLOON_EXIST, glm::vec2(0.f, 0.125f));
	balloon[1]->changeAnimation(0);

	rope[0] = Sprite::createSprite(glm::ivec2(24, 24), glm::vec2(0.125f, 0.125f), &tilesheet, &shaderProgram);
	rope[0]->setNumberAnimations(3);
	rope[0]->setAnimationSpeed(ROPE_LEFT, 8);
	rope[0]->addKeyframe(ROPE_LEFT, glm::vec2(0.25f, 0.125f));
	rope[0]->setAnimationSpeed(ROPE_MIDDLE, 8);
	rope[0]->addKeyframe(ROPE_MIDDLE, glm::vec2(0.f, 0.25f));
	rope[0]->setAnimationSpeed(ROPE_RIGHT, 8);
	rope[0]->addKeyframe(ROPE_RIGHT, glm::vec2(0.125f, 0.125f));
	rope[0]->changeAnimation(0);

	rope[1] = Sprite::createSprite(glm::ivec2(24, 24), glm::vec2(0.125f, 0.125f), &tilesheet, &shaderProgram);
	rope[1]->setNumberAnimations(3);
	rope[1]->setAnimationSpeed(ROPE_LEFT, 8);
	rope[1]->addKeyframe(ROPE_LEFT, glm::vec2(0.25f, 0.125f));
	rope[1]->setAnimationSpeed(ROPE_MIDDLE, 8);
	rope[1]->addKeyframe(ROPE_MIDDLE, glm::vec2(0.f, 0.25f));
	rope[1]->setAnimationSpeed(ROPE_RIGHT, 8);
	rope[1]->addKeyframe(ROPE_RIGHT, glm::vec2(0.125f, 0.125f));
	rope[1]->changeAnimation(0);

	wing[0] = Sprite::createSprite(glm::ivec2(24, 24), glm::vec2(0.125f, 0.125f), &tilesheet, &shaderProgram);
	wing[0]->setNumberAnimations(3);
	wing[0]->setAnimationSpeed(WING_UP, 8);
	wing[0]->setAnimationSpeed(WING_MEDIUM , 8);
	wing[0]->setAnimationSpeed(WING_DOWN, 8);
	wing[0]->addKeyframe(WING_UP, glm::vec2(0.25f, 0.875f));
	wing[0]->addKeyframe(WING_MEDIUM, glm::vec2(0.5f, 0.875f));
	wing[0]->addKeyframe(WING_DOWN, glm::vec2(0.75f, 0.875f));
	wing[0]->changeAnimation(0);
	
	wing[1] = Sprite::createSprite(glm::ivec2(24, 24), glm::vec2(0.125f, 0.125f), &tilesheet, &shaderProgram);
	wing[1]->setNumberAnimations(3);
	wing[1]->setAnimationSpeed(WING_UP, 8);
	wing[1]->setAnimationSpeed(WING_MEDIUM, 8);
	wing[1]->setAnimationSpeed(WING_DOWN, 8);
	wing[1]->addKeyframe(WING_UP, glm::vec2(0.125f, 0.875f));
	wing[1]->addKeyframe(WING_MEDIUM, glm::vec2(0.375f, 0.875f));
	wing[1]->addKeyframe(WING_DOWN, glm::vec2(0.625f, 0.875f));
	wing[1]->changeAnimation(0);

	strawberry = Sprite::createSprite(glm::ivec2(24, 24), glm::vec2(0.125f, 0.125f), &tilesheet, &shaderProgram);
	strawberry->setNumberAnimations(1);
	strawberry->setAnimationSpeed(STRAWBERRY_EXIST, 8);
	strawberry->addKeyframe(STRAWBERRY_EXIST, glm::vec2(0.f, 0.875f));
	strawberry->changeAnimation(0);

	for (int i = 0; i < 12; ++i) {
		platforms[i] = Sprite::createSprite(glm::ivec2(24, 24), glm::vec2(0.125f, 0.125f), &tilesheet, &shaderProgram);
		platforms[i]->setNumberAnimations(3);
		platforms[i]->setAnimationSpeed(FULL, 8);
		platforms[i]->addKeyframe(FULL, glm::vec2(0.f, 0.375f));
		platforms[i]->setAnimationSpeed(TOUCHED, 8);
		platforms[i]->addKeyframe(TOUCHED, glm::vec2(0.125f, 0.375f));
		platforms[i]->setAnimationSpeed(BROKEN, 8);
		platforms[i]->addKeyframe(BROKEN, glm::vec2(0.25f, 0.375f));
		platforms[i]->changeAnimation(0);
	}

	
	springLeft = Sprite::createSprite(glm::ivec2(24, 24), glm::vec2(0.125f, 0.125f), &tilesheet, &shaderProgram);
	springRight = Sprite::createSprite(glm::ivec2(24, 24), glm::vec2(0.125f, 0.125f), &tilesheet, &shaderProgram);

	springLeft->setNumberAnimations(2);
	springRight->setNumberAnimations(2);

	springLeft->setAnimationSpeed(OPEN, 8);
	springRight->setAnimationSpeed(OPEN, 8);
	
	springLeft->addKeyframe(OPEN, glm::vec2(0.875f, 0.75f));
	springRight->addKeyframe(OPEN, glm::vec2(0.875f, 0.75f));

	springLeft->setAnimationSpeed(CLOSED, 8);
	springRight->setAnimationSpeed(CLOSED, 8);
	
	springLeft->addKeyframe(CLOSED, glm::vec2(0.875f, 0.875f));
	springRight->addKeyframe(CLOSED, glm::vec2(0.875f, 0.875f));

	springLeft->changeAnimation(0);
	springRight->changeAnimation(0);
}

bool Player::spriteCollision(Sprite *s1, Sprite *s2, bool isWall) {
	int spriteSize = 24;
	if (isWall) spriteSize *= 2;
	bool noHorizontalCollide = s1->getPosition().x + spriteSize <= s2->getPosition().x || s1->getPosition().x >= s2->getPosition().x + spriteSize;
	bool noVerticalCollide = s1->getPosition().y + spriteSize <= s2->getPosition().y || s1->getPosition().y >= s2->getPosition().y + spriteSize;
	if (noHorizontalCollide || noVerticalCollide) return false;
	return true;
}

bool Player::platformCollision(Sprite* s1, Sprite* s2) {
	int spriteSize = 28;
	bool noHorizontalCollide = s1->getPosition().x + spriteSize <= s2->getPosition().x || s1->getPosition().x >= s2->getPosition().x + spriteSize;
	bool noVerticalCollide = s1->getPosition().y + spriteSize <= s2->getPosition().y;
	if (noHorizontalCollide || noVerticalCollide) return false;
	return true;
}

void Player::resetLevel() {
	bWallBroken = false;
	bStrawberryCollected = false;
}

void Player::setStrawberryDispl(glm::dvec2 displ) {
	strawberryDispl = displ;
}

void Player::setBalloonDispl(glm::dvec2 displ) {
	balloonDispl = displ;
}

void Player::update(int deltaTime, int level)
{
	tileMapDispl.y = 25 - 432 * (level - 1);
	sprite->update(deltaTime);
	springLeft->update(deltaTime);
	springRight->update(deltaTime);
	wall->update(deltaTime);
	strawberry->update(deltaTime);

	if (!Game::instance().getKey('g') && !Game::instance().getKey('d') && !Game::instance().getKey('s')) bChanging = false;
	if (!bChanging) {
		if (Game::instance().getKey('g')) {
			bGodMode = !bGodMode;
			if (bGodMode) cout << "God mode enabled" << endl;
			else cout << "God mode disabled" << endl;
			bChanging = true;
		}
		if (Game::instance().getKey('d')) {
			bDashMode = !bDashMode;
			if (bDashMode) cout << "Dash mode enabled" << endl;
			else cout << "Dash mode disabled" << endl;
			bChanging = true;
		}
		if (Game::instance().getKey('s')) {
			bSlowMode = !bSlowMode;
			if (bSlowMode) cout << "Slow mode enabled" << endl;
			else cout << "Slow mode disabled" << endl;		
			bChanging = true;
		}
	}

	// Clear console using back key
	if (Game::instance().getKey(8)) system("CLS");

	// JUMP
	if (Game::instance().getKey('c') && jumpAngle == 0) {
		if (map->collisionMoveDown(posPlayer + FALL_STEP, glm::ivec2(24, 24), &posPlayer.x, &posPlayer.y, initialX, initialY, bGodMode)) {
			cout << "Jump detected" << endl;
			if (sprite->animation() == STAND_RIGHT || sprite->animation() == LOOK_DOWN_RIGHT || sprite->animation() == LOOK_UP_RIGHT) sprite->changeAnimation(JUMP_RIGHT);
			else if (sprite->animation() == STAND_LEFT || sprite->animation() == LOOK_DOWN_LEFT || sprite->animation() == LOOK_UP_LEFT) sprite->changeAnimation(JUMP_LEFT);
			bJumping = true;
		}
	}

	if (bClimbJumping) {
		bGrabbing = false;
		if (sprite->animation() == JUMP_LEFT) {
			if (bSlowMode) posPlayer.x -= 2;
			else posPlayer.x -= 3;
			if (map->collisionMoveLeft(posPlayer, glm::ivec2(24, 24), &posPlayer.x, 3)) {
				if (bSlowMode) posPlayer.x += 2;
				else posPlayer.x += 3;
				bClimbing = true;
			}
		}
		else if (sprite->animation() == JUMP_RIGHT) {
			if (bSlowMode) posPlayer.x += 2;
			else posPlayer.x += 3;
			if (map->collisionMoveRight(posPlayer, glm::ivec2(24, 24), &posPlayer.x, 3)) {
				if (bSlowMode) posPlayer.x -= 2;
				else posPlayer.x -= 3;
				bClimbing = true;
			}
		}
		if (bSlowMode) climbJumpAngle += JUMP_ANGLE_STEP/2;
		else climbJumpAngle += JUMP_ANGLE_STEP;
		// FIRST HALF OF A CLIMBJUMP
		if (climbJumpAngle <= 75) {
			posPlayer.y = int(startY - JUMP_HEIGHT * sin(3.14159f * climbJumpAngle / 180.f));
			if (map->collisionMoveUp(posPlayer, glm::ivec2(24, 24), &posPlayer.y)) bClimbJumping = false;
		}
		// START TO FALL AFTER THE FIRST HALF
		else bClimbJumping = false;
	}
	else {
		// RESET THE STATE WHEN THE PLAYER TOUCHES THE GROUND
		if (map->collisionMoveDown(posPlayer, glm::ivec2(24, 24), &posPlayer.x, &posPlayer.y, initialX, initialY, bGodMode))
		{
			bClimbJumping = false;
			if (!Game::instance().getKey('c')) climbJumpAngle = 0;
			startY = posPlayer.y;
			dashY = posPlayer.y;
			if (sprite->animation() == JUMP_RIGHT || sprite->animation() == CLIMB_RIGHT) sprite->changeAnimation(STAND_RIGHT);
			else if (sprite->animation() == JUMP_LEFT || sprite->animation() == CLIMB_LEFT) sprite->changeAnimation(STAND_LEFT);
		}
		// SECOND HALF OF A JUMP
		if (climbJumpAngle > 75 && climbJumpAngle < 180) climbJumpAngle += JUMP_ANGLE_STEP;
		/* A CLIMBJUMP CAN HAVE MORE THAN 180 DEGREES
		WHEN THE PLAYER JUMPS TO A PLATFORM THAT IS
		AT A LOWER HEIGHT */
		else if (climbJumpAngle >= 180) {
			if (map->collisionMoveDown(posPlayer, glm::ivec2(24, 24), &posPlayer.x, &posPlayer.y, initialX, initialY, bGodMode)) {
				climbJumpAngle = 0;
				posPlayer.y = startY;
			}
		}
	}

	if (spriteCollision(sprite, wall, true)) {
		if (bDashing) {
			bWallBroken = true;
			if (level == 1 && dashDirection == 4) {
				posPlayer.x += 20;
				bDashing = false;
			}
			else if (level == 9 && (dashDirection == 4 || dashDirection == 5 || dashDirection == 6)) {
				posPlayer.x += 20;
				posPlayer.y += 20;
				bDashing = false;
			}
		}
		if (!bWallBroken) {
			if (level == 1) posPlayer.x += 3;
			else if (level == 9) posPlayer.y += 3;
		}
	}

	for (int i = 0; i < 12; ++i) {
		if (platformCollision(sprite, platforms[i])) {
			if (sprite->getPosition().y + 28 > platforms[i]->getPosition().y) {
				if (Game::instance().getKey('c') && !bJumping) {
					jumpAngle = 0;
					startY = posPlayer.y;
					if (sprite->animation() == STAND_RIGHT || sprite->animation() == LOOK_DOWN_RIGHT || sprite->animation() == LOOK_UP_RIGHT) sprite->changeAnimation(JUMP_RIGHT);
					else if (sprite->animation() == STAND_LEFT || sprite->animation() == LOOK_DOWN_LEFT || sprite->animation() == LOOK_UP_LEFT) sprite->changeAnimation(JUMP_LEFT);
					bJumping = true;
				}
				if (Game::instance().getKey('x') && !bDashing) {
					dashY = posPlayer.y;
					if (sprite->animation() == STAND_LEFT || sprite->animation() == CLIMB_RIGHT || sprite->animation() == LOOK_UP_LEFT) sprite->changeAnimation(JUMP_LEFT);
					if (sprite->animation() == STAND_RIGHT || sprite->animation() == CLIMB_LEFT || sprite->animation() == LOOK_UP_RIGHT) sprite->changeAnimation(JUMP_RIGHT);
					if (Game::instance().getSpecialKey(GLUT_KEY_RIGHT)) {
						if (Game::instance().getSpecialKey(GLUT_KEY_DOWN)) dashDirection = 1;
						else if (Game::instance().getSpecialKey(GLUT_KEY_UP)) dashDirection = 7;
						else dashDirection = 0;
					}
					else if (Game::instance().getSpecialKey(GLUT_KEY_LEFT)) {
						if (Game::instance().getSpecialKey(GLUT_KEY_DOWN)) dashDirection = 3;
						else if (Game::instance().getSpecialKey(GLUT_KEY_UP)) dashDirection = 5;
						else dashDirection = 4;
					}
					else if (Game::instance().getSpecialKey(GLUT_KEY_DOWN)) dashDirection = 2;
					else if (Game::instance().getSpecialKey(GLUT_KEY_UP)) dashDirection = 6;
					else if (sprite->animation() == STAND_LEFT || sprite->animation() == JUMP_LEFT || sprite->animation() == CLIMB_RIGHT) dashDirection = 4;
					else if (sprite->animation() == STAND_RIGHT || sprite->animation() == JUMP_RIGHT || sprite->animation() == CLIMB_LEFT) dashDirection = 0;
					dashAngle = 0;
					bDashing = true;
				}
				platforms[i]->changeAnimation(TOUCHED);
				if(platformFrames[i] < 30 && !bJumping) posPlayer.y -= FALL_STEP;
				if (platformFrames[i] == 20) platforms[i]->changeAnimation(BROKEN);
				i = 12;
			}
		}
	}
	
	for (int i = 0; i < 12; ++i) {
		if (platforms[i]->animation() != FULL) platformFrames[i]++;
		if (platformFrames[i] >= 50) {
			platformFrames[i] = 0;
			platforms[i]->changeAnimation(FULL);
		}
	}

	if (spriteCollision(sprite, strawberry, false)) bStrawberryCollected = true; 
	if (spriteCollision(sprite, springLeft, false)) {
		if (level == 3 || level == 9) {
			bJumping = true;
			jumpAngle = 0;
			dashAngle = 0;
			springLeft->changeAnimation(CLOSED);
		}
	}
	else if(spriteCollision(sprite, springRight, false)) {
		if (level == 3 || level == 9) {
			bJumping = true;
			jumpAngle = 0;
			dashAngle = 0;
			springRight->changeAnimation(CLOSED);
		}
	}

	if (bJumping) {
		if (bSlowMode) jumpAngle += JUMP_ANGLE_STEP/2;
		else jumpAngle += JUMP_ANGLE_STEP;
		// FIRST HALF OF A JUMP
		if (jumpAngle <= 90) {
			posPlayer.y = int(startY - JUMP_HEIGHT * sin(3.14159f * jumpAngle / 180.f));
			if (map->collisionMoveUp(posPlayer, glm::ivec2(24, 24), &posPlayer.y)) bJumping = false;
		}
		// START TO FALL AFTER THE FIRST HALF
		else bJumping = false;
	}
	else {
		// FALLING
		if (bGrabbing) {
			if (bSlowMode) posPlayer.y += FALL_STEP / 4;
			else posPlayer.y += (FALL_STEP / 2);
		}
		else if (!bDashing || (bDashing && dashAngle >= 60)) {
			if (bSlowMode) posPlayer.y += FALL_STEP / 2;
			else posPlayer.y += FALL_STEP;
		}
		if (sprite->animation() == STAND_RIGHT) sprite->changeAnimation(JUMP_RIGHT);
		else if (sprite->animation() == STAND_LEFT) sprite->changeAnimation(JUMP_LEFT);
		// RESET THE STATE WHEN THE PLAYER TOUCHES THE GROUND
		if (map->collisionMoveDown(posPlayer, glm::ivec2(24, 24), &posPlayer.x, &posPlayer.y, initialX, initialY, bGodMode))
		{
			bClimbing = false;
			bJumping = false;
			bClimbJumping = false;
			bGrabbing = false;
			if (!Game::instance().getKey('c')) {
				jumpAngle = 0;
				climbJumpAngle = 0;
			}
			startY = posPlayer.y;
			dashY = posPlayer.y;
			if (sprite->animation() == JUMP_RIGHT || sprite->animation() == CLIMB_RIGHT) sprite->changeAnimation(STAND_RIGHT);
			else if (sprite->animation() == JUMP_LEFT || sprite->animation() == CLIMB_LEFT) sprite->changeAnimation(STAND_LEFT);
		}
		// SECOND HALF OF A JUMP
		if (jumpAngle > 90 && jumpAngle < 180) {
			springLeft->changeAnimation(OPEN);
			springRight->changeAnimation(OPEN);
			if (bSlowMode) jumpAngle += JUMP_ANGLE_STEP / 2;
			else jumpAngle += JUMP_ANGLE_STEP;
		}
		/* A JUMP CAN HAVE MORE THAN 180 DEGREES
		WHEN THE PLAYER JUMPS TO A PLATFORM THAT IS
		AT A LOWER HEIGHT */
		else if (jumpAngle >= 180) {
			if (map->collisionMoveDown(posPlayer, glm::ivec2(24, 24), &posPlayer.x, &posPlayer.y, initialX, initialY, bGodMode)) {
				jumpAngle = 0;
				posPlayer.y = startY;
			}
		}
	}

	// CLIMBJUMPING
	if (Game::instance().getKey('c') && bClimbing && !bJumping && climbJumpAngle == 0) {
		cout << "ClimbJump detected" << endl;
		bClimbJumping = true;
		bJumping = false;
		bClimbing = false;
		bGrabbing = false;
		startY = posPlayer.y;
		if (sprite->animation() == STAND_LEFT || sprite->animation() == JUMP_LEFT || sprite->animation() == CLIMB_RIGHT) {
			if (map->collisionMoveLeft(posPlayer, glm::ivec2(24, 24), &posPlayer.x, 3)) posPlayer.x += 3;
			sprite->changeAnimation(JUMP_RIGHT);
		}
		else if (sprite->animation() == STAND_RIGHT || sprite->animation() == JUMP_RIGHT || sprite->animation() == CLIMB_LEFT) {
			if (map->collisionMoveRight(posPlayer, glm::ivec2(24, 24), &posPlayer.x, 3)) posPlayer.x -= 3;
			sprite->changeAnimation(JUMP_LEFT);
		}
	}

	if (bClimbing && !bClimbJumping && !Game::instance().getKey('c')) climbJumpAngle = 0;

	if (bClimbing) cout << "Climbing" << endl;
	if (bJumping) cout << "Jumping" << endl;
	if (bClimbJumping) cout << "ClimbJumping" << endl;
	if (bDashing) cout << "Dashing" << endl;
	if (bGrabbing) cout << "Grabbing" << endl;

	if (Game::instance().getKey('x')) cout << dashAngle << endl;

	// DASH
	if (Game::instance().getKey('x') && (!bDashing || bDashMode) && dashAngle == 0) {
		cout << "Dash detected" << endl;
		bDashing = true;
		dashY = posPlayer.y;
		if (sprite->animation() == STAND_LEFT || sprite->animation() == CLIMB_RIGHT || sprite->animation() == LOOK_UP_LEFT) sprite->changeAnimation(JUMP_LEFT);
		if (sprite->animation() == STAND_RIGHT || sprite->animation() == CLIMB_LEFT || sprite->animation() == LOOK_UP_RIGHT) sprite->changeAnimation(JUMP_RIGHT);
		if (Game::instance().getSpecialKey(GLUT_KEY_RIGHT)) {
			if (Game::instance().getSpecialKey(GLUT_KEY_DOWN)) dashDirection = 1;
			else if (Game::instance().getSpecialKey(GLUT_KEY_UP)) dashDirection = 7;
			else dashDirection = 0;
		}
		else if (Game::instance().getSpecialKey(GLUT_KEY_LEFT)) {
			if (Game::instance().getSpecialKey(GLUT_KEY_DOWN)) dashDirection = 3;
			else if (Game::instance().getSpecialKey(GLUT_KEY_UP)) dashDirection = 5;
			else dashDirection = 4;
		}
		else if (Game::instance().getSpecialKey(GLUT_KEY_DOWN)) dashDirection = 2;
		else if (Game::instance().getSpecialKey(GLUT_KEY_UP)) dashDirection = 6;
		else if (sprite->animation() == STAND_LEFT || sprite->animation() == JUMP_LEFT || sprite->animation() == CLIMB_RIGHT) dashDirection = 4;
		else if (sprite->animation() == STAND_RIGHT || sprite->animation() == JUMP_RIGHT || sprite->animation() == CLIMB_LEFT) dashDirection = 0;
	}

	if (bDashing) {
		if ((!bDashMode && dashAngle < 90) || (bDashMode && dashAngle < 60)) {
			bGrabbing = false;
			if (bSlowMode) dashAngle += DASH_ANGLE_STEP / 4;
			else dashAngle += DASH_ANGLE_STEP / 2;
			if (map->collisionMoveDown(posPlayer, glm::ivec2(24, 24), &posPlayer.x, &posPlayer.y, initialX, initialY, bGodMode)) bDashing = false;
			if (map->collisionMoveUp(posPlayer, glm::ivec2(24, 24), &posPlayer.y)) bDashing = false;
			switch (dashDirection) {
			case 0:
				if (bSlowMode) posPlayer.x += 2;
				else posPlayer.x += 4;
				if (map->collisionMoveRight(posPlayer, glm::ivec2(24, 24), &posPlayer.x, 4)) {
					if (bSlowMode) posPlayer.x -= 2;
					else posPlayer.x -= 4;
					bDashing = false;
				}
				break;
			case 1:
				if (bSlowMode) posPlayer.x += 2;
				else posPlayer.x += 4;
				if (map->collisionMoveRight(posPlayer + FALL_STEP, glm::ivec2(24, 24), &posPlayer.x, 4)) {
					if (bSlowMode) posPlayer.x -= 2;
					else posPlayer.x -= 4;
					bDashing = false;
				}
				if (!map->collisionMoveDown(posPlayer, glm::ivec2(24, 24), &posPlayer.x, &posPlayer.y, initialX, initialY, bGodMode)) posPlayer.y = int(dashY + DASH_HEIGHT * sin(3.14159f * dashAngle / 180.f));
				break;
			case 2:
				if (!map->collisionMoveDown(posPlayer + FALL_STEP, glm::ivec2(24, 24), &posPlayer.x, &posPlayer.y, initialX, initialY, bGodMode))posPlayer.y = int(dashY + DASH_HEIGHT * sin(3.14159f * dashAngle / 180.f));
				break;
			case 3:
				if (bSlowMode) posPlayer.x -= 2;
				else posPlayer.x -= 4;
				if (map->collisionMoveLeft(posPlayer, glm::ivec2(24, 24), &posPlayer.x, 4)) {
					if (bSlowMode) posPlayer.x += 2;
					else posPlayer.x += 4;
					bDashing = false;
				}
				if (!map->collisionMoveDown(posPlayer + FALL_STEP, glm::ivec2(24, 24), &posPlayer.x, &posPlayer.y, initialX, initialY, bGodMode)) posPlayer.y = int(dashY + DASH_HEIGHT * sin(3.14159f * dashAngle / 180.f));
				break;
			case 4:
				if (bSlowMode) posPlayer.x -= 2;
				else posPlayer.x -= 4;
				if (map->collisionMoveLeft(posPlayer, glm::ivec2(24, 24), &posPlayer.x, 4)) {
					if (bSlowMode) posPlayer.x += 2;
					else posPlayer.x += 4;
					bDashing = false;
				}
				break;
			case 5:
				if (bSlowMode) posPlayer.x -= 2;
				else posPlayer.x -= 4;
				if (map->collisionMoveLeft(posPlayer, glm::ivec2(24, 24), &posPlayer.x, 4)) {
					if (bSlowMode) posPlayer.x += 2;
					else posPlayer.x += 4;
					bDashing = false;
				}
				posPlayer.y = int(dashY - DASH_HEIGHT * sin(3.14159f * dashAngle / 180.f));
				break;
			case 6:
				posPlayer.y = int(dashY - DASH_HEIGHT * sin(3.14159f * dashAngle / 180.f));
				break;
			case 7:
				if (bSlowMode) posPlayer.x += 2;
				else posPlayer.x += 4;
				if (map->collisionMoveRight(posPlayer, glm::ivec2(24, 24), &posPlayer.x, 4)) {
					if (bSlowMode) posPlayer.x -= 2;
					else posPlayer.x -= 4;
					bDashing = false;
				}
				posPlayer.y = int(dashY - DASH_HEIGHT * sin(3.14159f * dashAngle / 180.f));
				break;
			default:
				break;
			}
		}
		else bDashing = false;
	}
	else if (bDashMode) dashAngle = 0;
	else if (!bJumping && !bClimbing && !bGrabbing && map->collisionMoveDown(posPlayer + FALL_STEP, glm::ivec2(24, 24), &posPlayer.x, &posPlayer.y, initialX, initialY, bGodMode) && !Game::instance().getKey('x')) dashAngle = 0;

	if (bDashMode && bGrabbing) {
		if (map->collisionMoveUp(posPlayer, glm::ivec2(24, 24), &posPlayer.y)) {
			bGrabbing = false;
		}
	}

	// WALK
	// MOVE LEFT
	if (Game::instance().getSpecialKey(GLUT_KEY_LEFT) && !bClimbJumping && (!bDashing || (bDashing && dashDirection != 4 && dashDirection != 0)))
	{
		if (bSlowMode) posPlayer.x -= 2;
		else posPlayer.x -= 3;
		if (map->collisionMoveLeft(posPlayer, glm::ivec2(24, 24), &posPlayer.x, 3)) {
			if (bSlowMode) posPlayer.x += 2;
			else posPlayer.x += 3;
			if (bClimbing) {
				bGrabbing = true;
				sprite->changeAnimation(CLIMB_RIGHT);
			}
			else if (!map->collisionMoveDown(posPlayer, glm::ivec2(24, 24), &posPlayer.x, &posPlayer.y, initialX, initialY, bGodMode) && !Game::instance().getKey('c')) {
				bClimbing = true;
			}
		}
		else if (sprite->animation() != MOVE_LEFT) {
			sprite->changeAnimation(MOVE_LEFT);
			bGrabbing = false;
		}
	}
	// MOVE RIGHT
	else if (Game::instance().getSpecialKey(GLUT_KEY_RIGHT) && !bClimbJumping && (!bDashing || (bDashing && dashDirection != 4 && dashDirection != 0)))
	{
		if (bSlowMode) posPlayer.x += 2;
		else posPlayer.x += 3;
		if (map->collisionMoveRight(posPlayer, glm::ivec2(24, 24), &posPlayer.x, 3)) {
			if (bSlowMode) posPlayer.x -= 2;
			else posPlayer.x -= 3;
			if (bClimbing) {
				bGrabbing = true;
				sprite->changeAnimation(CLIMB_LEFT);
			}
			else if (!map->collisionMoveDown(posPlayer, glm::ivec2(24, 24), &posPlayer.x, &posPlayer.y, initialX, initialY, bGodMode) && !Game::instance().getKey('c')) {
				bClimbing = true;
			}
		}
		else if (sprite->animation() != MOVE_RIGHT) {
			sprite->changeAnimation(MOVE_RIGHT);
			bGrabbing = false;
		}
	}
	// LOOK DOWN
	else if (Game::instance().getSpecialKey(GLUT_KEY_DOWN))
	{
		if (sprite->animation() == STAND_LEFT || sprite->animation() == MOVE_LEFT)
			sprite->changeAnimation(LOOK_DOWN_LEFT);
		else if (sprite->animation() == STAND_RIGHT || sprite->animation() == MOVE_RIGHT)
			sprite->changeAnimation(LOOK_DOWN_RIGHT);
	}
	// LOOK UP
	else if (Game::instance().getSpecialKey(GLUT_KEY_UP))
	{
		if (sprite->animation() == STAND_LEFT || sprite->animation() == MOVE_LEFT)
			sprite->changeAnimation(LOOK_UP_LEFT);
		else if (sprite->animation() == STAND_RIGHT || sprite->animation() == MOVE_RIGHT)
			sprite->changeAnimation(LOOK_UP_RIGHT);
	}
	// NOT MOVING
	else
	{
		bGrabbing = false;
		if (!map->collisionMoveDown(posPlayer, glm::ivec2(24, 24), &posPlayer.x, &posPlayer.y, initialX, initialY, bGodMode)) {
			posPlayer.x += 3;
			if (map->collisionMoveRight(posPlayer, glm::ivec2(24, 24), &posPlayer.x, 6) && !Game::instance().getKey('c')) {
				bClimbing = true;
			}
			posPlayer.x -= 6;
			if (map->collisionMoveLeft(posPlayer, glm::ivec2(24, 24), &posPlayer.x, 3) && !Game::instance().getKey('c')) {
				bClimbing = true;
			}
			posPlayer.x += 3;
		}
		if (sprite->animation() == MOVE_LEFT || sprite->animation() == CLIMB_RIGHT)
			sprite->changeAnimation(STAND_LEFT);
		else if (sprite->animation() == MOVE_RIGHT || sprite->animation() == CLIMB_LEFT)
			sprite->changeAnimation(STAND_RIGHT);
		else if (sprite->animation() == LOOK_DOWN_LEFT)
			sprite->changeAnimation(STAND_LEFT);
		else if (sprite->animation() == LOOK_DOWN_RIGHT)
			sprite->changeAnimation(STAND_RIGHT);
		else if (sprite->animation() == LOOK_UP_LEFT)
			sprite->changeAnimation(STAND_LEFT);
		else if (sprite->animation() == LOOK_UP_RIGHT)
			sprite->changeAnimation(STAND_RIGHT);
	}

	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));

	switch (level) {
		case 1:
			if (strawberryUp) {
				if (strawberryDispl.y > 138) strawberryUp = false;
				else strawberryDispl.y += 0.35;
			}
			else {
				if (strawberryDispl.y < 126) strawberryUp = true; 
				else strawberryDispl.y -= 0.35;
			}
			strawberry->setPosition(glm::vec2(float(tileMapDispl.x + strawberryDispl.x), float(tileMapDispl.y + strawberryDispl.y)));
			wall->setPosition(glm::vec2(float(tileMapDispl.x + 47), float(tileMapDispl.y + 120)));
			break;
		case 2:
			break;
		case 3:
			if (strawberryUp) {
				if (strawberryDispl.y > 138) strawberryUp = false;
				else strawberryDispl.y += 0.35;
			}
			else {
				if (strawberryDispl.y < 126) strawberryUp = true;
				else strawberryDispl.y -= 0.35;
			}
			strawberry->setPosition(glm::vec2(float(tileMapDispl.x + strawberryDispl.x), float(tileMapDispl.y + strawberryDispl.y + 938)));
			springLeft->setPosition(glm::vec2(float(tileMapDispl.x + 144), float(tileMapDispl.y + 1224)));
			springRight->setPosition(glm::vec2(float(tileMapDispl.x + 336), float(tileMapDispl.y + 1224)));
			break;
		case 4:
			if (strawberryUp) {
				if (strawberryDispl.y > 138) {
					strawberryUp = false;
					wing[0]->changeAnimation(WING_DOWN);
					wing[1]->changeAnimation(WING_DOWN);
				}
				else {
					strawberryDispl.y += 0.35;
					if (strawberryDispl.y > 133) {
						if (strawberryDispl.y > 133) {
							wing[0]->changeAnimation(WING_UP);
							wing[1]->changeAnimation(WING_UP);
						}
					}
				}
			}
			else {
				if (strawberryDispl.y < 126) {
					strawberryUp = true;
					wing[0]->changeAnimation(WING_MEDIUM);
					wing[1]->changeAnimation(WING_MEDIUM);
				}
				else strawberryDispl.y -= 0.35;
			}
			strawberry->setPosition(glm::vec2(float(tileMapDispl.x + strawberryDispl.x + 72), float(tileMapDispl.y + strawberryDispl.y + 1300)));
			wing[0]->setPosition(glm::vec2(float(tileMapDispl.x + strawberryDispl.x + 94), float(tileMapDispl.y + strawberryDispl.y + 1290)));
			wing[1]->setPosition(glm::vec2(float(tileMapDispl.x + strawberryDispl.x + 50), float(tileMapDispl.y + strawberryDispl.y + 1290)));
			platforms[0]->setPosition(glm::vec2(float(tileMapDispl.x + 120), float(tileMapDispl.y + 1680)));
			platforms[1]->setPosition(glm::vec2(float(tileMapDispl.x + 144), float(tileMapDispl.y + 1680)));
			platforms[2]->setPosition(glm::vec2(float(tileMapDispl.x + 220), float(tileMapDispl.y + 1656)));
			platforms[3]->setPosition(glm::vec2(float(tileMapDispl.x + 244), float(tileMapDispl.y + 1656)));
			platforms[4]->setPosition(glm::vec2(float(tileMapDispl.x + 308), float(tileMapDispl.y + 1632)));
			platforms[5]->setPosition(glm::vec2(float(tileMapDispl.x + 332), float(tileMapDispl.y + 1632)));
			platforms[6]->setPosition(glm::vec2(float(tileMapDispl.x + 356), float(tileMapDispl.y + 1584)));
			platforms[7]->setPosition(glm::vec2(float(tileMapDispl.x + 380), float(tileMapDispl.y + 1584)));
			platforms[8]->setPosition(glm::vec2(float(tileMapDispl.x + 284), float(tileMapDispl.y + 1536)));
			platforms[9]->setPosition(glm::vec2(float(tileMapDispl.x + 308), float(tileMapDispl.y + 1536)));
			platforms[10]->setPosition(glm::vec2(float(tileMapDispl.x + 308), float(tileMapDispl.y + 1464)));
			platforms[11]->setPosition(glm::vec2(float(tileMapDispl.x + 332), float(tileMapDispl.y + 1464)));
			break;
		case 5:
			break;
		case 6:
			if (balloonUp) {
				if (balloonDispl.y > 4) {
					balloonUp = false;
					if (rope[0]->animation() == ROPE_MIDDLE) rope[0]->changeAnimation(ROPE_RIGHT);
					else if (rope[0]->animation() == ROPE_RIGHT) rope[0]->changeAnimation(ROPE_LEFT);
					else if (rope[0]->animation() == ROPE_LEFT) rope[0]->changeAnimation(ROPE_MIDDLE);
				}
				else balloonDispl.y += 0.35;
			}
			else {
				if (balloonDispl.y < -4) {
					balloonUp = true;
					if (rope[0]->animation() == ROPE_MIDDLE) rope[0]->changeAnimation(ROPE_RIGHT);
					else if (rope[0]->animation() == ROPE_RIGHT) rope[0]->changeAnimation(ROPE_LEFT);
					else if (rope[0]->animation() == ROPE_LEFT) rope[0]->changeAnimation(ROPE_MIDDLE);
				}
				else balloonDispl.y -= 0.35;
			}
			balloon[0]->setPosition(glm::vec2(float(tileMapDispl.x + balloonDispl.x + 230), float(tileMapDispl.y + balloonDispl.y + 2376)));
			rope[0]->setPosition(glm::vec2(float(tileMapDispl.x + balloonDispl.x + 230), float(tileMapDispl.y + balloonDispl.y + 2400)));
			break;
		case 7:
			break;
		case 8:
			break;
		case 9:
			break;
		case 10:
			break;
		case 11:
			break;
		default: break;
	}

}

void Player::render(int level)
{
	sprite->render();
	switch (level) {
	case 1:
		if (!bWallBroken) wall->render();
		else if(!bStrawberryCollected) strawberry->render();
		break;
	case 2:
		break;
	case 3:
		if (!bStrawberryCollected) strawberry->render(); 
		springLeft->render();
		springRight->render();
		break;
	case 4:
		if (!bStrawberryCollected) {
			wing[0]->render();
			wing[1]->render();
			strawberry->render();
		}
		for (int i = 0; i < 12; ++i) if(platformFrames[i] <30) platforms[i]->render();
		break;
	case 5:
		break;
	case 6:
		if (!bBalloonsCollected[0]) {
			balloon[0]->render();
			rope[0]->render();
		}
		break;
	case 7:
		break;
	case 8:
		if (!bBalloonsCollected[0]) {
			balloon[0]->render();
			rope[0]->render();
		}
		break;
	case 9:
		if (!bBalloonsCollected[0]) {
			balloon[0]->render();
			rope[0]->render();
		}
		if (!bBalloonsCollected[1]) {
			balloon[1]->render();
			rope[1]->render();
		}	break;
	case 10:
		break;
	case 11:
		break;
	default: break;
	}
}

void Player::setTileMap(TileMap* tileMap)
{
	map = tileMap;
}

void Player::setPosition(const glm::vec2& pos)
{
	posPlayer = pos;
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));
}

void Player::setInitialPosition(int X, int Y)
{
	initialX = X;
	initialY = Y;
}




