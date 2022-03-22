#include <cmath>
#include <iostream>
#include <GL/glew.h>
#include <GL/glut.h>
#include "Player.h"
#include "Game.h"


#define JUMP_ANGLE_STEP 4
#define JUMP_HEIGHT 60
#define FALL_STEP 4


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

void Player::update(int deltaTime)
{
	sprite->update(deltaTime);
	if (Game::instance().getKey('1')) {
		map->changeLevel(1);
	}
	else if(Game::instance().getKey('2')){
		map->changeLevel(2);
	}
	else if(Game::instance().getSpecialKey(GLUT_KEY_LEFT))
	{
		if(sprite->animation() == JUMP_RIGHT)
			sprite->changeAnimation(JUMP_LEFT);
		else if(sprite->animation() != MOVE_LEFT)
			if (!bJumping) sprite->changeAnimation(MOVE_LEFT);
		posPlayer.x -= 3;
		if(map->collisionMoveLeft(posPlayer, glm::ivec2(24, 24)))
		{
			posPlayer.x += 3;
			if (bJumping && jumpAngle > 90 || !bJumping && !map->collisionMoveDown(posPlayer, glm::ivec2(24, 24), &posPlayer.y)) {
				bClimbing = true;
				sprite->changeAnimation(CLIMB_RIGHT);
			}
			else {
				bClimbing = false;
				if (sprite->animation() != JUMP_LEFT) sprite->changeAnimation(STAND_LEFT);
			}
		}
	}
	else if(Game::instance().getSpecialKey(GLUT_KEY_RIGHT))
	{
		if (sprite->animation() == JUMP_LEFT)
			sprite->changeAnimation(JUMP_RIGHT);
		else if(sprite->animation() != MOVE_RIGHT)
			if(!bJumping) sprite->changeAnimation(MOVE_RIGHT);
		posPlayer.x += 3;
		if(map->collisionMoveRight(posPlayer, glm::ivec2(24, 24)))
		{
			posPlayer.x -= 3;
			if (bJumping && jumpAngle > 90 || !bJumping && !map->collisionMoveDown(posPlayer, glm::ivec2(24, 24), &posPlayer.y)) {
				bClimbing = true;
				sprite->changeAnimation(CLIMB_LEFT);
			}
			else {
				bClimbing = false;
				if (sprite->animation() != JUMP_RIGHT) sprite->changeAnimation(STAND_RIGHT);
			}
		}
	}
	else if (Game::instance().getSpecialKey(GLUT_KEY_DOWN))
	{
		if (sprite->animation() != LOOK_DOWN_RIGHT && sprite->animation() != LOOK_DOWN_LEFT) {
			if (sprite->animation() == STAND_LEFT)
				sprite->changeAnimation(LOOK_DOWN_LEFT);
			else if (sprite->animation() == STAND_RIGHT)
				sprite->changeAnimation(LOOK_DOWN_RIGHT);
		}
	}
	else if (Game::instance().getSpecialKey(GLUT_KEY_UP))
	{
		if (sprite->animation() != LOOK_UP_RIGHT && sprite->animation() != LOOK_UP_LEFT) {
			if (sprite->animation() == STAND_LEFT)
				sprite->changeAnimation(LOOK_UP_LEFT);
			else if (sprite->animation() == STAND_RIGHT) 
				sprite->changeAnimation(LOOK_UP_RIGHT);
		}
	}
	else if (Game::instance().getKey('z'))
	{
		if (Game::instance().getSpecialKey(GLUT_KEY_LEFT)) {
			if (sprite->animation() == STAND_LEFT)
				sprite->changeAnimation(LOOK_UP_LEFT);
			else if (sprite->animation() == STAND_RIGHT)
				sprite->changeAnimation(LOOK_UP_RIGHT);
		}
	}
	else
	{
		if(sprite->animation() == MOVE_LEFT)
			sprite->changeAnimation(STAND_LEFT);
		else if(sprite->animation() == MOVE_RIGHT)
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
	
	if(bJumping)
	{
		if (bClimbing) {
			jumpAngle += (JUMP_ANGLE_STEP / 3);
		}
		else jumpAngle += JUMP_ANGLE_STEP;
		if(jumpAngle == 180)
		{
			bJumping = false;
			posPlayer.y = startY;
		}
		else
		{
			posPlayer.y = int(startY - JUMP_HEIGHT * sin(3.14159f * jumpAngle / 180.f));
			if (jumpAngle < 90)
				if (map->collisionMoveUp(posPlayer, glm::ivec2(24, 24), &posPlayer.y)) bJumping = false;

			if (jumpAngle > 90) bJumping = !map->collisionMoveDown(posPlayer, glm::ivec2(24, 24), &posPlayer.y);
		}
	}
	else
	{
		if (bClimbing) {
			posPlayer.y += (FALL_STEP / 3);
		}
		else posPlayer.y += FALL_STEP;
		if (map->collisionMoveDown(posPlayer, glm::ivec2(24, 24), &posPlayer.y))
		{
			bClimbing = false;
			if (sprite->animation() == JUMP_RIGHT || sprite->animation() == CLIMB_RIGHT) sprite->changeAnimation(STAND_RIGHT);
			else if (sprite->animation() == JUMP_LEFT || sprite->animation() == CLIMB_LEFT) sprite->changeAnimation(STAND_LEFT);
			
			if (Game::instance().getKey(' '))
			{
				bJumping = true;
				jumpAngle = 0;
				startY = posPlayer.y;
				if (sprite->animation() != JUMP_RIGHT && sprite->animation() != JUMP_LEFT) {
					if (sprite->animation() == STAND_LEFT || sprite->animation() == MOVE_LEFT || sprite->animation() == LOOK_DOWN_LEFT || sprite->animation() == LOOK_UP_LEFT)
						sprite->changeAnimation(JUMP_LEFT);
					else if (sprite->animation() == STAND_RIGHT || sprite->animation() == MOVE_RIGHT || sprite->animation() == LOOK_DOWN_RIGHT || sprite->animation() == LOOK_UP_RIGHT)
						sprite->changeAnimation(JUMP_RIGHT);
				}
			}
		}
		else {
			if (sprite->animation() == STAND_LEFT || sprite->animation() == MOVE_LEFT)
				sprite->changeAnimation(JUMP_LEFT);
			else if (sprite->animation() == STAND_RIGHT || sprite->animation() == MOVE_RIGHT)
				sprite->changeAnimation(JUMP_RIGHT);
		}
	}
	if (bClimbing){
		if (Game::instance().getKey(' '))
		{
			bJumping = true;
			jumpAngle = 0;
			startY = posPlayer.y;
			if (sprite->animation() != JUMP_RIGHT && sprite->animation() != JUMP_LEFT) {
				if (sprite->animation() == CLIMB_LEFT) {
					posPlayer.x -= 3;
					sprite->changeAnimation(JUMP_LEFT);
				}
				else if (sprite->animation() == CLIMB_RIGHT) {
					posPlayer.x += 3;
					sprite->changeAnimation(JUMP_RIGHT);
				}
			}
		}
		else if (!Game::instance().getSpecialKey(GLUT_KEY_LEFT) && !Game::instance().getSpecialKey(GLUT_KEY_LEFT)) bClimbing = false;
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




