#pragma once
#include "structures.h"

struct Array getFiles(char* path, bool showHidden);

void Files_DeleteDirectory(char* dname);
void Files_DuplicateFile(char* fname, char* newName);