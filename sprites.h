#ifndef SPRITES_H
#define SPRITES_H


#include <stdint.h>
#include "Images.h"


#define MAX_BULLET_COUNT 5
#define BULLET_VELOCITY 10
#define BACKGROUND_COLOR 0x3961
#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 128

struct sprite
{
	int x;
	int y;
	int xVel;
	int yVel;
	uint8_t currentRotation;
	const spriteImage_t *image;
	int health; 
	int doDraw;
	uint8_t isAlive;
	int oldX;
	int oldY;
	uint8_t oldRotation;
	const spriteImage_t *oldImage;
	char identity;
};
typedef struct sprite sprite_t;


enum direction
{
	LEFT,
	RIGHT,
	UP,
	DOWN,
};
typedef enum direction direction_t;

extern sprite_t player;
extern sprite_t bullets[MAX_BULLET_COUNT];
extern int bulletWait;

void MoveSprite(sprite_t *sprite);
void DrawSprite(sprite_t *sprite);
void spritesInit();
void rotateSprite(sprite_t *sprite, direction_t direction);
void EraseSprite(sprite_t *sprite);
void saveSprite(sprite_t *sprite);
void setBullet(int bullet);
void checkBullets();
int checkCollisions(sprite_t *sprite1, sprite_t *sprite2);
void init_basicZombieImage(sprite_t *location, uint8_t index);
void init_bigZombieImage(sprite_t *location, uint8_t index);
void MoveZombie(sprite_t* sprite);

extern sprite_t DrY;
extern sprite_t DrV;
extern char DrY_Message[];
extern char DrY_Message1[];
extern char DrY_Message2[];
extern char DrY_Message3[];
extern char DrV_Message[];
extern char DrV_Message1[];
extern char DrV_Message2[];
extern char DrV_Message3[];
extern char DrV_Message4[];

void EndGameInit(void);
void DrV_Movement(void);
void DrY_Movement(void);
#endif