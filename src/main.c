#include "_components.h"
#include "iohandle.h"
#include "render.h"
#include "structures.h"
#include "files.h"
#include "uiwindow.h"
#include "constants.h"

enum ViewMode {
	ViewMode_DirView     = 0,
	ViewMode_FileOptions = 1,
	ViewMode_Dialog      = 2
};

int main(void) {
	IOHandle_Init();

	bool         run                 = true;
	uint16_t     input;
	struct Array files;
	bool         refresh             = true;
	size_t       selected            = 0;
	size_t       fileOptionsSelected = 0;
	size_t       scrollY             = 0;
	bool         showHidden          = false;
	bool         renderAbout         = false;

	struct Alert alert;
	alert.text = NULL;
	alert.time = 3001;

	enum ViewMode viewMode = ViewMode_DirView;

	struct UIWindow about = UIWindow_New("About", 2, 1, 45, 15);
	UIWindow_WriteContents(&about, APP_NAME);
	UIWindow_WriteContents(&about, 
		"\nwritten by MESYETI\n\n"
		"control+q: quit\n"
		"control+r: refresh\n"
		"control+a: toggle showing hidden files\n"
		"control+y: show file options"
	);

	struct UIWindow fileOptions = UIWindow_New("File Options", (COLS / 2) - 10, (LINES / 2) - 5, 20, 10);
	UIWindow_Selection_Init(&fileOptions);
	UIWindow_Selection_AddButton(&fileOptions, "New file");
	UIWindow_Selection_AddButton(&fileOptions, "New directory");
	UIWindow_Selection_AddButton(&fileOptions, "Rename");
	UIWindow_Selection_AddButton(&fileOptions, "Delete");
	UIWindow_Selection_AddButton(&fileOptions, "Duplicate");

	struct UIWindow textbox = UIWindow_New("", 0, 0, 30, 5);

	while (run) {
		fileOptions.x = (COLS  / 2)  - (fileOptions.w / 2);
		fileOptions.y = (LINES / 2)  - (fileOptions.h / 2);
		textbox.x     = (COLS  / 2)  - (textbox.w     / 2);
		textbox.y     = (LINES / 2)  - (textbox.h     / 2);
		about.x       = (COLS  / 2)  - (about.w       / 2);
		about.y       = (LINES / 2)  - (about.h       / 2);
		if (refresh) {
			free(files.data);
			files.data = NULL;
			files.size = 0;
			files = getFiles(".", showHidden);
			refresh = false;
		}

		RenderClear();
		RenderScreen(files, selected, scrollY, alert);
		if (viewMode == ViewMode_FileOptions) {
			UIWindow_Render(&fileOptions);
		}
		if (viewMode == ViewMode_Dialog) {
			UIWindow_Render(&textbox);
		}
		if (renderAbout) {
			UIWindow_Render(&about);
		}
		refresh();

		if (alert.time <= 3000) {
			alert.time += 1000/60;
		}

		switch (viewMode) {
			case ViewMode_DirView: {
				input = getch();
				switch (input) {
					case ctrl('q'): {
						run = false;
						break;
					}
					case ctrl('a'): {
						selected   = 0;
						showHidden = !showHidden;
						refresh    = true;
						alert.text = (char*) showHidden? "Showing hidden files" : "Hiding hidden files";
						alert.time = 0;
						break;
					}
					case ctrl('y'): {
						viewMode = ViewMode_FileOptions;
						fileOptions.selectionFinished = false;
						fileOptions.selectionSelected = 0;
						break;
					}
					case ctrl('r'): {
						refresh = true;
						break;
					}
					case ctrl('i'): {
						renderAbout = true;
						break;
					}
					case KEY_DOWN: {
						if (selected < files.size - 1) {
							++ selected;
							if (selected >= scrollY + LINES - 2) {
								++ scrollY;
							}
						}
						break;
					}
					case KEY_UP: {
						if (selected > 0) {
							-- selected;
							if (selected < scrollY)
								scrollY = selected;
						}
						break;
					}
					case ' ':
					case '\n': {
						if (renderAbout) {
							renderAbout = false;
						}
						else if (((struct File*)files.data)[selected].type == FileType_Folder) {
							chdir(((struct File*)files.data)[selected].name);
							refresh  = true;
							selected = 0;
							scrollY  = 0;
						}
						break;
					}
					default: {
						break;
					}
				}
				break;
			}
			case ViewMode_FileOptions: {
				if (!UIWindow_Selection_Input(&fileOptions)) {
					IOHandle_Quit();
					return 0;
				}
				if (fileOptions.selectionFinished) {
					UIWindow_Textbox_Delete(&textbox);
					if (strcmp(((char**)fileOptions.selectionButtons.data)[fileOptions.selectionSelected], "New file") == 0) {
						textbox.title   = "New file";
						textbox.content = "Enter new file name:";
						viewMode        = ViewMode_Dialog;
						UIWindow_Textbox_Init(&textbox);
					}
					else if (strcmp(((char**)fileOptions.selectionButtons.data)[fileOptions.selectionSelected], "New directory") == 0) {
						textbox.title   = "New directory";
						textbox.content = "Enter new directory name:";
						viewMode        = ViewMode_Dialog;
						UIWindow_Textbox_Init(&textbox);
					}
					if (strcmp(((char**)fileOptions.selectionButtons.data)[fileOptions.selectionSelected], "Rename") == 0) {
						if (strcmp(((struct File*)files.data)[selected].name, "..") == 0) {
							alert.text = "Can't rename this folder";
							alert.time = 0;
						}
						else {
							textbox.title   = "Rename file";
							textbox.content = "Enter new name:";
							viewMode        = ViewMode_Dialog;
							UIWindow_Textbox_Init(&textbox);
						}
					}
					else if (strcmp(((char**)fileOptions.selectionButtons.data)[fileOptions.selectionSelected], "Delete") == 0) {
						if (((struct File*) files.data)[selected].type == FileType_File) {
							if (remove(((struct File*)files.data)[selected].name) == 0) {
								alert.text = "File deleted";
								alert.time = 0;
								refresh    = true;
								if (selected > files.size - 2)
									selected = files.size - 2;
							}
							else {
								alert.text = "Error deleting file";
								alert.time = 0;
							}
						}
						else if (((struct File*) files.data)[selected].type == FileType_Folder) {
							if (strcmp(((struct File*)files.data)[selected].name, "..") == 0) {
								alert.text = "Can't delete this directory";
								alert.time = 0;
							}
							else {
								Files_DeleteDirectory(((struct File*)files.data)[selected].name);
								alert.text = "Directory deleted";
								alert.time = 0;
							}
						}
						viewMode = ViewMode_DirView;
					}
					else if (strcmp(((char**)fileOptions.selectionButtons.data)[fileOptions.selectionSelected], "Duplicate") == 0) {
						if (((struct File*)files.data)[selected].type == FileType_File) {
							char* add = " - Copy";
							char* newName = (char*) malloc(strlen(((struct File*)files.data)[selected].name) + strlen(add) + 1);
							strcpy(newName, ((struct File*)files.data)[selected].name);
							strcat(newName, add);
							Files_DuplicateFile(((struct File*)files.data)[selected].name, newName);
							alert.text = "Duplicated";
							alert.time = 0;
							viewMode = ViewMode_DirView;
							refresh = true;
						}
						else if (((struct File*)files.data)[selected].type == FileType_Folder) {
							if (strcmp(((struct File*)files.data)[selected].name, "..") == 0) {
								alert.text = "Can't duplicate this folder";
								alert.time = 0;
							}
							else {
							char* add = " - Copy";
								char* newName = (char*) malloc(strlen(((struct File*)files.data)[selected].name) + strlen(add) + 1);
								strcpy(newName, ((struct File*)files.data)[selected].name);
								strcat(newName, add);
								mkdir(newName, 0777);
								alert.text = "Duplicated";
								alert.time = 0;
								viewMode = ViewMode_DirView;
								refresh = true;
							}
						}
					}
				}
				break;
			}
			case ViewMode_Dialog: {
				if (!UIWindow_Textbox_Input(&textbox)) {
					run = false;
				}
				if (textbox.textboxFinished) {
					if (strcmp(((char**)fileOptions.selectionButtons.data)[fileOptions.selectionSelected], "Rename") == 0) {
						if (rename(((struct File*)files.data)[selected].name, textbox.textboxContent) != 0) {
							alert.text = "Failed to rename";
							alert.time = 0;
						}
						else {
							alert.text = "Renamed";
							alert.time = 0;
							refresh    = true;
						}
						UIWindow_Textbox_Delete(&textbox);
					}
					else if (strcmp(((char**)fileOptions.selectionButtons.data)[fileOptions.selectionSelected], "New file") == 0) {
						FILE* fhnd;
						fhnd = fopen(textbox.textboxContent, "w");
						if (fhnd == NULL) {
							alert.text = "Failed to create file";
							alert.time = 0;
						}
						else {
							fclose(fhnd);
							alert.text = "File created";
							alert.time = 0;
							refresh    = true;
						}
					}
					else if (strcmp(((char**)fileOptions.selectionButtons.data)[fileOptions.selectionSelected], "New directory") == 0) {
						if (mkdir(textbox.textboxContent, 0777) != 0) {
							alert.text = "Failed to create directory";
							alert.time = 0;
						}
						else {
							alert.text = "Directory created";
							alert.time = 0;
							refresh    = true;
						}
					}
					viewMode = ViewMode_DirView;
				}
				break;
			}
		}

		usleep(1000000/60);
	}

	IOHandle_Quit();
	return 0;
}