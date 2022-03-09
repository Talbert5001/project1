#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>


#define MAXSTR 1000
#define MAXLIST 100
#define clear() printf("\33[H\033[J]")

#define PAGE 7
#define PAGE_TABLE 4
#define MEMORY 256



void terminal();
int processCmd(char** parsed);
int stringer(char* str, char** parsed);
int getLine(char* str);
int chatbot(); //starts a chat bot

int main(){
	char inputString[MAXSTR], *parsedCmd[MAXLIST];
	char* parsedPiped[MAXLIST];

	terminal();

	while(1){
		char cwd[1024];
		getcwd(cwd, sizeof(cwd));
		printf("\nDir:%s",cwd);
		if(getLine(inputString))
			continue;
		stringer(inputString,parsedCmd);
	}
	return 0;
}

int getLine(char* str){
    char* buf; 
    buf = readline("# ");
    if(strlen(buf) != 0){ 
        add_history(buf); 
        strcpy(str, buf); 
        return 0;
    }
    else{
        return 1;
    }
}

void terminal(){
	clear();
	printf("\n ********************************\n");
	printf("Welcome to the Talbert and Chris Terminal");
	char* username = getenv("USER");
	printf("\n\nUSER is: @%s",username);
	sleep(2);	
	clear(); 

}
void parseSpace(char* str, char** parsed){
    for(int i = 0; i < MAXLIST; i++){
        parsed[i] = strsep(&str, " ");
        if(parsed[i] == NULL){
            break;
        }
        if(strlen(parsed[i]) == 0){
            i--;
        }
    }
}
int stringer(char* str, char** parsed){
	parseSpace(str,parsed);

	if(processCmd(parsed)){
		return 0;
	}
}

int ls(void){
	struct dirent *de;
	DIR *dr =opendir(".");

	if(dr == NULL){
		printf("could not open current directory");
		return 0;
	}
	
	while((de = readdir(dr)) != NULL)
		printf("%s ",de->d_name);

	closedir(dr);
	return 0;
}

int chatbot(){
	char name;
	printf("Hey! What is your name?\n");
	scanf("%c", &name);
	printf("Oh... I don't talk to people named %c\n\n",name);
	printf("BYE!");
	return 0;
		
}

int calculator (char* method){

	if(method = "add"){
		int a,b,sum;
		printf("Enter First Number:");
		scanf("%d", &a);
		printf("Enter Second Number:");
		scanf("%d", &b);
		sum = a+b;
		printf("%d + %d = %d", a,b,sum);
	}
	if(method = "subtract"){
		int a,b,sum;

		printf("Enter First Number:");
		scanf("%d",&a);
		printf("Enter Second Number:");
		scanf("%d",&b);
		sum = a-b;
		printf("%d - %d = %d",a,b,sum);
	}
}

int page(char *filename){
	char pages[PAGE_TABLE][10];
	char *pageTable[PAGE_TABLE];
	int physicalMemory[MEMORY];
	srand(time(NULL));
	int i,j = 0;

	FILE *fp = fopen(filename,"r");

	if (fp == NULL) {

		perror("Unable to open file!");
		exit (1);
	}	
        while(fgets(pages[i],10,fp) != NULL) {
               	pages[i][strlen(pages[i]) -1] = '\0';
		i++;
       	}
	j = i;

	for(i = 0; i < j; ++i){
		printf("\n index[%d] frame[%d]  String:%s \n" ,i,rand() %(PAGE + 1) + 0, pages[i]);	
	}
	printf("\n");
	fclose(fp);
}

int processCmd(char** parsed){
    int numCmds = 8; //number of default commands
    int switchCmd = 0;
    char* cmds[numCmds]; 
    cmds[0] = "help"; //associated strings are used to call commands
    cmds[1] = "cd";
    cmds[2] = "exit";
    cmds[3] = "ls";
    cmds[4] = "bot";
    cmds[5] = "add"; 
    cmds[6] = "page";
    cmds[7] = "subtract";
    for(int i = 0; i < numCmds; i++){
        if(strcmp(parsed[0], cmds[i]) == 0){
            switchCmd = i+1;
            break;
        }
    }
    switch(switchCmd){ //use cases
    case 1: //help command
        printf("HELP\n");
	printf("cd - change directory\n");
	printf("ls - get list of files in directory\n");
	printf("exit - leaves cmd\n");
	printf("bot - starts a small chat bot\n");
	printf("add - adds two numbers\n");
	printf("page(filename) - pages a  your file\n");
	printf("subtract - subtracts two numbers \n");
        return 1;
    case 2: //cd command
        chdir(parsed[1]);
        return 1;
    case 3: //exit command
        printf("\nClosing\n\n");
        exit(0);
    case 4: //list files
	ls();
        return 1;
    case 5://chat bot command
	printf("\nSTRATING CHAT BOT\n");
	chatbot();
	return 1;
    case 6://starts calculator
	printf("STARTING CALCULATOR\n");
	calculator("add");
	return 1; 
    case 7://page
	printf("Creating Virtual Memory, Page Table");
	page(parsed[1]);
	return 1;
    case 8:
	printf("\n STARING CALCULATOR\n");
	calculator("subtract");
	return 1;
    default:
        break;
    }

    return 0;
}

