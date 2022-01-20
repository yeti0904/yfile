#include "_components.h"
#include "iohandle.h"
#include "render.h"
#include "structures.h"
#include "files.h"

int main(void) {
	IOHandle_Init();

	bool         run        = true;
	uint16_t     input;
	struct Array files;
	bool         refresh    = true;
	size_t       selected   = 0;
	size_t       scrollY    = 0;
	bool         showHidden = false;

	struct Alert alert;
	alert.text = NULL;
	alert.time = 3001;

	while (run) {
		if (refresh) {
			files = getFiles(".", showHidden);
			refresh = false;
		}
		RenderScreen(files, selected, scrollY, alert);

		if (alert.time <= 3000) {
			alert.time += 1000/60;
		}

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
			case KEY_DOWN: {
				if (selected < files.size - 1) {
					++ selected;
					if (selected >= scrollY + LINES - 1) {
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
				if (((struct File*)files.data)[selected].type == FileType_Folder) {
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

		usleep(1000000/60);
	}

	IOHandle_Quit();
	return 0;
}