#include <cmath>
#include <iostream>
#include <GL/glew.h>
#include <GL/glut.h>
#include "Player.h"
#include "Game.h"


#define JUMP_ANGLE_STEP 4
#define JUMP_HEIGHT 64
#define FALL_STEP 4
#define DASH_ANGLE_STEP 12
#define DASH_HEIGHT 72

enum PlayerAnims
{
	STAND_LEFT, STAND_RIGHT, MOVE_LEFT, MOVE_RIGHT,
	LOOK_DOWN_LEFT, LOOK_DOWN_RIGHT, LOOK_UP_LEFT, LOOK_UP_RIGHT,
	JUMP_LEFT, JUMP_RIGHT, CLIMB_LEFT, CLIMB_RIGHT
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
	dashDirection = 0;
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
			else cout << "Slow mode disabled" << endl;		bChanging = true;
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
			if (map->collisionMoveLeft(posPlayer, glm::ivec2(24, 24), &posPlayer.x, &posPlayer.y, initialX, initialY, bGodMode, 3)) {
				if (bSlowMode) posPlayer.x += 2;
				else posPlayer.x += 3;
				bClimbing = true;
			}
		}
		else if (sprite->animation() == JUMP_RIGHT) {
			if (bSlowMode) posPlayer.x += 2;
			else posPlayer.x += 3;
			if (map->collisionMoveRight(posPlayer, glm::ivec2(24, 24), &posPlayer.x, &posPlayer.y, initialX, initialY, bGodMode, 3)) {
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
			if (map->collisionMoveUp(posPlayer, glm::ivec2(24, 24), &posPlayer.x, &posPlayer.y, initialX, initialY, bGodMode)) bClimbJumping = false;
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

	if (bJumping) {
		if (bSlowMode) jumpAngle += JUMP_ANGLE_STEP/2;
		else jumpAngle += JUMP_ANGLE_STEP;
		// FIRST HALF OF A JUMP
		if (jumpAngle <= 90) {
			posPlayer.y = int(startY - JUMP_HEIGHT * sin(3.14159f * jumpAngle / 180.f));
			if (map->collisionMoveUp(posPlayer, glm::ivec2(24, 24), &posPlayer.x, &posPlayer.y, initialX, initialY, bGodMode)) bJumping = false;
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
			if (map->collisionMoveLeft(posPlayer, glm::ivec2(24, 24), &posPlayer.x, &posPlayer.y, initialX, initialY, bGodMode, 3)) posPlayer.x += 3;
			sprite->changeAnimation(JUMP_RIGHT);
		}
		else if (sprite->animation() == STAND_RIGHT || sprite->animation() == JUMP_RIGHT || sprite->animation() == CLIMB_LEFT) {
			if (map->collisionMoveRight(posPlayer, glm::ivec2(24, 24), &posPlayer.x, &posPlayer.y, initialX, initialY, bGodMode, 3)) posPlayer.x -= 3;
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
			if (map->collisionMoveUp(posPlayer, glm::ivec2(24, 24), &posPlayer.x, &posPlayer.y, initialX, initialY, bGodMode)) bDashing = false;
			switch (dashDirection) {
			case 0:
				if (bSlowMode) posPlayer.x += 2;
				else posPlayer.x += 4;
				if (map->collisionMoveRight(posPlayer, glm::ivec2(24, 24), &posPlayer.x, &posPlayer.y, initialX, initialY, bGodMode, 4)) {
					if (bSlowMode) posPlayer.x -= 2;
					else posPlayer.x -= 4;
					bDashing = false;
				}
				break;
			case 1:
				if (bSlowMode) posPlayer.x += 2;
				else posPlayer.x += 4;
				if (map->collisionMoveRight(posPlayer + FALL_STEP, glm::ivec2(24, 24), &posPlayer.x, &posPlayer.y, initialX, initialY, bGodMode, 4)) {
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
				if (map->collisionMoveLeft(posPlayer, glm::ivec2(24, 24), &posPlayer.x, &posPlayer.y, initialX, initialY, bGodMode, 4)) {
					if (bSlowMode) posPlayer.x += 2;
					else posPlayer.x += 4;
					bDashing = false;
				}
				if (!map->collisionMoveDown(posPlayer + FALL_STEP, glm::ivec2(24, 24), &posPlayer.x, &posPlayer.y, initialX, initialY, bGodMode)) posPlayer.y = int(dashY + DASH_HEIGHT * sin(3.14159f * dashAngle / 180.f));
				break;
			case 4:
				if (bSlowMode) posPlayer.x -= 2;
				else posPlayer.x -= 4;
				if (map->collisionMoveLeft(posPlayer, glm::ivec2(24, 24), &posPlayer.x, &posPlayer.y, initialX, initialY, bGodMode, 4)) {
					if (bSlowMode) posPlayer.x += 2;
					else posPlayer.x += 4;
					bDashing = false;
				}
				break;
			case 5:
				if (bSlowMode) posPlayer.x -= 2;
				else posPlayer.x -= 4;
				if (map->collisionMoveLeft(posPlayer, glm::ivec2(24, 24), &posPlayer.x, &posPlayer.y, initialX, initialY, bGodMode, 4)) {
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
				if (map->collisionMoveRight(posPlayer, glm::ivec2(24, 24), &posPlayer.x, &posPlayer.y, initialX, initialY, bGodMode, 4)) {
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


	// WALK
	// MOVE LEFT
	if (Game::instance().getSpecialKey(GLUT_KEY_LEFT) && !bClimbJumping && (!bDashing || (bDashing && dashDirection != 4 && dashDirection != 0)))
	{
		if (bSlowMode) posPlayer.x -= 2;
		else posPlayer.x -= 3;
		if (map->collisionMoveLeft(posPlayer, glm::ivec2(24, 24), &posPlayer.x, &posPlayer.y, initialX, initialY, bGodMode, 3)) {
			if (bSlowMode) posPlayer.x += 2;
			else posPlayer.x += 3;
			if (bClimbing && !bDashMode) {
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
		if (map->collisionMoveRight(posPlayer, glm::ivec2(24, 24), &posPlayer.x, &posPlayer.y, initialX, initialY, bGodMode, 3)) {
			if (bSlowMode) posPlayer.x -= 2;
			else posPlayer.x -= 3;
			if (bClimbing && !bDashMode) {
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
			if (map->collisionMoveRight(posPlayer, glm::ivec2(24, 24), &posPlayer.x, &posPlayer.y, initialX, initialY, bGodMode, 6) && !Game::instance().getKey('c')) {
				bClimbing = true;
			}
			posPlayer.x -= 6;
			if (map->collisionMoveLeft(posPlayer, glm::ivec2(24, 24), &posPlayer.x, &posPlayer.y, initialX, initialY, bGodMode, 3) && !Game::instance().getKey('c')) {
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
}

void Player::render()
{
	sprite->render();
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




