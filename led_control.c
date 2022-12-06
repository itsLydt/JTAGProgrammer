#include <fcntl.h> 
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "led_control.h"

bool setPinValue(int pin_fd, bool value){
	int len = snprintf(NULL, 0, "%d", value);
	char* valueStr = malloc(len + 1);
	snprintf(valueStr, len + 1, "%d", value);
	printf("Setting pin to %s\r\n", valueStr);
	int bWritten = write(pin_fd, valueStr, len);
	if (bWritten != len) {
		printf("Failed to write pin value\r\n");
		return false;
    }
	return true;
}

int configurePin(int pinNo) {
	//TODO: check if already exported

	//export GPIO pin
	int fd = open("/sys/class/gpio/export", O_WRONLY); 
	if(fd == -1){
		printf("Failed to access gpio export file\r\n");
		return -1;
	}
	int len = snprintf( NULL, 0, "%d", pinNo); //determine length of resulting string
	char* pinStr = malloc(len + 1); // allocate len + 1 (for null termination)
	snprintf(pinStr, len + 1, "%d", pinNo);	//int to string
	printf("Pin number is %d, %s\r\n", pinNo, pinStr);
	int bWritten = write(fd, pinStr, len); //write pin to export
	close(fd);

	if(bWritten != len){
		printf("Failed to export GPIO pin.\r\n");
		free(pinStr);
		return -1;
	}
	
	//set pin to be an output pin
	len = snprintf(NULL, 0, "/sys/class/gpio/gpio%s/direction", pinStr);
	char* filePath = malloc(len + 1);
	snprintf(filePath, len + 1, "/sys/class/gpio/gpio%s/direction", pinStr);

	fd = open(filePath, O_WRONLY);
	free(filePath);

	if(fd == -1){
		printf("Failed to access gpio pin direction file\r\n");
		free(pinStr);
		return -1;
	}

	bWritten = write(fd, "out", 3);
	close(fd);

	if(bWritten != 3){
		printf("Failed to set pin direction.\r\n");
		free(pinStr);
		return -1;
	}

	//open pin for writing

	len = snprintf(NULL, 0, "/sys/class/gpio/gpio%s/value", pinStr);
	filePath = malloc(len + 1);
	snprintf(filePath, len + 1, "/sys/class/gpio/gpio%s/value", pinStr);
    fd = open(filePath, O_WRONLY);

	free(pinStr);
	free(filePath);

	if(fd == -1){
		printf("Failed to access pin\r\n");
		return -1;
	}
	write(fd, "1", 1);
	return fd;
}

int releasePin(int pinNo){
	//clean up and unexport the pin
	int fd = open("/sys/class/gpio/unexport", O_WRONLY); 
	if(fd == -1){
		printf("Failed to access gpio unexport\r\n");
		return -1;
	}
	
	int len = snprintf( NULL, 0, "%d", pinNo); //determine length of resulting string
	char* pinStr = malloc(len + 1); // allocate len + 1 (for null termination)
	snprintf(pinStr, len + 1, "%d", pinNo);	//int to string
	int bWritten = write(fd, pinStr, len); //write pin to unexport

	if(bWritten != len){
		printf("Failed to unexport GPIO pin.\r\n");
		return -1;
	}

    close(fd);
    return 0;
}
/*
int main() {
	
	//export GPIO pin 18
	int fd = open("/sys/class/gpio/export", O_WRONLY); 
	if(fd == -1){
		printf("Failed to access gpio export file\r\n");
		return 1;
	}
	
	int bWritten = write(fd, "18", 2); //export pin 18

	if(bWritten != 2){
		printf("Failed to export GPIO pin.\r\n");
		return 1;
	}

	close(fd);

	//set pin to be an output pin
	fd = open("/sys/class/gpio/gpio18/direction", O_WRONLY);
	if(fd == -1){
		printf("Failed to access gpio pin direction file\r\n");
		return 1;
	}
	bWritten = write(fd, "out", 3);
	if(bWritten != 3){
		printf("Failed to set pin direction.\r\n");
		return 1;
	}
	close(fd);

	//open pin for writing
    	fd = open("/sys/class/gpio/gpio18/value", O_WRONLY);
	if(fd == -1){
		printf("Failed to access pin\r\n");
		return 1;
	}


	for(int i = 0; i < 10; i++){
        	if (write(fd, "1", 1) != 1) {
            		printf("Failed to write pin value\r\n");
        	}
        	sleep(3);

        	if (write(fd, "0", 1) != 1) {
           		printf("Failed to write pin value\r\n");
        	}
        	sleep(3);
	}

	close(fd);
	
	//clean up and unexport the pin
	fd = open("/sys/class/gpio/unexport", O_WRONLY); 
	if(fd == -1){
		printf("Failed to access gpio unexport\r\n");
		return 1;
	}
	
	bWritten = write(fd, "18", 2); //unexport pin 18

	if(bWritten != 2){
		printf("Failed to unexport GPIO pin.\r\n");
		return 1;
	}

    close(fd);
    return 0;
}
*/
