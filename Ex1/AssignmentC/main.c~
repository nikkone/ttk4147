#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

void allocate(int value) {
	int *ptr = NULL;
	ptr = malloc(1024*1024*sizeof(char));
    if(ptr == NULL){
        perror(NULL);
        exit(0);
    }
	*ptr = value;
	printf("test of allocated memory: %i\n",value);
}

int main(int argc, char *args[]){
	while(1){
		allocate(4);
	}
}
