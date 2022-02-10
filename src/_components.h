#pragma once

// macros
#define _XOPEN_SOURCE 501
#define _GNU_SOURCE

// standard headers
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <limits.h>
#include <ftw.h>

// libraries
#include <ncurses.h>

// macros
#define ctrl(x)    ((x) & 0x1f)
#define _GNU_SOURCE /* vscode why */