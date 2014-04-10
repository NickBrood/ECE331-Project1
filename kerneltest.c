#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

/*
A simple test to see if the kernel driver will 
allow insert and remove without crashing kernel.

Appropriate calls to open() and close() are below. 
*/

int main(int argc, char *argv[]){
        int fd; 

        //Look in special devices for module to test open
        fd = open("/dev/lcd", O_RDWR);

        //Error Checking
        if(fd < 0) {
                printf("Error: Initialization failed\n");
        }
	else{
		printf("Woo! File opened! fd: %d\n", fd);
	}

        //Test close
        close(fd);

        return 0;
}
