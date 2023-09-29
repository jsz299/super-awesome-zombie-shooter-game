#ifndef BACKGROUNDSETTINGS_H
#define BACKGROUNDSETTINGS_H

#include "Images.h"

#define startScreen 0
#define MainMenuNewGame 1
#define MainMenuLanguageEnglish 2
#define MainMenuLanguageSpanish 3
#define cutscene1 4
#define cutscene2 5
#define cutscene3 6
#define gameOverMenu 7
#define pauseScreen 8
#define inGame 9

#define leftButton 0x1
#define rightButton 0x2
#define shootButton 0x4
#define pauseButton 0x8

/* BackgroundSettings header and c file serve the purposes of creating Start Menu interaction, Pause menu interaction, 
and other miscelaneous functions that aren't dealing with the actual gameplay */

//initialize the starting screen
extern void startOfGame();
//pause the game
extern void pauseGame(void);
//cutscenes after playerdeath
extern void playerDeath(void);

#endif // BACKGROUNDSETTINGS_H
