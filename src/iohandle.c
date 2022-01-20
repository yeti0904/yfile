#include "_components.h"

void IOHandle_Init() {
	initscr();
	raw();
	noecho();
	keypad(stdscr, true);
	nodelay(stdscr, true);
	curs_set(0);
}

void IOHandle_Quit() {
	endwin();
}