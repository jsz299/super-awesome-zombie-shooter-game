#include "sprites.h"
#include "../inc/ST7735.h"
#include "Images.h"
#include "stdlib.h"
#include <math.h>

sprite_t player;
sprite_t bullets[MAX_BULLET_COUNT];
int bulletWait;

//================================================================
//Input: pointer to a sprite object. 
//Transforms the x and y coordinates of the sprite by their corresponding velocities

void MoveSprite(sprite_t *sprite)
{
	sprite->x += sprite->xVel;
	sprite->y -= sprite->yVel;
}

//Move zombie based on direction they're facing. Zombies can move the direction they're facing... 
void MoveZombie(sprite_t* sprite) {
	
	if (sprite->currentRotation == ForwardFacing) {
		sprite->y = sprite->y - sprite->yVel;
		sprite->doDraw = 1;
	}
	else if (sprite->currentRotation == RightFacing) {
		sprite->x = sprite->x + sprite->xVel;
		sprite->doDraw = 1;
	}
	else if (sprite->currentRotation == BackwardFacing) {
		sprite->y = sprite->y + sprite->yVel;
		sprite->doDraw = 1;
	}
	else if (sprite->currentRotation == LeftFacing) {
		sprite->x = sprite->x - sprite->xVel;
		sprite->doDraw = 1;
	}
}

//================================================================
//Input: pointer to a sprite object.
//Draws the sprite image to the screen according to its x and y coordinates and currentRotation

void DrawSprite(sprite_t *sprite)
{
	uint32_t imgIdx = 0;

	for(int i = 0; i < sprite->image[sprite->currentRotation].height; i++)
	{
		for(int j = 0; j < sprite->image[sprite->currentRotation].width; j++, imgIdx++)
		{
			if(sprite->image[sprite->currentRotation].image[imgIdx] != 0x07E0)
				ST7735_DrawPixel(sprite->x+j, sprite->y-i, sprite->image[sprite->currentRotation].image[imgIdx]); //skip pixel if =0x07E0
			else
				ST7735_DrawPixel(sprite->x+j, sprite->y-i, BACKGROUND_COLOR);
		}
	}
}

//================================================================
//Input: pointer to a sprite object.
//Erases the previous sprite image from the screen according to the state of the previous image (oldX, oldY, oldRotation).
//This is necessary to prevent trailing when moving a sprite; after changing coordinates of the image, we must erase the old image at the
//previous location before drawing the image at the new location. 

void EraseSprite(sprite_t *sprite)
{
	uint32_t imgIdx = 0;

	for(int i = 0; i < sprite->oldImage[sprite->oldRotation].height; i++)
	{
		for(int j = 0; j < sprite->oldImage[sprite->oldRotation].width; j++, imgIdx++)
		{
			ST7735_DrawPixel(sprite->oldX+j, sprite->oldY-i, BACKGROUND_COLOR);
		}
	}
}

//================================================================
//Initializes sprite objects at runtime.

void spritesInit()
{
	player.x = player.oldX = 64;                //initialize player
	player.y = player.oldY = 80;
	player.currentRotation = player.oldRotation = 0;
	player.image = player.oldImage = playerImage;
	player.health = 3; 
	player.doDraw = 1;
	player.isAlive = 1;
	
	for(int i = 0; i < MAX_BULLET_COUNT; i++) //initialize bullets[] 
	{
		bullets[i].image = BulletImage;
		//bullets[i].isAlive = 0;
		bullets[i].health = 0;
		bullets[i].oldImage = BulletImage;
	}
}

//================================================================
//Inputs: pointer to a sprite, and direction (LEFT or RIGHT)
//Changes the currentRotation attribute of the sprite object.

//Forward:  currentRotation = 0 
//Right:    currentRotation = 1
//Backward: currentRotation = 2
//Left:     currentRotation = 3

void rotateSprite(sprite_t *sprite, direction_t direction)
{
	if(direction == LEFT)
	{
		//saveSprite(sprite);
		sprite->currentRotation = (sprite->currentRotation - 1 + 4) % 4;
		sprite->doDraw = 1;
	}
	
	else if(direction == RIGHT)
	{
		//saveSprite(sprite);
		sprite->currentRotation = (sprite->currentRotation + 1) % 4;
		sprite->doDraw = 1;
	}
	
}

//================================================================
//Input: pointer to a sprite object.
//Called when drawing an updated sprite image to the display. Assigns the x and y coordinates and rotation value to oldX, oldY, oldRotation.

void saveSprite(sprite_t *sprite)
{
	sprite->oldX = sprite->x;
	sprite->oldY = sprite->y;
	sprite->oldRotation = sprite->currentRotation;
	sprite->oldImage = sprite->image;
}

//================================================================
//Input: index number of the bullets array.
//Sets the bullet's status to active and sets the coordinates and rotation based on player's location/rotation.
//Velocity set to BULLET_VELOCITY. Bullet starts at tip of player's gun and dies when it exits the screen or hits a zombie.

void setBullet(int bullet)         
{
	//bullets[bullet].isAlive = 1;
	bullets[bullet].health = 1;
	bullets[bullet].doDraw = 1;
	bullets[bullet].xVel = 0;
	bullets[bullet].yVel = 0;
	
	if(player.currentRotation == 0)
	{
	bullets[bullet].x = player.x + 4;
	bullets[bullet].y = player.y - 13;
	bullets[bullet].yVel = BULLET_VELOCITY;
	bullets[bullet].currentRotation = 0;
	}
	
	else if(player.currentRotation == 1)
	{
		bullets[bullet].x = player.x + 13;
		bullets[bullet].y = player.y - 4;
		bullets[bullet].xVel = BULLET_VELOCITY;
		bullets[bullet].currentRotation = 1; //horizontal bullet
	}
	
	else if(player.currentRotation == 2)
	{
		bullets[bullet].x = player.x + 4;
		bullets[bullet].y = player.y + 2;
		bullets[bullet].yVel = -BULLET_VELOCITY;
		bullets[bullet].currentRotation = 0;
	}
	
	else if(player.currentRotation == 3)
	{
		bullets[bullet].x = player.x - 2;
		bullets[bullet].y = player.y - 4;
		bullets[bullet].xVel = -BULLET_VELOCITY;
		bullets[bullet].currentRotation = 1;
	}
} 

//================================================================
//Iterates through bullets array and checks if they have reached the edges of the screen. If they have, set status to inactive.
//If bullet is still active, reset doDraw. 

void checkBullets()   
{
	for(int i = 0; i < MAX_BULLET_COUNT; i++)     
	{
		if(bullets[i].health)
		{
			MoveSprite(&bullets[i]);
			bullets[i].doDraw = 1;
		}
	}
	
	
	for(int i = 0; i < MAX_BULLET_COUNT; i++)
	{
		if(bullets[i].y < 0 || bullets[i].y > SCREEN_HEIGHT || bullets[i].x < 0 || bullets[i].x > SCREEN_WIDTH)
			bullets[i].health = 0;
	}
}

//================================================================
//Inputs: pointers to two sprite objects. 
//Returns 1 if sprites have collided, 0 otherwise. 
int checkCollisions(sprite_t *sprite1, sprite_t *sprite2)
{	
  int16_t L1 = sprite1->x;
  int16_t R1 = sprite1->x + sprite1->image[sprite1->currentRotation].width;
  int16_t T1 = sprite1->y - sprite1->image[sprite1->currentRotation].height;
  int16_t B1 = sprite1->y;
		
  int16_t L2 = sprite2->x;
  int16_t R2 = sprite2->x + sprite2->image[sprite2->currentRotation].width;
  int16_t T2 = sprite2->y - sprite2->image[sprite2->currentRotation].height;
	int16_t B2 = sprite2->y;
	
	int8_t dx;
	int8_t dy;
	
	
	//if(R1 > L2 && T1 > T2 && T1 < B2)
	
	
	if(R1 > L2 && R1 < R2) //1 overlaps 2 on the left side
	{
		dx = R1 - L2;
		if(B1 > T2 && B1 < B2) //1 overlaps 2 on the top side
		{
			dy = B1 - T2;
			if(dx < dy)
			{
				sprite1->x = sprite2->x - sprite1->image[sprite1->currentRotation].width; //left collision
     		sprite1->xVel = 0;
				MoveSprite(sprite1);
				return 1;
			}
			
			else if(dx > dy)
			{
				sprite1->y = sprite2->y - sprite2->image[sprite2->currentRotation].height; //top collision
				sprite1->yVel = 0;
				MoveSprite(sprite1);
				return 1;
			}
		}
		
		if(T1 > T2 && T1 < B2) //1 overlaps 2 on the bottom side
		{
			dy = B2 - T1;
			if(dx < dy)
			{
				sprite1->x = sprite2->x - sprite1->image[sprite1->currentRotation].width; //left collision
     		sprite1->xVel = 0;
				MoveSprite(sprite1);
				return 1;
			}
			
			else if (dx > dy)
			{
				sprite1->y = sprite2->y + sprite1->image[sprite1->currentRotation].height; //bottom collision
				sprite1->yVel = 0;	
				MoveSprite(sprite1);
				return 1;
			}
		}
		
		if(T2 > T1 && T2 < B1 && B2 > T1 && B2 < B1) //1 overlaps 2 on both top and bottom sides (2 is smaller than 1)
		{
			sprite1->x = sprite2->x - sprite1->image[sprite1->currentRotation].width; //left collision
     	sprite1->xVel = 0;
			MoveSprite(sprite1);
			return 1;
		}
	}
	
	if(L1 < R2 && L1 > L2) //1 overlaps 2 on the right side
	{
		dx = R2 - L1;
		
		if(B1 > T2 && B1 < B2) //1 overlaps 2 on the top side
		{
			dy = B1 - T2;
			if(dx < dy)
			{
				sprite1->x = sprite2->x + sprite2->image[sprite2->currentRotation].width; //right collision
				sprite1->xVel = 0;
				MoveSprite(sprite1);
				return 1;		    
			}
			
			else if(dx > dy)
			{
  			sprite1->y = sprite2->y - sprite2->image[sprite2->currentRotation].height; //top collision
				sprite1->yVel = 0;
				MoveSprite(sprite1);
				return 1;
			}
		}
		
		if(T1 > T2 && T1 < B2) //1 overlaps 2 on the bottom side
		{
			dy = B2 - T1;
			if(dx < dy)
			{
				sprite1->x = sprite2->x + sprite2->image[sprite2->currentRotation].width; //right collision
				sprite1->xVel = 0;		
				MoveSprite(sprite1);
				return 1;
			}
			
			else if(dx > dy)
			{
				sprite1->y = sprite2->y + sprite1->image[sprite1->currentRotation].height; //bottom collision
				sprite1->yVel = 0;	
				MoveSprite(sprite1);
				return 1;
			}
		}
		
	  if(T2 > T1 && T2 < B1 && B2 > T1 && B2 < B1) //1 overlaps 2 on both top and bottom sides (2 is smaller than 1)
		{
			sprite1->x = sprite2->x + sprite2->image[sprite2->currentRotation].width; //right collision
			sprite1->xVel = 0;		
			MoveSprite(sprite1);
			return 1;
		}
	}
	
	if(L2 > L1 && L2 < R1 && R2 > L1 && R2 < R1 && B1 > T2 && B1 < B2) //1 overlaps 2 on top, left, and right sides (2 is smaller than 1)
	{
		sprite1->y = sprite2->y - sprite2->image[sprite2->currentRotation].height; //top collision
		sprite1->yVel = 0;
		MoveSprite(sprite1);
		return 1;
	}
	
	if(L2 > L1 && L2 < R1 && R2 > L1 && R2 < R1 && B2 > T1 && B2 < B1) //1 overlaps 2 on bottom , left and right sides (2 is smaller than 1)
	{
		sprite1->y = sprite2->y + sprite1->image[sprite1->currentRotation].height; //bottom collision
		sprite1->yVel = 0;	
		MoveSprite(sprite1);
		return 1;
	}
	return 0;
}


sprite_t DrY;
sprite_t DrV;

char DrV_Message[] = "You might have lost now..."; 
char DrV_Message1[] = "But who said you"; 
char DrV_Message2[] = "only have one chance?";
char DrV_Message3[] = "Good luck! and"; 
char DrV_Message4[] = "you'll ace the exam!";
char DrY_Message[] = "Be fearless when ";
char DrY_Message1[] = "learning and try new";
char DrY_Message2[] = "things! ...BTW...";
char DrY_Message3[] = "Have you tried yoga?";

void EndGameInit(void){
	DrY.image = ProfessorImages;
	DrY.currentRotation = 0;
	DrV.currentRotation = 1;
	
	DrV.health = DrY.health = 1000;
	
	DrV.y = DrY.y = 96;
	DrV.x = 0;
	DrY.x = 100;
	
	DrV.xVel = DrY.xVel = DrY.yVel = DrV.yVel = 1;
	
	DrV.image = ProfessorImages;
	return;
}

void DrV_Movement(void){
	if(DrV.x + DrV.image[DrV.currentRotation].width == 80){
		DrV.x = DrV.x - DrV.xVel - 1;
	}
	else if((DrV.y+DrV.image[DrV.currentRotation].height)== 0){
		DrV.y = DrV.y + DrV.yVel + 1;
	}
	else if(DrY.y==120){
		DrV.y = DrV.y - DrV.yVel - 1; 
	}
	else if(DrY.x == 0){
		DrV.x = DrV.x + DrV.xVel + 1;
	}
	else{
		uint8_t randDirection = rand()%3;
		if(randDirection == 1){
			DrV.x = DrV.x + DrV.xVel;
		}
		else if(randDirection == 2){
			DrV.y = DrV.y + DrV.yVel;
		}
		else if (randDirection == 3){
			DrV.y = DrV.y - DrV.yVel;
		}
	return;
	}
}


void DrY_Movement(void){
	if(DrY.x + DrY.image[DrY.currentRotation].width == 160){
		DrY.x = DrY.x - DrY.xVel - 1;
	}
	else if((DrY.y+DrY.image[DrY.currentRotation].height)== 0){
		DrY.y = DrY.y + DrY.yVel + 1;
	}
	else if(DrY.y==120){
		DrY.y = DrY.y - DrY.yVel - 1; 
	}
	else if(DrY.x == 80){
		DrY.x = DrY.x + DrY.xVel + 1;
	}
	else{
		uint8_t randDirection = rand()%3;
		if(randDirection == 1){
			DrY.x = DrY.x + DrY.xVel;
		}
		else if(randDirection == 2){
			DrY.y = DrY.y + DrY.yVel;
		}
		else if (randDirection == 3){
			DrY.y = DrY.y - DrY.yVel;
		}
	return;
	}
}



//void MoveZombie(sprite_t *zombie)
//{
//	int32_t dx = (zombie->x - player.x);
//	int32_t dy = (zombie->y - player.y);
//	int8_t xVel = 1;
//	int8_t yVel = 1;
//	
//  if(dx < 0)
//		xVel = 1;
//	else if(dx > 0)
//		xVel = -1;
//	else
//		xVel = 0;
//	
//	if(dy < 0)
//		yVel = 1;
//	else if(dy > 0)
//		yVel = -1;
//	else
//		yVel = 0;
//	
//	zombie->x += xVel;
//	zombie->y += yVel; 
//	zombie->doDraw = 1;
//}




//void init_basicZombieImage(sprite_t *location, uint8_t index){
//	 location[index].image = basicZombieImage;
//	return;
//}

//void init_bigZombieImage(sprite_t *location, uint8_t index){
//	 location[index].image = bigZombieImage;
//	return;
//}










