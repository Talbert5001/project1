#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>

int main (int argc, char *argv[])
{
	
	//fork();
	
	printf("PID of ex1.c = %d\n", getpid());
	//char *args[] = {"Hello","Test", "Citro", NULL};
	
	
	//execv("./ex2",args);
	//printf("Back to ex1.c");

	if (argc < 1);
		return 0;

	int i;
	int strsize = 0;
	for (i =1; i <argc; i++){
		strsize += strlen(argv[i]);
		if(argc > i+1)
		strsize++;
	}

	printf("strsize: %d\n",strsize);

	char *cmdstring;

	cmdstring[0] = '\0';
	
	for (i=1; i<argc; i++){
		strcat(cmdstring, argv[i]);
		if(argc > i+1)
			strcat(cmdstring," ");
	}
	printf("cmdstring: %s\n", cmdstring);

	return 0;

}
