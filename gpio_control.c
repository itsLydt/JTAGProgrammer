#include "gpio_control.h"

bool setPinValue(int pin_fd, bool value){
	int valueInt = value? 1 : 0;
	int len = snprintf(NULL, 0, "%d", valueInt);
	char* valueStr = malloc(len + 1);
	snprintf(valueStr, len + 1, "%d", valueInt);
	printf("Setting pin to %s\r\n", valueStr);
	int bWritten = write(pin_fd, valueStr, len);
	if (bWritten != len) {
		printf("Failed to write pin value\r\n");
		return false;
    }
	return true;
}

bool readPinValue(int pin_fd){
	//seek to beginning of file
	int offset = lseek(pin_fd, 0, SEEK_SET);
	if(offset == -1){
		printf("Failed to read pin value\r\n");
		return false;
	}
	char buffer[2];
	//read the IO
	int bytesRead = read(pin_fd, &buffer, 1);
	if(bytesRead == -1){
		printf("Failed to read pin value file\r\n");
		return false;
	}
	else {
		//printf("Pin value was: %c\r\n", buffer[0]);
		return buffer[0] == '1';
	}
}


int configurePin(int pinNo, bool direction) {
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
	
	//open direction file
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

	if(direction){
		//set pin to be an output pin

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
		return fd;
	}
	else {
		//set pin to be an input pin
		bWritten = write(fd, "in", 2);
		close(fd);

		if(bWritten != 2){
			printf("Failed to set pin direction.\r\n");
			free(pinStr);
			return -1;
		}

		//open pin for reading
		len = snprintf(NULL, 0, "/sys/class/gpio/gpio%s/value", pinStr);
		filePath = malloc(len + 1);
		snprintf(filePath, len + 1, "/sys/class/gpio/gpio%s/value", pinStr);
		fd = open(filePath, O_RDONLY);

		free(pinStr);
		free(filePath);

		if(fd == -1){
			printf("Failed to access pin\r\n");
			return -1;
		}
		return fd;
	}
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
