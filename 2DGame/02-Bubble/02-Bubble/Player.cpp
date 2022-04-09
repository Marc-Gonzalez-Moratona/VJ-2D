#include <cmath>
#include <iostream>
#include <GL/glew.h>
#include <GL/glut.h>
#include "Player.h"
#include "Game.h"


#define JUMP_ANGLE_STEP 4
#define JUMP_HEIGHT 64
#define FALL_STEP 4
#define DASH_ANGLE_STEP 4
#define DASH_HEIGHT 96

enum PlayerAnims
{
	STAND_LEFT, STAND_RIGHT, MOVE_LEFT, MOVE_RIGHT, 
	LOOK_DOWN_LEFT, LOOK_DOWN_RIGHT, LOOK_UP_LEFT, LOOK_UP_RIGHT,
	JUMP_LEFT, JUMP_RIGHT, CLIMB_LEFT, CLIMB_RIGHT
};


void Player::init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram)
{
	bJumping = false;
	bClimbing = false;
	bClimbJumping = false;
	bDash = true;
	bDashing = false;
	bDashUP = false;
	bDashDOWN = false;
	bDashLEFT = false;
	bDashRIGHT = false;
	bGodMode = false;
	bDashMode = false;
	bSlowMode = false;
	bGrabbing = false;
	climbJumpingCount = 0;
	dashingCount = 0;
	dashDirection = 0;
	dashFrame = 0;
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
	
}


void Player::update(int deltaTime, int level)
{
	tileMapDispl.y = 25 - 432 * (level - 1);
	sprite->update(deltaTime);
	if (Game::instance().getKey('g')) bGodMode = !bGodMode;
	if (Game::instance().getKey('d')) bDashMode = !bDashMode;
	if (Game::instance().getKey('s')) bSlowMode = !bSlowMode;

	
	// JUMP
	if (Game::instance().getKey('c') && jumpAngle == 0 && !bClimbJumping) {
		if (sprite->animation() == STAND_RIGHT) sprite->changeAnimation(JUMP_RIGHT);
		else if (sprite->animation() == STAND_LEFT) sprite->changeAnimation(JUMP_LEFT);
		posPlayer.y += 3;
		if (map->collisionMoveDown(posPlayer, glm::ivec2(24, 24), &posPlayer.x, &posPlayer.y, initialX, initialY, bGodMode)) {
			bJumping = true;
			posPlayer.y -= 3;
		}
	}
	if (bJumping || bClimbJumping) {
		jumpAngle += JUMP_ANGLE_STEP;
		// FIRST HALF OF A JUMP
		if (jumpAngle <= 90) {
			if (!bDashing) posPlayer.y = int(startY - JUMP_HEIGHT * sin(3.14159f * jumpAngle / 180.f));
			if (map->collisionMoveUp(posPlayer, glm::ivec2(24, 24), &posPlayer.y)) {
				bJumping = false;
				bClimbJumping = false;
			}
		}
		// START TO FALL AFTER THE FIRST HALF
		else {
			bJumping = false;
			bClimbJumping = false;
		}
	}
	else {
		// FALLING
		if (bGrabbing) posPlayer.y += (FALL_STEP / 2);
		else posPlayer.y += FALL_STEP;
		// RESET THE STATE WHEN THE PLAYER TOUCHES THE GROUND
		if (map->collisionMoveDown(posPlayer, glm::ivec2(24, 24), &posPlayer.x, &posPlayer.y, initialX, initialY, bGodMode))
		{
			bClimbing = false;
			bJumping = false;
			bDashing = false;
			bClimbJumping = false;
			bGrabbing = false;
			if (!Game::instance().getKey('x')) {
				bDash = true;
				//dashFrame = 0;
				dashAngle = 0;
			}
			if (!Game::instance().getKey('c')) jumpAngle = 0;
			startY = posPlayer.y;
			dashY = posPlayer.y;
			if (sprite->animation() == JUMP_RIGHT || sprite->animation() == CLIMB_RIGHT) sprite->changeAnimation(STAND_RIGHT);
			else if (sprite->animation() == JUMP_LEFT || sprite->animation() == CLIMB_LEFT) sprite->changeAnimation(STAND_LEFT);
		}
		if (!bJumping && !bDashing) {
			// SECOND HALF OF A JUMP
			if (jumpAngle > 90 && jumpAngle < 180) jumpAngle += JUMP_ANGLE_STEP;
			/* A JUMP CAN HAVE MORE THAN 180 DEGREES
			WHEN THE PLAYER JUMPS TO A PLATFORM THAT IS
			AT A LOWER HEIGHT */
			else if (jumpAngle >= 180) {
				if (map->collisionMoveDown(posPlayer, glm::ivec2(24, 24), &posPlayer.x, &posPlayer.y, initialX, initialY, bGodMode)) {
					jumpAngle = 0;
					if (!bDashing) posPlayer.y = startY;
				}
			}
		}
	}

	// CLIMBJUMPING
	if (Game::instance().getKey('c') && bClimbing && !bJumping) {
		bClimbJumping = true;
		climbJumpingCount = 0;
		bJumping = false;
		bClimbing = false;
		bGrabbing = false;
		jumpAngle = 0;
		startY = posPlayer.y;
		if (sprite->animation() == STAND_LEFT || sprite->animation() == JUMP_LEFT) {
			if (map->collisionMoveLeft(posPlayer, glm::ivec2(24, 24))) posPlayer.x += 3;
			sprite->changeAnimation(JUMP_RIGHT);
		}
		else if (sprite->animation() == STAND_RIGHT || sprite->animation() == JUMP_RIGHT) {
			if (map->collisionMoveRight(posPlayer, glm::ivec2(24, 24)))	posPlayer.x -= 3;
			sprite->changeAnimation(JUMP_LEFT);
		}
	}
	if (bClimbJumping) {
		bGrabbing = false;
		if (sprite->animation() == JUMP_LEFT) {
			posPlayer.x -= 3;
			if (map->collisionMoveLeft(posPlayer, glm::ivec2(24, 24))) {
				posPlayer.x += 3;
				bClimbing = true;
			}
		}
		else if (sprite->animation() == JUMP_RIGHT) {
			posPlayer.x += 3;
			if (map->collisionMoveRight(posPlayer, glm::ivec2(24, 24))) {
				posPlayer.x -= 3;
				bClimbing = true;
			}
		}
	}

	// DASH
	if (Game::instance().getKey('x') && dashAngle == 0 && bDash) {
		bDash = false;
		bDashing = true;
		dashY = posPlayer.y;
	}
	if (bDashing) {
		bGrabbing = false;
		dashAngle += DASH_ANGLE_STEP;
		// FIRST HALF OF A DASH
		if (dashAngle <= 90) {
			posPlayer.y = int(dashY - DASH_HEIGHT * sin(3.14159f * dashAngle / 180.f));
			if (map->collisionMoveUp(posPlayer, glm::ivec2(24, 24), &posPlayer.y)) bDashing = false;
		}
		// START TO FALL AFTER THE FIRST HALF
		else bDashing = false;
	}
	else {
		// SECOND HALF OF A DASH
		if (dashAngle > 90 && dashAngle < 180) {
			dashAngle += DASH_ANGLE_STEP;
			posPlayer.y = int(dashY - DASH_HEIGHT * sin(3.14159f * dashAngle / 180.f));
		}
		/* A DASH CAN HAVE MORE THAN 180 DEGREES
		WHEN THE PLAYER DASHES TO A PLATFORM THAT IS
		AT A LOWER HEIGHT */
		else if (dashAngle >= 180) {
			if (map->collisionMoveDown(posPlayer, glm::ivec2(24, 24), &posPlayer.x, &posPlayer.y, initialX, initialY, bGodMode)) {
				dashAngle = 0;
				posPlayer.y = dashY;
			}
		}
	}

	// WALK
	if (bDash || dashAngle > 60) {
		// MOVE LEFT
		if (Game::instance().getSpecialKey(GLUT_KEY_LEFT) && !bClimbJumping)
		{
			posPlayer.x -= 3;
			if (map->collisionMoveLeft(posPlayer, glm::ivec2(24, 24))) {
				posPlayer.x += 3;
				if (bClimbing) {
					bGrabbing = true;
					sprite->changeAnimation(CLIMB_RIGHT);
				}
				else if (!map->collisionMoveDown(posPlayer + FALL_STEP, glm::ivec2(24, 24), &posPlayer.x, &posPlayer.y, initialX, initialY, bGodMode) && !Game::instance().getKey('c')) bClimbing = true;
			}
			else if (sprite->animation() != MOVE_LEFT) {
				sprite->changeAnimation(MOVE_LEFT);
				bGrabbing = false;
			}
		}
		// MOVE RIGHT
		else if (Game::instance().getSpecialKey(GLUT_KEY_RIGHT) && !bClimbJumping)
		{
			posPlayer.x += 3;
			if (map->collisionMoveRight(posPlayer, glm::ivec2(24, 24))) {
				posPlayer.x -= 3;
				if (bClimbing) {
					bGrabbing = true;
					sprite->changeAnimation(CLIMB_LEFT);
				}
				else if (!map->collisionMoveDown(posPlayer + FALL_STEP, glm::ivec2(24, 24), &posPlayer.x, &posPlayer.y, initialX, initialY, bGodMode) && !Game::instance().getKey('c')) bClimbing = true;
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
				if (map->collisionMoveRight(posPlayer, glm::ivec2(24, 24)) && !Game::instance().getKey('c')) bClimbing = true;
				posPlayer.x -= 6;
				if (map->collisionMoveLeft(posPlayer, glm::ivec2(24, 24)) && !Game::instance().getKey('c')) bClimbing = true;
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
	}

	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));
}

void Player::render()
{
	sprite->render();
}

void Player::setTileMap(TileMap *tileMap)
{
	map = tileMap;
}

void Player::setPosition(const glm::vec2 &pos)
{
	posPlayer = pos;
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));
}

void Player::setInitialPosition(int X, int Y)
{
	initialX = X;
	initialY = Y;
}




