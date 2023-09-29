#ifndef ZOMBIE_MECHANICS_H
#define ZOMBIE_MECHANICS_H

#include "Sprites.h"

#define ZOMBIE_MAXSIZE 5

extern uint8_t Round;
extern uint16_t count_BasicZombies;
extern uint8_t count_BigZombies;
extern sprite_t onScreen_Zombies[ZOMBIE_MAXSIZE];
//we can change the array amount but figured 15 is a good starting point for reduced screen clutter

void checkZombies();
void spawnZombies();
void zombie_Movement();
void generateSeed(char* ID);
void zombie_DirectionFacing(sprite_t *zombie);
uint8_t collision(sprite_t* zombie);
void reset_onScreen_Zombies();

//debugging purposes
void test_spawn();
#endif //ZOMBIE_MECHANICS_H