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
#define MAXSTR 1000
#define MAXLIST 100
#define clear() printf("\33[H\033[J]")

#define PAGE 256;
#define FRAME 256;
#define PAGE_TABLE 128;
#define TABLE_SIZE 16;
#define BUFFER_SIZE 10;


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
	}else{ printf(" not adding");

		}


}

int processCmd(char** parsed){
    int numCmds = 6; //number of default commands
    int switchCmd = 0;
    char* cmds[numCmds]; //array for default commands
    cmds[0] = "help"; //associated strings are used to call commands
    cmds[1] = "cd";
    cmds[2] = "exit";
    cmds[3] = "ls";
    cmds[4] = "bot";
    cmds[5] = "add";
    for(int i = 0; i < numCmds; i++){
        if(strcmp(parsed[0], cmds[i]) == 0){
            switchCmd = i+1;
            break;
        }
    }
    switch(switchCmd){ //use cases so the user may trigger one command after another without restarting program
    case 1: //help command
        printf("HELP\n");
	printf("cd - change directory\n");
	printf("ls - get list of files in directory\n");
	printf("exit - leaves cmd\n");
	printf("bot - starts a small chat bot\n\n");
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
    case 6:
	printf("STARTING CALCULATOR\n");
	calculator("add");
	return 1;
    default:
        break;
    }
    return 0;

}

