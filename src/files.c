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

	add.name = (char*) "..";
	add.type = FileType_Folder;
	++ ret.size;
	ret.data = safeAlloc(ret.data, sizeof(struct File) * ret.size);
	((struct File*)ret.data)[ret.size - 1] = add;

	while ((dir = readdir(dhnd)) != NULL) {
		if ((dir->d_type == DT_DIR) && ((dir->d_name[0] != '.') || showHidden ) && (strcmp(dir->d_name, "..") != 0)) {
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

int nftw_callback(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
	int ret = remove(fpath);

	if (ret != 0) {
		IOHandle_Quit();
		perror("remove()");
		exit(1);
	}

	return ret;
}

void Files_DeleteDirectory(char* dname) {
	if (nftw(dname, nftw_callback, 64, FTW_DEPTH | FTW_PHYS) == -1) {
		IOHandle_Quit();
		perror("nftw()");
		exit(1);
	}
}

void Files_DuplicateFile(char* fname, char* newName) {
	FILE* fhnd = fopen(fname, "rb");

	if (fhnd == NULL) {
		IOHandle_Quit();
		perror("fopen()");
		exit(1);
	}

	fseek(fhnd, 0, SEEK_END); 
	size_t fsize = ftell(fhnd);
	fseek(fhnd, 0, SEEK_SET); 
	char* file1 = (char*) malloc(fsize);

	char ch;
	for (int i = 0; (ch = getc(fhnd)) != EOF; ++i) {
		file1[i] = ch;
	}

	fclose(fhnd);

	fhnd = fopen(newName, "wb");
	if (fwrite(file1, fsize, 1, fhnd) != fsize) {
		IOHandle_Quit();
		perror("fwrite()");
		exit(1);
	}
}