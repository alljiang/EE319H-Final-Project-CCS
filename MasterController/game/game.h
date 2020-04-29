//
// Created by Allen on 2/28/2020.
//

#include <cstdint>

#ifndef GAME_H
#define GAME_H

extern void game_startup();
extern void game_loop();
extern void startup();
extern void switchStageMenuToCharMenu(int8_t stageSelect);
extern void switchCharMenuToGame(int8_t char1, int8_t char2);
extern void switchGameToWin(int8_t survivor, int8_t character);
extern void switchWinToStageMenu();
extern void loop();

#endif //GAME_H
