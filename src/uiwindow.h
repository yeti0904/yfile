#pragma once
#include "_components.h"
#include "structures.h"

struct UIWindow {
	// main stuff
	char*  title;
	size_t x, y, w, h;
	char*  content;

	// selection
	bool         selection;
	struct Array selectionButtons;
	size_t       selectionSelected;
	bool         selectionFinished;

	// textbox
	bool   textbox;
	size_t textboxCursor;
	char*  textboxContent;
	bool   textboxFinished;
};

struct UIWindow UIWindow_New(char* title, size_t x, size_t y, size_t w, size_t h);
void            UIWindow_Delete(struct UIWindow* window);
void            UIWindow_ClearContents(struct UIWindow* window);
void            UIWindow_WriteContents(struct UIWindow* window, char* content);
void            UIWindow_Render(struct UIWindow* window);
void            UIWindow_Selection_Init(struct UIWindow* window);
void            UIWindow_Selection_AddButton(struct UIWindow* window, char* label);
bool            UIWindow_Selection_Input(struct UIWindow* window);
void            UIWindow_Selection_Delete(struct UIWindow* window);
void            UIWindow_Textbox_Init(struct UIWindow* window);
bool            UIWindow_Textbox_Input(struct UIWindow* window);
void            UIWindow_Textbox_Delete(struct UIWindow* window);