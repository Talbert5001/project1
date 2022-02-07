#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>

int main (int argc, char *argv[])
{
	
	/fork();
	
	printf("PID of ex1.c = %d\n", getpid());
	char *args[] = {"Hello","Test", "Citro", NULL};
	
	
	execv("./ex2",args);
	printf("Back to ex1.c");

	

	return 0;

}
