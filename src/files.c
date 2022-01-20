#include "_components.h"
#include "structures.h"
#include "iohandle.h"
#include "safe.h"

size_t getFileSize(char* fname) {
	FILE* f = fopen(fname, "rb");
	if (f == NULL) {
		return 0;
	}
	fseek(f, 0, SEEK_END);
	size_t size = ftell(f);
	fclose(f);
	return size;
}

struct Array getFiles(char* path, bool showHidden) {
	struct Array ret;
	ret.size = 0;
	ret.data = NULL;

	struct File add;

	DIR* dhnd = opendir(path);
	if (dhnd == NULL) {
		IOHandle_Quit();
		perror("opendir()");
		exit(1);
	}
	struct dirent* dir;

	while ((dir = readdir(dhnd)) != NULL) {
		if ((dir->d_type == DT_DIR) && ((dir->d_name[0] != '.') || showHidden || (strcmp(dir->d_name, "..") == 0))) {
			++ ret.size;
			ret.data = safeAlloc(ret.data, ret.size * sizeof(struct File));
			add.name = dir->d_name;
			add.type = FileType_Folder;
			((struct File*)ret.data)[ret.size - 1] = add;
		}
	}
	dhnd = opendir(path);
	if (dhnd == NULL) {
		IOHandle_Quit();
		perror("opendir()");
		exit(1);
	}
	while ((dir = readdir(dhnd)) != NULL) {
		add.name = dir->d_name;
		if ((dir->d_type != DT_DIR) && ((dir->d_name[0] != '.') || showHidden)) {
			++ ret.size;
			ret.data = safeAlloc(ret.data, ret.size * sizeof(struct File));
			add.type = FileType_File;
			add.size = getFileSize(add.name);
			((struct File*)ret.data)[ret.size - 1] = add;
		}
	}

	return ret;
}