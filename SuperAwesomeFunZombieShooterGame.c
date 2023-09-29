// ******* Possible Hardware I/O connections*******************
// Slide pot pin 1 connected to ground
// Slide pot pin 2 connected to PD2/AIN5
// Slide pot pin 3 connected to +3.3V 
// buttons connected to PE0-PE3
// 32*R resistor DAC bit 0 on PB0 (least significant bit)
// 16*R resistor DAC bit 1 on PB1
// 8*R resistor DAC bit 2 on PB2 
// 4*R resistor DAC bit 3 on PB3
// 2*R resistor DAC bit 4 on PB4
// 1*R resistor DAC bit 5 on PB5 (most significant bit)
// LED on PD1
// LED on PD0

#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "../inc/ST7735.h"
#include "Random.h"
#include "TExaS.h"
#include "../inc/ADC.h"
#include "Images.h"
#include "../inc/wave.h"
#include "Timer1.h"
#include "SuperAwesomeFunZombieShooterGame.h"
#include "ADCSWTrigger.h"
#include <math.h>
#include <stdlib.h>
#include "sprites.h"
#include "zombie_mechanics.h"

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void Delay100ms(uint32_t count); // time delay in 0.1 seconds


uint32_t potData[2];
uint8_t playerCollisionValue;
uint8_t playerHealthWait = 0;

int leftButtonState = 0;
int leftButtonLastState = 0;
int rightButtonState = 0;
int rightButtonLastState = 0;


int killCount = 0;
char killStr[] = "000";
char updateScore = 1;
char healthCount[] = "3";


char englishScore[] = "Score: ";
char englishDead[] = "You have died.";
char spanishScore[] = "Puntaje: ";
char spanishDead[] = "Te has muerto.";
char EnglishHealthStr[] = "health: ";
char SpanishHealthStr[] = "salud: ";
	
char *score;
char *deadMessage;
char *healthStr;


void Delay100ms(uint32_t count)
{
	count *= 8000000;
	while(count)
		count--;
}

void THE_NUKE(void) {
    for (uint8_t index = 0; index < 15; index++) {
        onScreen_Zombies[index].health = 0;
    }
		ST7735_FillScreen(BACKGROUND_COLOR);
		player.doDraw = 1;
    return;
}

//================================================================

void PortF_Init()
{
	SYSCTL_RCGCGPIO_R |= 0b100000;
	while(!(SYSCTL_RCGCGPIO_R & 0b101110)){};
	GPIO_PORTF_DIR_R |= 0b1110;
	GPIO_PORTF_DEN_R |= 0b1110;
	GPIO_PORTF_PUR_R |= 0x10;
}

//================================================================
//Inputs: RED, BLUE, GREEN
//used for testing purposes.

void LED_Toggle(Color_t color)
{
	switch(color)
	{
		case RED:
			GPIO_PORTF_DATA_R ^= 0x2;
			break;
		case BLUE:
			GPIO_PORTF_DATA_R ^= 0x4;
			break;
		case GREEN:
			GPIO_PORTF_DATA_R ^= 0x8;
	}
}

//================================================================
//main game loop; triggered by timer1A, runs at 30Hz

void GameTasks()
{
	checkPots();


	
	leftButtonState = GPIO_PORTE_DATA_R & LEFT_BUTTON;
	rightButtonState = GPIO_PORTE_DATA_R & RIGHT_BUTTON;
	
	if(leftButtonState && !leftButtonLastState) //left button to rotate player anti-clockwise
	{
		rotateSprite(&player, LEFT);
		LED_Toggle(BLUE);
	}
	
	if(rightButtonState && !rightButtonLastState) //right button to rotate player clockwise
	{
		rotateSprite(&player, RIGHT);
		LED_Toggle(RED);
	}
	
	
	for(int i = 0; i < ZOMBIE_MAXSIZE; i++) //check collisions for each zombie on the screen
	{
		if(onScreen_Zombies[i].health <= 0) //skip zombie if dead
			continue;
		
//		zombie_DirectionFacing(&onScreen_Zombies[i]);
		
		for(int j = 0; j < MAX_BULLET_COUNT; j++)
		{
			if(bullets[j].health == 1)
			{
				if(checkCollisions(&bullets[j], &onScreen_Zombies[i])) //check collisions of each bullet with zombie
			  {
				onScreen_Zombies[i].health--;
					
				if(onScreen_Zombies[i].health <= 0)
				{
					onScreen_Zombies[i].doDraw = 1; //doDraw set to 1 so that can erase
					killCount++;
					updateScore = 1;
				}
					
				bullets[j].health = 0; //bullet goes inactive if hits zombie
				bullets[j].doDraw = 1; //doDraw set to 1 so that can erase
				}
			}

		}
		
		if(onScreen_Zombies[i].health) //check player collision with zombie, decrement player health if true
		{
			if(checkCollisions(&player, &onScreen_Zombies[i]) && !playerHealthWait)
			{
				//Wave_Ouch();
				//player.health--;
				playerHealthWait = 10; //add small amount of delay to give player time to move away from zombie
			}
			
//			else
//				MoveZombie(&onScreen_Zombies[i]);
		}	
	}

	for(int i = 0; i < ZOMBIE_MAXSIZE; i++)
	{
		if(onScreen_Zombies[i].health && collision(&onScreen_Zombies[i]) && !playerHealthWait)
		{
			//Wave_Ouch();
		  player.health--;
			updateScore = 1;
			playerHealthWait = 20; //add small amount of delay to give player time to move away from zombie
		}
	}
	
	if(playerHealthWait > 0)
		playerHealthWait--;
	
	zombie_Movement();
	checkBullets();
	checkZombies();
	spawnZombies();
	
	if((GPIO_PORTE_DATA_R & FIRE_BUTTON) && bulletWait == 0)// && !soundActive)    //fire a bullet             
	{
		bulletWait = 10;
		for(int i = 0; i < MAX_BULLET_COUNT; i++)
		{
			if(bullets[i].health == 0)
			{
				setBullet(i);
				//Wave_GunSound();
				break;
			}
		}
	}
	else if(bulletWait > 0)
		bulletWait--;
	

	
	leftButtonLastState = leftButtonState;
	rightButtonLastState = rightButtonState;
}

//================================================================

void(*PortE_Task)(void);
void PortEButton_Init(void(*task)(void))
{
	SYSCTL_RCGCGPIO_R |= 0x10; //enable clock for port E
	while(!(SYSCTL_RCGCGPIO_R & 0x10)){};
	GPIO_PORTE_DIR_R &= ~0xF; //make PE0-3 input
	GPIO_PORTE_DEN_R |= 0xF; //digital enable PE0-3
	GPIO_PORTE_PDR_R |= 0xF; //pull-down resistors on PE0-3
	GPIO_PORTE_IS_R &= ~0xB; //PE0-3 edge sensitive
	GPIO_PORTE_IBE_R &= ~0xB; //not both edges
	GPIO_PORTE_IEV_R |= 0xB; //rising edge
	GPIO_PORTE_ICR_R = 0xB; //clear flags
	GPIO_PORTE_IM_R |= 0xB; //arm interrupts on PE0-3
	NVIC_PRI1_R &= ~0xE0;
	NVIC_PRI1_R |= 0x00; //set priority to 1 (bits 5-7)
	PortE_Task = task;
	//NVIC_EN0_R |= 0x10; //enable interrupt 4 in IVT	
}

void GPIOPortE_Handler()
{
	GPIO_PORTE_ICR_R = 0xF;
	(*PortE_Task)();
}

//================================================================
//Triggered by PortE_Handler edge triggered interrupts. 

void Buttons()
{
	if(GPIO_PORTE_DATA_R & LEFT_BUTTON)
	{
		LED_Toggle(RED); //for testing
		rotateSprite(&player, LEFT);
	}
	
	else if(GPIO_PORTE_DATA_R & RIGHT_BUTTON)
	{
		LED_Toggle(GREEN); //for testing
		rotateSprite(&player, RIGHT);
	}

	else if(GPIO_PORTE_DATA_R & PAUSE_BUTTON)
	{
		LED_Toggle(RED); //for testing
		LED_Toggle(BLUE); //for testing
		
		THE_NUKE();
	}
}

//================================================================
//Input: pointer to a sprite object.
//Call this to draw a sprite image to the display. 

void LCD_Out(sprite_t *sprite)
{
	EraseSprite(sprite);
	//ST7735_FillRect(sprite->oldX, (sprite->oldY - sprite->oldImage[sprite->oldRotation].height), sprite->oldImage[sprite->oldRotation].width, sprite->oldImage[sprite->oldRotation].height, 0x6143);
	if(sprite->health)
		DrawSprite(sprite);
	
	sprite->doDraw = 0; 
	saveSprite(sprite);
}

//================================================================
//Checks the value of the potentiometers and adjusts the x and y coordinates of the player accordingly.

void checkPots()
{
	ADC_In89(potData);
	int32_t pot1 = potData[0];
	int32_t pot2 = potData[1];
	player.xVel = 0;
	player.yVel = 0;
	
	if((pot1 > 2548) && (player.x <= SCREEN_WIDTH - player.image[player.currentRotation].width)) //move right
	{
		player.xVel = (pot1 - 2548) / 310;
		MoveSprite(&player);
		player.doDraw = 1;
	}
	else if((pot1 < 1548) && (player.x >= 0)) //move left
	{
		player.xVel = (pot1 - 1548) / 310;
		MoveSprite(&player);
		player.doDraw = 1;
	}
	if((pot2 > 2548) && (player.y >= player.image[player.currentRotation].height)) //move down
	{
		player.yVel = (pot2 - 2548) / 310;
		MoveSprite(&player);
		player.doDraw = 1;
	}
	else if((pot2 < 1548) && (player.y <= SCREEN_HEIGHT)) //move up
	{
		player.yVel = (pot2 - 1548)  / 310;
		MoveSprite(&player);
		player.doDraw = 1;
	}
}


char englishStr[] = "GREEN for English";
char spanishStr[] = "ROJA para Espanol";
//================================================================
void menuScreen()
{
	ST7735_DrawBitmap(0, 127, startscreen, 160, 128);
	Delay100ms(2);
	ST7735_DrawString(4, 7, englishStr, ST7735_YELLOW);
	ST7735_DrawString(4, 8, spanishStr, ST7735_YELLOW);
	while(1)
	{
		if(GPIO_PORTE_DATA_R & LEFT_BUTTON) //for English
		{
			score = englishScore;
			deadMessage = englishDead;
			healthStr = EnglishHealthStr;
			return;
		}
		
		else if(GPIO_PORTE_DATA_R & RIGHT_BUTTON)
		{
			score = spanishScore;
			deadMessage = spanishDead;
			healthStr = SpanishHealthStr;
			return;
		}
	}
}

//================================================================

void playerHasDied()
{
	TIMER1_CTL_R = 0x00000000; //disable Timer 1A
	//Wave_Dammit();
	ST7735_FillScreen(BACKGROUND_COLOR); //erase screen
	ST7735_SetCursor(0, 0);
	ST7735_OutString(deadMessage);
	Delay100ms(3);
	menuScreen();
}

//================================================================

void startGame()
{
	Round = 0; //reset to round 0;
	killCount = 0;
	count_BasicZombies = 1;
	count_BigZombies = 0;
	reset_onScreen_Zombies();
	spawnZombies();
	spritesInit();
	LCD_Out(&player);
	ST7735_FillScreen(BACKGROUND_COLOR);
	TIMER1_CTL_R = 0x00000001;
}

//================================================================
int main()
{
	DisableInterrupts();
	TExaS_Init(NONE);
	Timer1_Init(GameTasks, 80000000/30, 0); //30Hz
	PortF_Init(); //for testing
	PortEButton_Init(Buttons); //for testing
	ST7735_InitR(INITR_REDTAB);
	ST7735_SetRotation(1);
	ADC_Init89();
	generateSeed("jsz299");
	Wave_Init();
	menuScreen();
	startGame();
	EnableInterrupts();
	while(1) //main loop handles output to display
	{

		
		if(updateScore)
		{
			updateScore = 0;
			ST7735_SetCursor(0, 0);
			ST7735_OutString(score);
			ST7735_SetCursor(9, 0);
			sprintf(killStr, "%d", killCount);
			ST7735_OutString(killStr);
			ST7735_SetCursor(0, 1);
			ST7735_OutString(healthStr);
			sprintf(healthCount, "%d", player.health);
			ST7735_SetCursor(8, 1);
			ST7735_OutString(healthCount);
		}
		if(player.health <= 0)
		{
			
			EndGameInit();
			ST7735_FillScreen(ST7735_BLACK);
			LCD_Out(&DrV);
			while((GPIO_PORTE_DATA_R & 0x04) != 0x04){
				
			}
			ST7735_SetCursor(0,0);
			ST7735_OutString(DrV_Message);
			while((GPIO_PORTE_DATA_R & 0x02) != 0x02){
				
			}
			ST7735_SetCursor(0,1);
			ST7735_OutString(DrV_Message1);
			while((GPIO_PORTE_DATA_R & 0x04) != 0x04){
				
			}
			ST7735_SetCursor(0,2);
			ST7735_OutString(DrV_Message2);
			while((GPIO_PORTE_DATA_R & 0x02) != 0x02){
				
			}
			ST7735_SetCursor(0,3);
			ST7735_OutString(DrV_Message3);
			while((GPIO_PORTE_DATA_R & 0x04) != 0x04){
				
			}
			ST7735_SetCursor(0,4);
			ST7735_OutString(DrV_Message4);
			while((GPIO_PORTE_DATA_R & 0x02) != 0x02){
				
			}
			LCD_Out(&DrY);
			ST7735_SetCursor(0, 10);
			ST7735_OutString(DrY_Message);
			while((GPIO_PORTE_DATA_R & 0x04) != 0x04){
				
			}
			ST7735_SetCursor(0, 11);
			ST7735_OutString(DrY_Message1);
			while((GPIO_PORTE_DATA_R & 0x02) != 0x02){
			}				
			ST7735_SetCursor(0,12);
			ST7735_OutString(DrY_Message2);
			while((GPIO_PORTE_DATA_R & 0x04) != 0x04){
				
			}
			ST7735_SetCursor(0,7);
			ST7735_OutString(DrY_Message3);
			while((GPIO_PORTE_DATA_R & 0x02) != 0x02){
				
			}
			playerHasDied();
			startGame();
						
		}
		if(player.doDraw)  //draw player if moved
			LCD_Out(&player);
		
		for(int i = 0; i < MAX_BULLET_COUNT; i++) //index through bullet array. If bullet.doDraw == 1, draw bullet to display. 
		{
			if(bullets[i].doDraw)
			{
				LCD_Out(&bullets[i]);
			}
		}
		
		for(int i = 0; i < ZOMBIE_MAXSIZE; i++) //index through zombies array. If zombie.doDraw == 1, draw to display. 
		{
			if(onScreen_Zombies[i].doDraw)
			{
				LCD_Out(&onScreen_Zombies[i]);
			}
		}

			
	}
}


