
#include <stdint.h>
#include <stdlib.h>
#include "zombie_mechanics.h"
//#include "Sprites.h"
//#include "sprites.h"


#define TRUE 1
#define FALSE 0


//will eventually move this function to the sprites.h file but for now it's here for debugging purposes
//this function will be used to determine the center of a sprite 
static uint8_t spriteCenterX(sprite_t* sprite) {
	uint8_t currentRotationImage = sprite->currentRotation;
	uint8_t centerX = sprite->x + (sprite->image[currentRotationImage].width)/2;
	return centerX;
}

static uint8_t spriteCenterY(sprite_t* sprite) {
	uint8_t currentRotationImage = sprite->currentRotation;
	uint8_t centerY = sprite->y + (sprite->image[currentRotationImage].height) / 2;
	return centerY;
}

void zombie_DirectionFacing(sprite_t *zombie);

//public variables that can be accessed by the main program
uint8_t Round = 0;
uint16_t count_BasicZombies = 1;
uint8_t count_BigZombies = 0;
sprite_t onScreen_Zombies[ZOMBIE_MAXSIZE];

//private variables that we don't want the user to be able to datamine
static uint8_t random_zombie_type;
static uint8_t random_x_spawn;
static uint8_t random_y_spawn;
static uint8_t zombies_LoadedIn = 0; //will be used to decrement the count of big and basic zombies and to load in the zombies 

/* Using static so that the player cannot access the seed that we're using for the random number generator...
The seed will be generated using the player's UTEID */
static void seed_generator(char* ID) {
	uint32_t unique_ID = 0;
	for (uint8_t i = 0; *(ID + i) != 0x00; i++) {
		unique_ID = *(ID + i) + unique_ID;
	}
	if (unique_ID == 0) {
		srand(23983);
	}
	else {
		srand(unique_ID);
	}
}

//function that checks if the zombie is already on the player
uint8_t collision(sprite_t* zombie) {
//	int8_t x_distance_right = abs(player.x - zombie->x);
//	int8_t y_distance_bottom = abs(player.y - zombie->y);
//	
//	int8_t x_distance_left = abs(player.x - (zombie->x + zombie->image->width));
//	int8_t y_distance_top = abs(player.y - (zombie->y + zombie->image->height));


//	if ((x_distance_right <= player.image->width) && ((y_distance_bottom <= player.image->height)||(y_distance_top <= player.image->height))) {
//		return TRUE;
//	}
//	
//	else if ((x_distance_left <= player.image->width) && ((y_distance_bottom <= player.image->height) || (y_distance_top <= player.image->height))) {
//		return TRUE;
//	}

//	else {
//		return 0;
	int8_t PL = player.x;
	int8_t PR = PL + player.image[player.currentRotation].width;
	int8_t PT = player.y;
	int8_t PB = PT + player.image[player.currentRotation].height;
	
	int8_t ZL = zombie->x;
	int8_t ZR = ZL + zombie->image[zombie->currentRotation].width;
	int8_t ZT = zombie->y;
	int8_t ZB = ZT + zombie->image[zombie->currentRotation].height;
	
	if(PR < ZL || PL > ZR)
		return 0;
	
	if(PB < ZT || PT > ZB)
		return 0;
	
	return 1;
	}


static uint8_t is_player_on_left(sprite_t* zombie) {
	uint8_t playerCurrentRotation = player.currentRotation;
	if (spriteCenterX(zombie) > spriteCenterX(&player)) {
		return TRUE;
	}
	else {
		return FALSE;
	}
}

static uint8_t is_player_on_right(sprite_t* zombie) {
	if (spriteCenterX(zombie) < spriteCenterX(&player)) {
		return TRUE;
	}
	else {
		return FALSE;
	}
}

static uint8_t is_player_above(sprite_t* zombie) {
	if (spriteCenterY(zombie) > spriteCenterY(&player)) {
		return TRUE;
	}
	else {
		return FALSE;
	}
}

static uint8_t is_player_below(sprite_t* zombie) {
	if (spriteCenterY(zombie) < spriteCenterY(&player)) {
		return TRUE;
	}
	else {
		return FALSE;
	}
}


//function will be used to constantly determine the direction the zombies are facing...
void zombie_DirectionFacing(sprite_t *zombie) {
	//Idea is to iterate through the onScreenZombie array and make sure that while x and y 
	//are updated by the zombie_Movement function that the orientation will always be correct
	//Want to include some degree of randomness in the code... Gives zombies potentially 2 random movements based on players x and y coordinates...
	//Origin of the display ~ (x,y) = (0,0) ~ is top left. Zombie and player (x,y) is the bottom left of the image... 

	int randomMovement = rand() % 2;

	if (is_player_below(zombie) == TRUE) {
		if (is_player_on_right(zombie) == TRUE && randomMovement == TRUE) {
			zombie->currentRotation = RightFacing;
		}
		else if (is_player_on_left(zombie) == TRUE && randomMovement == TRUE) {
			zombie->currentRotation = LeftFacing;
		}
		else {
			zombie->currentRotation = BackwardFacing;
		}
	}
	else if (is_player_above(zombie) == TRUE) {
		if (is_player_on_right(zombie) == TRUE && randomMovement == TRUE) {
			zombie->currentRotation = RightFacing;
		}
		else if (is_player_on_left(zombie) == TRUE && randomMovement == TRUE) {
			zombie->currentRotation = LeftFacing;
		}
		else {
			zombie->currentRotation = ForwardFacing;
		}
	}
	
	else if (is_player_on_right(zombie) == TRUE) {
		zombie->currentRotation = RightFacing;
	}
	
	else if (is_player_on_left(zombie) == TRUE) {
		zombie->currentRotation = LeftFacing;
	}

}

/* This function will be used to reset the onScreen_Zombies array... and has the following uses...
1. set the identity of the element in the array to 'n'... this is an indicator for a draw function
 in a separate module to draw nothing.
2. set the health of each element in the array to 0 so that zombies can be spawned in and take its place */
void reset_onScreen_Zombies() {
	for (uint8_t index = 0; index < ZOMBIE_MAXSIZE; index++) {
		onScreen_Zombies[index].identity = 'n'; //'n' will be used to identify nothing
		onScreen_Zombies[index].health = 0;
		onScreen_Zombies[index].x = 0;
		onScreen_Zombies[index].y = 0;
		onScreen_Zombies[index].xVel = 0;
		onScreen_Zombies[index].yVel = 0;
	}
}


/* Using static here will help optimize the compiler... because this function
is only called in the zombie_mechanics.c file */

/* Function will be used to generate a basic zombie given an x and y position that's given by
the zombieWaves function.*/
static void basicZombieInit(uint8_t x, uint8_t y) {
	uint8_t index;

	//find the dead zombie
	for (index = 0; onScreen_Zombies[index].health != 0; index++) {

	}

	//create identifier... 'a' will be for basic zombie
	onScreen_Zombies[index].identity = 'a';

	//create spawn point
	onScreen_Zombies[index].x = x;
	onScreen_Zombies[index].y = y;

	//set initial zombie facing direction when spawned in
	if (y == 0) {
		onScreen_Zombies[index].currentRotation = 2;
	}
	else if (x == 0) {
		onScreen_Zombies[index].currentRotation = 1;
	}
	else if (y == 128) {
		onScreen_Zombies[index].currentRotation = 0;
	}
	else if (x == 160) {
		onScreen_Zombies[index].currentRotation = 3;
	}
	
	//puts mem address of image into zombies attributes
	onScreen_Zombies[index].image = onScreen_Zombies[index].oldImage = basicZombieImage;

	//set health of the zombie 
	onScreen_Zombies[index].health = 3;

	onScreen_Zombies[index].xVel = 1;
	onScreen_Zombies[index].yVel = 1;

	return;
}

/* Function will be used to generate a big zombie given an x and y position that's passed
through the zombieWaves function. */
static void bigZombieInit(uint8_t x, uint8_t y) {
	uint8_t index;

	//find the dead zombie
	for (index = 0; onScreen_Zombies[index].health != 0; index++) {

	}

	//create identifier... b indicatede BIG zombie
	onScreen_Zombies[index].identity = 'b';

	//create spawn point
	onScreen_Zombies[index].x = x;
	onScreen_Zombies[index].y = y;

	//set initial zombie facing direction when spawned in
	if (y == 0) {
		onScreen_Zombies[index].currentRotation = 2;
	}
	else if (x == 0) {
		onScreen_Zombies[index].currentRotation = 1;
	}
	else if (y == 128) {
		onScreen_Zombies[index].currentRotation = 0;
	}
	else if (x == 160) {
		onScreen_Zombies[index].currentRotation = 3;
	}

	//pass the memory address of the bigZombie image structure... a separate 
	// draw function in another module will then draw the image through...
	//Colors ... onScreen_Zombies[index].image

	onScreen_Zombies[index].image = onScreen_Zombies[index].oldImage = bigZombieImage;

	//set health of the zombie 
	onScreen_Zombies[index].health = 20;
	
	onScreen_Zombies[index].xVel = 1;
	onScreen_Zombies[index].yVel = 1;

	return;

}

/* Generates the zombie type and coordinates for a spawning zombie */
static void determine_spawn() {
	//determine zombie type that's spawned.. 1 indicates big zombie 0 indicates basic zombie
	random_zombie_type = rand() % 2;
	// determine if zombie is spawning at the top/bottom (1) or left/right (0) of the LCD screen
	uint8_t random_zombie_spawn_xORy = rand() % 2;

	if (random_zombie_spawn_xORy == 1) {
		//determine a random x_spawn... 160 is the width of our LCD screen
		random_x_spawn = rand() % 160;
		if (random_x_spawn == 0) {
			random_x_spawn = random_x_spawn + 1; //prevent both x and y position from becoming 0
		}
		else if (random_x_spawn == 160) {
			random_x_spawn = random_x_spawn - 1; //prevent both x and y position from becoming 0
		}
		random_y_spawn = (rand() % 2) * 128; //determine a random y that is either bottom (0) or top(128)
	}
	else {
		//determine a random y_spawn... 128 is the height of our LCD screen
		random_y_spawn = rand() % 128;
		if (random_y_spawn == 0) {
			random_y_spawn = random_y_spawn + 1; //prevent both x and y position from becoming 0
		}
		else if (random_y_spawn == 160) {
			random_y_spawn = random_y_spawn - 1; //prevent both x and y position from becoming 0
		}
		random_x_spawn = (rand() % 2) * 160; //determine a random x that is either left (0) or right (160)
	}
	return;
}

/* Function that's purpose is to update the zombie waves... This function will be triggered
by checkZombies function... Code will have the following functionality...
1. increment the round by 1
2. Before round 5 we'll increase the count of basic_zombies by 2 for each new round
3. On Round 5 there will be 1 big zombie and 1 basic zombie
4. After Round 5 the count of basic zombies will increment by 2 for each new round
5. The count of big zombies will increment by 1 after every 3 rounds */
static void newZombieWave() {
	Round++;

	if (Round < 5) {
		count_BasicZombies = 2 * Round + 1;
	}
	else {
		count_BasicZombies = 2 * (Round - 5) + 1;
		count_BigZombies = (Round - 2) / 3;
	}

	reset_onScreen_Zombies();

	return;
}

/* Function will be public so that it can be called by the main program to generate a seed based off UTEID*/
void generateSeed(char* ID) {
	seed_generator(ID);
}

/* Function will be used to fill an array of zombies using the ZombieInit functions.
The amount of zombies created will be determined by the ZombieWaves function/Round the games on */
void spawnZombies() {
	for (; (count_BasicZombies + count_BigZombies) > 0 && zombies_LoadedIn < ZOMBIE_MAXSIZE; zombies_LoadedIn++) {
		determine_spawn(); //load in values to fill the zombie structure
		if ((random_zombie_type == 1 && count_BigZombies > 0) || (count_BasicZombies == 0 && count_BigZombies > 0)) {
			bigZombieInit(random_x_spawn, random_y_spawn);
			count_BigZombies = count_BigZombies - 1;
		}
		else if ((random_zombie_type == 0 && count_BasicZombies > 0) || (count_BigZombies == 0 && count_BasicZombies > 0)) {
			basicZombieInit(random_x_spawn, random_y_spawn);
			count_BasicZombies = count_BasicZombies - 1;
		}
	}
	return;
}

// Function that calls  ZombieWaves if all on screen zombies are dead
void checkZombies() {
	uint8_t dead_zombies;
	for (dead_zombies = 0; (onScreen_Zombies[dead_zombies].health == 0 && dead_zombies < ZOMBIE_MAXSIZE); dead_zombies++) {
		if (zombies_LoadedIn > 0) {
			zombies_LoadedIn--;
		}
	}
	if ((count_BasicZombies + count_BigZombies) == 0 && dead_zombies == ZOMBIE_MAXSIZE) {
		newZombieWave();
	}
	return;
}

//function will be used for the zombie so that it can walk towards the player constantly... 
//probably will be activated through a non-intrusive interrupt so that other code is working at the same time
void zombie_Movement(void) {
	for (uint8_t index = 0; index < ZOMBIE_MAXSIZE; index++) {
		if (onScreen_Zombies[index].health != 0) {
			uint8_t no_move = collision(&onScreen_Zombies[index]);
			if (no_move != TRUE) {
				zombie_DirectionFacing(&onScreen_Zombies[index]);
				MoveZombie(&onScreen_Zombies[index]);
			}
		}
	}
	return;
}


//testing purposes to spawn
void test_spawn(void) {
	determine_spawn();
	basicZombieInit(60, 50);
	return;
}
