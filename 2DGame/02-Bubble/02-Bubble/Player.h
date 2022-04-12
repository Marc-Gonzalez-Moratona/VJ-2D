#ifndef _PLAYER_INCLUDE
#define _PLAYER_INCLUDE


#include "Sprite.h"
#include "TileMap.h"


// Player is basically a Sprite that represents the player. As such it has
// all properties it needs to track its movement, jumping, and collisions.


class Player
{

public:
	void init(const glm::ivec2& tileMapPos, ShaderProgram& shaderProgram);
	void update(int deltaTime, int level);
	void render(int level);

	void setTileMap(TileMap* tileMap);
	void setPosition(const glm::vec2& pos);
	void setInitialPosition(int X, int Y);
	bool spriteCollision(Sprite *s1, Sprite *s2, bool isWall);
	void resetLevel();
	void setStrawberryDispl(glm::fvec2 strawberryDispl);

private:
	bool bJumping, bClimbing, bClimbJumping, bDashing, bGrabbing, bWallBroken, bStrawberryCollected, strawberryUp;
	glm::ivec2 tileMapDispl, posPlayer;
	glm::fvec2 strawberryDispl;
	int jumpAngle, climbJumpAngle, dashAngle, startY, initialX, initialY, dashDirection, dashY;
	Texture spritesheet, strawberrysheet, springsheet, baloonsheet, wallsheet, wingsheet, platformsheet;
	Sprite *sprite, *strawberry, *springLeft, *springRight, *baloon, *wall, *wing[2], *platforms[12];
	TileMap *map;
	bool bGodMode, bSlowMode, bDashMode, bChanging;

};


#endif // _PLAYER_INCLUDE


