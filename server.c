#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

#include "extern/printC.h"

#include "uiplusplus.h"

#define MAX 16
#define MAX_LINE_LENGTH 1000
#define MAX_LINES 1000

struct mesg_buffer
{
    long mesg_type;
    char mesg_text[100];
} message;

int respond(char *path)
{

    char line[MAX_LINE_LENGTH];
    char lines[MAX_LINES][MAX_LINE_LENGTH];
    int line_counter = 0;

    FILE *fileptr;
    fileptr = fopen(path, "r");
    if(fileptr == NULL) {
        printc("Error: invalid path or nonexistant file!\n", RED);
        return 0;
    }

    while (fgets(line, MAX_LINE_LENGTH, fileptr))
    {
        if(line_counter >= MAX_LINES) {
            printc("Error: Exceeded maximum number of lines!\n", RED);
            break;
        }
        strcpy(lines[line_counter], line);
        line_counter++;
    }
    
    fclose(fileptr);

    // Following lines must not be executed before file-error checks!
    // Otherwise, the error will occur, but message queue won't be closed!
    // Found out the hard way :(
        
    key_t key;
    int msgid;
    key = ftok("progfile", 65);
    msgid = msgget(key, 0666 | IPC_CREAT);
    message.mesg_type = 1;

    printc("Sending following lines to message queue:\n\n", YEL);
    for (int i = 0; i < line_counter; i++) {
        printf("ln%d:\t%s", i, lines[i]);
        strcpy(message.mesg_text, lines[i]);
        msgsnd(msgid, &message, sizeof(message), 0);
        printc("OK!\n", GRN);
    }
    printf("\n");

    // After everything has finished, send an 'end of transmission' signal.
    strcpy(message.mesg_text, "END");
    msgsnd(msgid, &message, sizeof(message), 0);

    return line_counter;
    // if (sample != NULL)
    // {
    //     strcpy(message.mesg_text, sample);
    // }
    // else
    // {
    //     printf("Argument list is empty, write data: ");
    //     fgets(message.mesg_text, MAX, stdin);
    // }
    // msgsnd(msgid, &message, sizeof(message), 0);
    // printf("Data sent is : %s%s%s \n", GRN, message.mesg_text, CRESET);

    
}

int main(int argc, char **argv)
{
    //print_splash_screen(SRV);
    if(!strcmp(argv[1], "--h")){
        print_help(SRV);
        return 0;
    }
    if(respond(argv[1])) {
        printc("Success!\n", GRN);
    }
    else{
        printc("Transmission failed!\n", RED);
    }
    return 0;
}
