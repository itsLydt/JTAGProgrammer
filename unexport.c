#include <fcntl.h> 
#include <unistd.h>
#include <stdio.h>

int main(){
	//clean up and unexport the pin
	int fd = open("/sys/class/gpio/unexport", O_WRONLY); 
	if(fd == -1){
		printf("Failed to access gpio unexport\r\n");
		return 1;
	}
	
	int bWritten = write(fd, "23", 2); //unexport pin 18
    bWritten += write(fd, "18", 2); //unexport pin 

	bWritten += write(fd, "24", 2); //unexport pin 


	if(bWritten != 6){
		printf("Failed to unexport GPIO pins.\r\n");
		return 1;
	}

	close(fd);
	return 0;
}
