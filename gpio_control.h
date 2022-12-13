#pragma once

#include <fcntl.h> 
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int configurePin(int pinNo, bool direction);
int releasePin(int pinNo);
bool setPinValue(int pin_fd, bool value);
bool readPinValue(int pin_fd);
