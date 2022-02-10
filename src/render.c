#include "_components.h"
#include "structures.h"
#include "constants.h"
#include "iohandle.h"

void RenderClear(void) {
	for (size_t i = 1; i<LINES; ++i) {
		mvhline(i, 0, ' ', COLS);
	}
}

void RenderScreen(struct Array files, size_t selected, size_t scrollY, struct Alert alert) {
	// render top bar
	move(0, 0);
	attron(A_REVERSE);
	mvhline(0, 0, ' ', COLS);
	move(0, 0);
	printw("%s |", APP_NAME);
	attroff(A_REVERSE);

	// render files
	move(1, 0);
	for (size_t i = scrollY; (i<files.size) && (i < (LINES - 2) + scrollY); ++i) {
		if (i == selected) attron(A_REVERSE);
		if (((struct File*)files.data)[i].type == FileType_Folder) {
			attron(A_BOLD);
			printw("%s/\n", ((struct File*)files.data)[i].name);
			attroff(A_BOLD);
		}
		else {
			printw("%s\n", ((struct File*)files.data)[i].name);
		}
		if (i == selected) attroff(A_REVERSE);
	}

	move(0, COLS-10);
	attron(A_REVERSE);
	printw("File size");
	attroff(A_REVERSE);

	for (size_t i = 1; i<LINES; ++i) {
		mvhline(i, COLS-10, ' ', 10);
	}
	for (size_t i = scrollY; (i<files.size) && (i < (LINES - 2) + scrollY); ++i) {
		move(i - scrollY + 1, COLS - 10);
		if (((struct File*)files.data)[i].type == FileType_File) {
			if (((struct File*)files.data)[i].size >= 1024) {
				printw("%ldKB", ((struct File*)files.data)[i].size / 1024);
			}
			else if (((struct File*)files.data)[i].size >= 1024 * 1024) {
				printw("%ldMB", ((struct File*)files.data)[i].size / (1024 * 1024));
			}
			else if (((struct File*)files.data)[i].size >= 1024 * 1024 * 1024) {
				printw("%ldGB", ((struct File*)files.data)[i].size / (1024 * 1024 * 1024));
			}
			else {
				printw("%ldB", ((struct File*)files.data)[i].size);
			}
		}
	}

	// render alert
	if (alert.time <= 3000) {
		move(LINES / 2, COLS / 2 - strlen(alert.text) / 2 - 2);
		attron(A_REVERSE);
		printw("[ %s ]", alert.text);
		attroff(A_REVERSE);
	}

	// render bottom bar
	char wd[PATH_MAX];
	attron(A_REVERSE);
	mvhline(LINES - 1, 0, ' ', COLS);
	move(LINES - 1, 0);
	if (getcwd(wd, PATH_MAX) != NULL) {
		printw("%s", wd);
	}
	else {
		IOHandle_Quit();
		perror("getcwd");
		exit(1);
	}
	attroff(A_REVERSE);
}