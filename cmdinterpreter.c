/*
Kasey Cocking
CST-315-99
Project 1: Unix/Linux Improved Command Line Interpreter
Project 2: Pager - A Virtual Memory Manager
This project is aimed at becoming more familiar with the Linux OS and
understanding the inner-workings of the powershell. This project has 
been improved upon with additonal commands and an implemented virtual
memory manager.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <alloca.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>
//program requires all 11 packages to run

#define MAXSTR 1000 //max characters supported
#define MAXLIST 100 //max commands supported
#define clear() printf("\033[H\033[J") //for clearing terminal

#define BUFF_SIZE 10 //buffer size for reading a line from a file
#define ADDRESS_MASK 0xFFFF //for masking all of logAddress except the address
#define OFFSET_MASK 0xFF //for masking the offset
#define TLB_SIZE 16 //16 entries in the TLB
#define PAGE_TABLE_SIZE 128 //table size of 2^7
#define PAGE 256 //upon page fault, read in 256-byte page from backing store
#define FRAME_SIZE 256 //frame size

typedef enum{ //define a boolean type
    false, true
} bool;

int TLBEntries = 0; //current # of TLB entries
int hits = 0; //counter for TLB hits
int faults = 0; //counter for page faults
int currPage = 0; //current number of pages
int logAddress; //int to store logical address
int TLBpages[TLB_SIZE]; //array to hold page numbers in TLB
bool pagesRef[PAGE_TABLE_SIZE]; //array to hold reference bits for page numbers in TLB
int pageTableNumbers[PAGE_TABLE_SIZE]; //array to hold page numbers in page table
char currentAddress[BUFF_SIZE]; //array to store addresses
signed char fromBackingStore[PAGE]; //holds reads from backing store
signed char byte; //holds value of physical memory at frame number/offset
int physicalMemory[PAGE_TABLE_SIZE][FRAME_SIZE]; //physical memory array of 32,678 bytes

void terminal(); //starts terminal with header
int takeLine(char* str); //takes input to history
void printDir(); //prints directory
void execCmd(char** parsed); //method for executing system commands
void execPiped(char** parsed, char** parsedPipe); //method for executing piped system commands
void parseSpace(char* str, char** parsed); //method for parsing command characters
int parsePipe(char* str, char** strpiped); //method for finding pipe
int processString(char* str, char** parsed, char** parsedPipe); //method for parsing strings
int processCmd(char** parsed); //method for executing default commands
void help(); //prints helpful info
void hmmMonke(); //playful command
void page(int logAddress); //takes logical address and obtains physical address and byte stored at that address
int backingStore(int pageNum); //used to read from backing store
void TLBInsert(int pageNum, int frameNum); //insert page into TLB
void *runner(void *arg); //convert arg to string and runs it
void fromFile(char *fileName); //uses threads to pull from file
void vmm(char *arg); //main function to use as command to pull from file

FILE *addresses;
FILE *backing_store;

int main(){
    char inputString[MAXSTR], *parsedCmd[MAXLIST]; //initialize input variables
    char* parsedPiped[MAXLIST];
    int execFlag = 0;
    terminal(); //launch terminal header
    while(1){ //waiting for input
        printDir();
        if(takeLine(inputString)) //checks for input
            continue; //processes input
        execFlag = processString(inputString, parsedCmd, parsedPiped); //execFlagis 0 when no command is inputted
        if(execFlag == 1) //1 = input command to be executed using execCmd
            execCmd(parsedCmd);
        if(execFlag == 2) //2 = imput command to be executed using proccessCmd
            execPiped(parsedCmd, parsedPiped);
    }
    return 0;
}

void terminal(){
    clear(); //cleans up display
    printf("\n***********************************");
    printf("\nC Command Line Interpreter in Linux"); //heading for program
    printf("\n***********************************");
    char* username = getenv("USER"); //chooses user from environment
    printf("\n\nUSER is: @%s", username); //display user
    printf("\n");
    sleep(3); //wait 3 seconds for header to be read
    clear();
}
int takeLine(char* str){
    char* buf; //buffer variable
    buf = readline("# "); //input after the # is used to fill the buffer
    if(strlen(buf) != 0){ //0 means nothing is entered
        add_history(buf); //input is saved to history
        strcpy(str, buf); //input string is copied
        return 0;
    }
    else{
        return 1;
    }
}
void printDir(){
    char cwd[1024]; //create directory pointer size
    getcwd(cwd, sizeof(cwd)); //finds directory from pointer size range
    printf("\nDir:%s", cwd);
}
void execCmd(char** parsed){
    pid_t pid = fork(); //fork a child
    if(pid == -1){ //-1 suggests the fork can't complete
        printf("\nFailed forking child.");
        return;
    }
    else if(pid == 0){ //0 suggests the fork completed but the command can't be executed
        if(execvp(parsed[0], parsed) < 0){ //execvp searches for available arguments in the parsed lib
            printf("\nCould not execute command.");
        }
        exit(0);
    }
    else{
        wait(NULL);
        return;
    }
}
void execPiped(char** parsed, char** parsedpipe){
    int pipefd[2]; //2 slot array for 0(read) and 1(write)
    pid_t p1, p2;
    if(pipe(pipefd) < 0){
        printf("\nPipe could not be initialized.");
        return;
    }
    p1 = fork(); //parent executing
    if(p1 < 0){ //must be 0 or 1
        printf("\nCould not fork.");
        return;
    }
    if(p1 == 0){ //child 1 executes
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO); //1 means write
        close(pipefd[1]);
        if(execvp(parsed[0], parsed) < 0){
            printf("\nCould not execute command 1.");
            exit(0);
        }
    }
    else{
        p2 = fork(); //parent executing
        if(p2 < 0){
            printf("\nCould not fork.");
            return;
        }
    }
    if(p2 == 0){ //child 2 executes
        close(pipefd[1]);
        dup2(pipefd[0], STDIN_FILENO); //0 means read
        close(pipefd[0]);
        if(execvp(parsedpipe[0], parsedpipe) < 0){
            printf("\nCould not execute command 2.");
            exit(0);
        }
    }
    else{ //parent waits for the 2 child processes while it executes
        wait(NULL);
        wait(NULL);
    }
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
int parsePipe(char* str, char** strpiped){
    for(int i = 0; i < 2; i++){
        strpiped[i] = strsep(&str, "|"); //searching for pipe
        if(strpiped[i] == NULL){
            break;
        }
    }
    if(strpiped[1] == NULL){
        return 0; //returns 0 for no pipe
    }
    else{
        return 1;
    }
}
int processString(char* str, char** parsed, char** parsedpipe){
    char* strpiped[2];
    int piped = 0;
    piped = parsePipe(str, strpiped);
    if(piped){
        parseSpace(strpiped[0], parsed);
        parseSpace(strpiped[1], parsedpipe);
    }
    else{
        parseSpace(str, parsed);
    }
    if(processCmd(parsed)){
        return 0;
    }
    else{
        return 1 + piped;
    }
}
int processCmd(char** parsed){
    int numCmds = 5; //number of default commands
    int switchCmd = 0;
    char* cmds[numCmds]; //array for default commands
    char* username;
    cmds[0] = "help"; //associated strings are used to call commands
    cmds[1] = "cd";
    cmds[2] = "quit";
    cmds[3] = "ooga_booga";
    cmds[4] = "page";
    for(int i = 0; i < numCmds; i++){
        if(strcmp(parsed[0], cmds[i]) == 0){
            switchCmd = i+1;
            break;
        }
    }
    switch(switchCmd){ //use cases so the user may trigger one command after another without restarting program
    case 1: //help command
        help();
        return 1;
    case 2: //cd command
        chdir(parsed[1]);
        return 1;
    case 3: //quit command
        printf("\nClosing\n\n");
        exit(0);
    case 4: //fun gimmick command
    	hmmMonke();
        exit(0);
    case 5: //run virtual memory manager
        vmm(parsed[1]);
        return 1;
    default:
        break;
    }
    return 0;

}
void help(){ //this command simply prints information for the user
    printf("\nSupported Commands: ");
    printf("\n./*filenamehere*: executes file");
    printf("\ncd: changes directory");
    printf("\nclear: clears terminal");
    printf("\nhelp: opens help menu");
    printf("\nls: shows existing files in directory");
    printf("\nquit: terminates program\n");
    printf("ooga_booga: you know you want to");
    return;
}
void hmmMonke(){
	printf("\nThis ain't no place to monkey around...\n"); //display warning for inappropriate cmd
    sleep(2); //wait before next print line
    printf("\nForcing Shutdown\n"); //warn of shutdown
}
void page(int logAddress){
    int frameNum = -1; //sentinal value
    int pageNum = ((logAddress & ADDRESS_MASK) >> 8); //mask leftmost 16 bits, then shift right 8 bits to extract page number
    int offset = (logAddress & OFFSET_MASK); //rightmost bits
    for(int i = 0; i < TLB_SIZE; i++){
        if(TLBpages[i] == pageNum){
            frameNum = i; //extract frame number
            hits++; //increase hits
        }
    }
    if(frameNum == -1){
        for(int i = 0; i < currPage;){
            if(pageTableNumbers[i] == pageNum){
                frameNum = i; //if page number found extract
                pagesRef[i] = true;
            }
        }
        if(frameNum == -1){
            int count = backingStore(pageNum);
            faults++; //page not found, increase faults
            frameNum = count;
        }
    }
    TLBInsert(pageNum, frameNum); //insert page number and frame number into TLB
    byte = physicalMemory[frameNum][offset]; //assign the value of the signed char to byte
    printf("Virtual adress: %d Physical address: %d Value: %d\n", logAddress, (frameNum << 8) | offset, byte);
}
int backingStore(int pageNum){
    int counter = 0;
    if(fseek(backing_store, pageNum * PAGE, SEEK_SET) != 0){
        fprintf(stderr, "Error seeking in backing store\n");
    }
    if(fread(fromBackingStore, sizeof(signed char), PAGE, backing_store) == 0){
        fprintf(stderr, "Error reading from backing store\n");
    }
    bool search = true;
    while(search){
        if(currPage == PAGE_TABLE_SIZE){
            currPage = 0;
        }
        if(pagesRef[currPage] == false){
            pageTableNumbers[currPage] = pageNum;
            search = false;
        }
        else{
            pagesRef[currPage] = false;
        }
        currPage++;
    }
    for(int i = 0; i < PAGE; i++){
        physicalMemory[currPage - 1][i] = fromBackingStore[i]; 
    }
    counter = currPage - 1;
    return counter;
}
void TLBInsert(int pageNum, int frameNum){
    int i;
    for(i = 0; i < TLBEntries; i++){
        if(TLBpages[i] == pageNum){
            break;
        }
    }
    if(i == TLBEntries){
        if(TLBEntries < TLB_SIZE){
            TLBpages[TLBEntries] = pageNum;
        }
        else{
            for(i = 0; i < TLB_SIZE - 1; i++){
                TLBpages[i] = TLBpages[i + 1];
            }
            TLBpages[TLBEntries - 1] = pageNum;
        }
    }
    else{
        for(i = i; i < TLBEntries - 1; i++){
            TLBpages[i] = TLBpages[i + 1];
        }
        if(TLBEntries < TLB_SIZE){
            TLBpages[TLBEntries] = pageNum;
        }
        else{
            TLBpages[TLBEntries - 1] = pageNum;
        }
    }
    if(TLBEntries < TLB_SIZE){
        TLBEntries++;
    }
}
void *runner(void *arg){
    system((char *)arg);
    return NULL;
}
void fromFile(char *fileName){
    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    fp = fopen(fileName, "r");
    if(fp == NULL){
        exit(EXIT_FAILURE);
    }
    while((read = getline(&line, &len, fp)) != -1){
        char *token = strtok(line, ";");
        pthread_t tid[10];
        int i = 0;
        while(token != NULL){
            pthread_create(&tid[i++], NULL, runner, token);
            token = strtok(NULL, ";");
        }
        for(int k = 0; k < i; k++){
            pthread_join(tid[k], NULL);
        }
    }
    fclose(fp);
    if(line){
        free(line);
    }
}
void vmm(char* arg){
    if(arg){ //if given a command to parse
        fromFile(arg); //pull from file parsed
    }
}
