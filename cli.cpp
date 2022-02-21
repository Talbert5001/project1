//C program to illustrate
//command line arguments
#include <iostream>
#include <stdlib.h>
#include <unistd.h>

using namespace std;
int main()
{
char input[100];

//char *argv[] = {"hello","citro","class",NULL};

//execvp("./ex2");
cout <<"****************************************** \n";
cout <<"Welcome to Cody and Talbert's Command Line \n";
cout <<"****************************************** \n";

cout << "Please Enter Command: \n";
cin.getline(input,100);
system(input);

return 0;

}
