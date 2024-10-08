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
#include <semaphore.h>
#include <fcntl.h>
#include "extern/printC.h"
#include "utils.h"
#include "uiplusplus.h"

#define MAX 16
#define MAX_LINE_LENGTH 1000
#define MAX_LINES 1000
#define _XOPEN_SOURCE 700
#define SEM_NAME "/jaki_semafor"

sem_t* mutex;

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

    // TODO: Obraditi SVE greske Boze mi pomozi

    // Create path from requested file
    char file_path[256];
    strcpy(file_path, "db/");
    strcat(file_path, req_token);

    // Find requested file, file size, disassemble it and send it one by one
    size_t file_size;
    unsigned char* file_chunks = file_disassembler(file_path, &file_size);

    // File opening error
    if(file_chunks == NULL) {
        strcpy(message.mesg_text, "404");
        msgsnd(client_mq, &message, sizeof(message), 0);
        strcpy(message.mesg_text, "END");
        msgsnd(client_mq, &message, sizeof(message), 0);
        printf("[%s INVALID REQUEST %s ]\t File not found or reading has failed.\n\n", BRED, CRESET);
        return;
    }

    char str_file_size[32];
    sprintf(str_file_size, "%d", file_size);

    char name_size[32];
    strcpy(name_size, req_token);
    strcat(name_size, ":");
    strcat(name_size, str_file_size);
    strcpy(message.mesg_text, name_size);
    msgsnd(client_mq, &message, sizeof(message), 0);
    
    for (size_t i = 0; i < file_size; ++i) {
            // printf("[%d] SENDING: %02X \n", i, file_chunks[i]);
            char buffer[4];
            sprintf(buffer, "%02X", file_chunks[i]);
            strcpy(message.mesg_text, buffer);
            msgsnd(client_mq, &message, sizeof(message), 0);
        }

    strcpy(message.mesg_text, "END");
    msgsnd(client_mq, &message, sizeof(message), 0);
    printf("[ %sDONE%s ]\t\t { %s has been served. }\n\n", BGRN, CRESET, request_text);

    free(file_chunks);

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

        printf("[ %sINCOMING REQUEST%s ]\t { %s } 🚥\n", BGRN, CRESET, message.mesg_text);
        sem_wait(mutex);
        handle_request(message.mesg_text);
        sem_post(mutex);
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

    mutex = sem_open(SEM_NAME, O_CREAT, S_IRUSR | S_IWUSR, 1);

    if(mutex == SEM_FAILED) {
        printc("Fatal error: Failed to link a semaphore. Exiting...", BRED);
        exit(EXIT_FAILURE);
    } else {
        printc("Semaphore linked successfully.\n", BGRN);
    }

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

    sem_close(mutex);
    sem_unlink(SEM_NAME);

    return 0;
}
