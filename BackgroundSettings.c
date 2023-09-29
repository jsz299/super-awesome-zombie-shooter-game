#include "BackgroundSettings.h"
#include "../inc/ST7735.h"
#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
// Path: BackgroundSettings.c

uint8_t currentBackground = startScreen;

static void previousOption(void);
static void nextOption(void);
static void checkButton(void);
static void selectOption(void);
static void resumeGame(void);
static void drawBackground(void);

void pauseGame(void);


static void checkButton(void) {
	if ((GPIO_PORTE_DATA_R & leftButton) == leftButton) {
		previousOption();
	}
	else if ((GPIO_PORTE_DATA_R & rightButton) == rightButton) {
		nextOption();
	}
	else if ((GPIO_PORTE_DATA_R & shootButton) == shootButton) {
		selectOption();
	}
	else if ((GPIO_PORTE_DATA_R & pauseButton) == pauseButton) {
		resumeGame();
	}
	return;
}


static void resumeGame(void) {
	currentBackground = inGame;
	return;
}

//Function to draw a background depending on situation
static void drawBackground(void) {
	ST7735_DrawBitmap(0, 0, backgroundOptions[currentBackground].image, 160, 128); //draw current display
	return;
}


//Function that will select an option in the game
static void selectOption(void) {
	if (currentBackground == MainMenuNewGame) {
		currentBackground = inGame;
	}
	else if (currentBackground == MainMenuLanguageEnglish || currentBackground == MainMenuLanguageSpanish) {
		return;
	}
	else if (currentBackground == pauseScreen) {
		return;
	}
	else {
		currentBackground++;
	}
	return;
}

//function that changes the screen to the next one. 
static void nextOption(void) {
	if (currentBackground == MainMenuLanguageSpanish) {
		currentBackground = MainMenuLanguageEnglish;
		return;
	}
	else if (currentBackground == startScreen) {
		return;
	}
	else if (currentBackground == gameOverMenu) {
		return;
	}
	else if (currentBackground == pauseScreen) {
		return;
	}
	else {
		currentBackground++;
		return;
	}
}

//function that changes the screen to the previous one. At certain points we can't go back to previous screen... as defined in the function.
static void previousOption(void) {
	if (currentBackground == startScreen) {
		return;
	}
	else if (currentBackground == cutscene1) {
		return;
	}
	else if (currentBackground == gameOverMenu) {
		return;
	}
	else if (currentBackground == pauseScreen) {
		return;
	}
	else {
		currentBackground--;
		return;
	}
}

//function for debugging purposes... 
static void mimicButtons(void) {
	char button = 0x00;
	//printf("Type 'a' to go back to previous cutscene... type 'd' to goto next cutscene... type 's' to shoot/select option... : ");
	//scanf_s("%c", &button);
	if (button == 'a') {
		previousOption();
	}
	if (button == 'd') {
		nextOption();
	}
	if (button == 's') {
		selectOption();
	}
	if (button == 'p') {
		resumeGame();
	}
	else {
		return;
	}
}


//initialize the start screen and settings before gameplay starts
void startOfGame(void) {
	drawBackground();
	//printf("\nCurrent Background: %d\n", currentBackground);
	if (currentBackground != inGame) {
		//mimicButtons();
		checkButton();
		startOfGame();
	}
	else {
		return;
	}
}

//Function that will pause the game displaying the startscreen from the beginning of the game until shoot button is pressed indicating game will continue
void pauseGame(void) {
	currentBackground = pauseScreen;
	drawBackground();
	while (currentBackground == pauseScreen) {
		//mimicButtons();
		checkButton();
	}
	return;
}

//cutscenes after playerdeath
void playerDeath(void) {
	if (currentBackground == inGame) {
		currentBackground = cutscene1;
	}
	drawBackground();
	//printf("\nCurrent Background: %d", currentBackground);
	if (currentBackground != gameOverMenu) {
		//mimicButtons();
		checkButton();
		playerDeath();
		return;
	}
	if (currentBackground == gameOverMenu) {
		while (1) {

		}
	}
}