#pragma once

#include <dirent.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <glob.h>

bool dirExists(char* dirName);

int getFileList(char* pattern, glob_t* pglob);