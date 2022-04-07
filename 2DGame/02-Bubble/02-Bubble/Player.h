#ifndef _PLAYER_INCLUDE
#define _PLAYER_INCLUDE


#include "Sprite.h"
#include "TileMap.h"


// Player is basically a Sprite that represents the player. As such it has
// all properties it needs to track its movement, jumping, and collisions.


class Player
{

public:
	void init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram);
	void update(int deltaTime, int level);
	void render();
	
	void setTileMap(TileMap *tileMap);
	void setPosition(const glm::vec2 &pos);
	void setInitialPosition(int X, int Y);
	
private:
	bool bJumping, bClimbing, bClimbJumping, bDash, bDashing, bDashUP, bDashDOWN, bDashLEFT, bDashRIGHT, bGrabbing;
	glm::ivec2 tileMapDispl, posPlayer;
	int jumpAngle, dashAngle, startY, climbJumpingCount, dashingCount, initialX, initialY, dashDirection, dashFrame, dashY;
	Texture spritesheet;
	Sprite *sprite;
	TileMap *map;
	bool bGodMode, bSlowMode, bDashMode;

};


#endif // _PLAYER_INCLUDE


