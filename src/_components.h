#pragma once

// standard headers
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>

// libraries
#include <ncurses.h>

// macros
#define ctrl(x)    ((x) & 0x1f)