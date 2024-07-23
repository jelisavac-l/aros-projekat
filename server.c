#define _XOPEN_SOURCE 700 // For new C standards (signals don't work)

#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include "extern/printC.h"

#include "uiplusplus.h"

#define MAX 16
#define MAX_LINE_LENGTH 1000
#define MAX_LINES 1000
#define _XOPEN_SOURCE 700

typedef struct mesg_buffer
{
    long mesg_type;
    char mesg_text[32];
} MESG_BUFFER;

MESG_BUFFER message;
bool listen_continue = true;

void catch_signal(int signo)
{
    printf("\nCaught signal: %d\n", signo);
    listen_continue = false;
}

void handle_request(char* request_text)
{
    // Separate the MQID from text
    char *msq_token;
    char *req_token;
    char *delimeter = ":";
    msq_token = strtok(request_text, delimeter);
    req_token = strtok(NULL, delimeter);
    printf("[ %sHANDLINIG REQUEST%s ]\t { TO: %s, FILE: %s }\n", BGRN, CRESET, msq_token, req_token);


    // Answer the request
    int client_mq = atoi(msq_token);
    MESG_BUFFER message;
    message.mesg_type = 1;
    strcpy(message.mesg_text, req_token);
    msgsnd(client_mq, &message, sizeof(message), 0);
    strcpy(message.mesg_text, "END");
    msgsnd(client_mq, &message, sizeof(message), 0);
    printf("[ %sDONE%s ]\t { %s has been served. }\n", BGRN, CRESET, request_text);

}

void listen()
{

    struct sigaction sa;
    sa.sa_handler = &catch_signal;
    sigaction(SIGINT, &sa, NULL);   // SIGINT is Ctrl+C

    key_t server_key;
    int server_msgid;
    server_key = ftok("/server", 69);
    server_msgid = msgget(server_key, 0666 | IPC_CREAT);
    printf("Listening for requests on MessageQueue %s%d%s...\n", GRN, server_msgid, COLOR_RESET);
    printf("You can %ssafely%s stop the server by pressing Ctrl+C\n\n", BHGRN, COLOR_RESET);

    while (listen_continue)
    {
        if (msgrcv(server_msgid, &message, sizeof(message), 0, IPC_NOWAIT) < 0)
            continue;

        printf("[ %sINCOMING REQUEST%s ]\t { %s }\n", BGRN, CRESET, message.mesg_text);
        handle_request(message.mesg_text);
    }
    msgctl(server_msgid, IPC_RMID, NULL);
    printc("Server queue closed safely.\n", GRN);
}

int respond(char *path, char *target)
{

    char line[MAX_LINE_LENGTH];
    char lines[MAX_LINES][MAX_LINE_LENGTH];
    int line_counter = 0;

    FILE *fileptr;
    fileptr = fopen(path, "r");
    if (fileptr == NULL)
    {
        printc("Error: invalid path or nonexistent file!\n", RED);
        return 0;
    }

    while (fgets(line, MAX_LINE_LENGTH, fileptr))
    {
        if (line_counter >= MAX_LINES)
        {
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

    // First line is a name of a target file
    strcpy(message.mesg_text, target);
    msgsnd(msgid, &message, sizeof(message), 0);

    for (int i = 0; i < line_counter; i++)
    {
        printf("[%d]\t%s", i, lines[i]);
        strcpy(message.mesg_text, lines[i]);
        msgsnd(msgid, &message, sizeof(message), 0);
    }
    printc("\nOK!\n", GRN);
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

char *get_file_name(char *path) // TODO: To be tested
{
    int len = strlen(path);
    int flag = 0;

    for (int i = len - 1; i > 0; i--)
    {
        if (path[i] == '/')
        {
            flag = 1;
            path = path + i + 1;
            break;
        }
    }
    return path;
}

int main(int argc, char **argv)
{

    // Drzi vodu (segfault)
    if (argv[1] == NULL)
    {
        printc("Error: no path provided! This could have resulted in Segmentation fault!\n", RED);
        printc("Tip: read instructions by executing ./server --h\n", YEL);
        printf(COLOR_RESET);
        return -1;
    }

    if (!strcmp(argv[1], "--h"))
    {
        print_help(SRV);
        return 0;
    }
    else if (!strcmp(argv[1], "-l"))
    {
        listen();
        return 0;
    }
    char *target = get_file_name(argv[1]);
    printf("Requested file for transmission: %s%s%s\n", YEL, target, COLOR_RESET);
    if (respond(argv[1], target))
    {
        printc("Success!\n", GRN);
    }
    else
    {
        printc("Transmission failed!\n", RED);
    }

    printf(COLOR_RESET); // So the cursor doesn't stay colored...
    return 0;
}
