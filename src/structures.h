#pragma once

enum FileType {
	FileType_Null   = 0,
	FileType_Folder = 1,
	FileType_File   = 2
};

struct File {
	char*         name;
	enum FileType type;
	size_t        size;
};

struct Array {
	size_t size;
	void*  data;
};

struct Alert {
	const char* text;
	uint16_t    time;
};