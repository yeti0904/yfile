#include "uiwindow.h"
#include "structures.h"
#include "safe.h"
#include "iohandle.h"

struct UIWindow UIWindow_New(char* title, size_t x, size_t y, size_t w, size_t h) {
	struct UIWindow window;
	struct Array    buttons;

	window.title   = title;
	window.x       = x;
	window.y       = y;
	window.w       = w;
	window.h       = h;
	window.content = safeMalloc(1);

	strcpy(window.content, "");

	buttons.data = NULL;
	buttons.size = 0;

	window.selection         = false;
	window.selectionButtons  = buttons;
	window.selectionSelected = 0;
	window.selectionFinished = false;

	window.textbox         = false;
	window.textboxCursor   = 0;
	window.textboxContent  = NULL;
	window.textboxFinished = false;

	return window;
}

void UIWindow_Delete(struct UIWindow* window) {
	free(window->content);
	free(window->selectionButtons.data);
}

void UIWindow_ClearContents(struct UIWindow* window) {
	window->content = safeRealloc(window->content, 1);
	strcpy(window->content, "");
}

void UIWindow_WriteContents(struct UIWindow* window, char* content) {
	window->content = safeRealloc(window->content, strlen(window->content) + strlen(content) + 1);
	strcat(window->content, content);
}

void UIWindow_Render(struct UIWindow* window) {
	// render background
	attron(A_REVERSE);
	for (size_t i = 0; i<window->h; ++i) {
		mvhline(window->y + i, window->x, ' ', window->w);
	}

	// render border
	mvhline(window->y, window->x + 1, ACS_HLINE, window->w - 1);
	mvhline(window->y + window->h, window->x + 1, ACS_HLINE, window->w - 1);
	mvvline(window->y + 1, window->x, ACS_VLINE, window->h - 1);
	mvvline(window->y + 1, window->x + window->w, ACS_VLINE, window->h - 1);
	mvaddch(window->y, window->x, ACS_ULCORNER);
	mvaddch(window->y, window->x + window->w, ACS_URCORNER);
	mvaddch(window->y + window->h, window->x, ACS_LLCORNER);
	mvaddch(window->y + window->h, window->x + window->w, ACS_LRCORNER);

	// render title
	attron(A_BOLD);
	mvaddstr(window->y, window->x + (window->w / 2) - (strlen(window->title) / 2), window->title);
	attroff(A_BOLD);

	// render content
	size_t cols = 0, lines = 0;
	for (size_t i = 0; i<strlen(window->content); ++i) {
		move(window->y + 1 + lines, window->x + 1 + cols);
		switch (window->content[i]) {
			case '\n': {
				++ lines;
				cols = 0;
				break;
			}
			default: {
				addch(window->content[i]);
				++ cols;
				break;
			}
		}
	}

	attroff(A_REVERSE);

	// render selection
	if (window->selection) {
		for (size_t i = 0; i<window->selectionButtons.size; ++i) {
			if (i == window->selectionSelected)
				attron(A_REVERSE);

			mvhline(window->y + i + 1, window->x + 2, ' ', window->w - 2);
			mvaddstr(window->y + i + 1, window->x + 2, ((char**)window->selectionButtons.data)[i]);

			if (i == window->selectionSelected) {
				attroff(A_REVERSE);
				move(window->y + i + 1, window->x + 1);
				addch('>');
			}
		}
	}
	if (window->textbox) {
		mvhline(window->y + window->h - 1, window->x + 1, ' ', window->w - 2);
		if (window->textboxContent == NULL) {
			IOHandle_Quit();
			printf("Error: textbox content is somehow NULL\n");
			exit(1);
		}
		for (size_t i = 0; (i<=strlen(window->textboxContent)) && (i<window->w); ++i) {
			move(window->y + window->h - 1, window->x + 1 + i);
			if (i == window->textboxCursor)
				attron(A_REVERSE);
			if (window->textboxContent[i] == '\0') {
				addch(' ');
			}
			else {
				addch(window->textboxContent[i]);
			}
			if (i == window->textboxCursor)
				attroff(A_REVERSE);
		}
	}
}

void UIWindow_Selection_Init(struct UIWindow* window) {
	window->selection             = true;
	window->selectionButtons.data = NULL;
	window->selectionButtons.size = 0;
	window->selectionSelected     = 0;
	window->selectionFinished     = false;
}

void UIWindow_Selection_AddButton(struct UIWindow* window, char* label) {
	++ window->selectionButtons.size;
	window->selectionButtons.data = safeAlloc(window->selectionButtons.data, window->selectionButtons.size * sizeof(char**));
	((char**)window->selectionButtons.data)[window->selectionButtons.size - 1] = safeMalloc(strlen(label) + 1);
	strcpy(((char**)window->selectionButtons.data)[window->selectionButtons.size - 1], label);
}

bool UIWindow_Selection_Input(struct UIWindow* window) {
	uint16_t input = getch();
	switch (input) {
		case KEY_UP: {
			if (window->selectionSelected > 0)
				-- window->selectionSelected;
			break;
		}
		case KEY_DOWN: {
			if (window->selectionSelected < window->selectionButtons.size - 1)
				++ window->selectionSelected;
			break;
		}
		case ' ':
		case '\n': {
			window->selectionFinished = true;
			break;
		}
		case ctrl('q'): {
			return false;
		}
		default: {
			break;
		}
	}
	return true;
}

void UIWindow_Selection_Delete(struct UIWindow* window) {
	for (size_t i = 0; i<window->selectionButtons.size; ++i) {
		free(((char**)window->selectionButtons.data)[i]);
	}
	free(window->selectionButtons.data);
	window->selectionButtons.size = 0;
}

void UIWindow_Textbox_Init(struct UIWindow* window) {
	window->textbox         = true;
	window->textboxCursor   = 0;
	window->textboxContent  = safeMalloc(1);
	*window->textboxContent = '\0';
	window->textboxFinished = false;
}

bool UIWindow_Textbox_Input(struct UIWindow* window) {
	uint16_t input = getch();
	switch (input) {
		case KEY_LEFT: {
			if (window->textboxCursor > 0)
				-- window->textboxCursor;
			break;
		}
		case KEY_RIGHT: {
			if (window->textboxCursor < strlen(window->textboxContent))
				++ window->textboxCursor;
			break;
		}
		case 127:
		case KEY_BACKSPACE: {
			if (window->textboxCursor > 0) {
				for (size_t i = window->textboxCursor; i<strlen(window->textboxContent); ++i) {
					window->textboxContent[i - 1] = window->textboxContent[i];
				}
				-- window->textboxCursor;
				window->textboxContent[strlen(window->textboxContent) - 1] = '\0';
			}
			break;
		}
		case '\n': {
			window->textboxFinished = true;
			break;
		}
		case ctrl('q'): {
			return false;
		}
		default: {
			if (input >= 32 && input <= 126) {
				window->textboxContent = safeRealloc(window->textboxContent, strlen(window->textboxContent) + 2);
				for (size_t i = strlen(window->textboxContent); i>window->textboxCursor; --i) {
					window->textboxContent[i] = window->textboxContent[i - 1];
				}
				window->textboxContent[window->textboxCursor] = input;
				++ window->textboxCursor;
			}
			break;
		}
	}
	return true;
}

void UIWindow_Textbox_Delete(struct UIWindow* window) {
	free(window->textboxContent);
	window->textboxContent = NULL;
}