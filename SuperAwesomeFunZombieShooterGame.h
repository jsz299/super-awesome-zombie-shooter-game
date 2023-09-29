#ifndef SUPERAWESOMEFUNZOMBIESHOOTERGAME_H
#define SUPERAWESOMEFUNZOMBIESHOOTERGAME_H

#define LEFT_BUTTON 0x1
#define RIGHT_BUTTON 0x2
#define PAUSE_BUTTON 0x8
#define FIRE_BUTTON 0x4
//================================================================

enum Color
{
	RED,
	GREEN,
	BLUE
};
typedef enum Color Color_t;

//================================================================

void PortF_Init();
void LED_Toggle(Color_t color);
void GameTasks();
void PortEButton_Init(void(*task)(void));
void Buttons();
void checkPots();
void playerHasDied();





#endif