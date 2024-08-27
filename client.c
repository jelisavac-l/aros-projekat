#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdbool.h>
#include <fcntl.h>
#include <semaphore.h>
#include <unistd.h>
#include "extern/printC.h"
#include "uiplusplus.h"
#include "utils.h"
#include <sys/stat.h>
#include <linux/stat.h>

#define SERVER_PATHNAME "/server"
#define SERVER_PROJ_ID 69

#define SEM_NAME "/jaki_semafor"

#define CLIENT_PATHNAME "/home/luka/"
#define CLIENT_PROJ_ID 'C'

sem_t* mutex;

struct mesg_buffer
{
    long mesg_type;
    char mesg_text[32];
} message;

unsigned char hex_to_byte(char* hex) {

    int value = 0;
    if (hex[0] >= '0' && hex[0] <= '9') {
        value += (hex[0] - '0') * 16;
    } else if (hex[0] >= 'A' && hex[0] <= 'F') {
        value += (hex[0] - 'A' + 10) * 16;
    } else if (hex[0] >= 'a' && hex[0] <= 'f') {
        value += (hex[0] - 'a' + 10) * 16;
    }

    if (hex[1] >= '0' && hex[1] <= '9') {
        value += (hex[1] - '0');
    } else if (hex[1] >= 'A' && hex[1] <= 'F') {
        value += (hex[1] - 'A' + 10);
    } else if (hex[1] >= 'a' && hex[1] <= 'f') {
        value += (hex[1] - 'a' + 10);
    }

    return (unsigned char) value;
}

void request()
{
    char requested_file[32];
    printf("%sEnter a name of a file you wish to get from the server:%s ", YEL, CRESET);
    fscanf(stdin, "%s", requested_file);
    printf("Requested file: %s\n", requested_file);
    printc("OK. Creating client message queue...\n",GRN);

    // Creating a client MQ

    key_t client_key;
    int client_msgid;
    client_key = ftok(CLIENT_PATHNAME, CLIENT_PROJ_ID);

    client_msgid = msgget(client_key, 0666 | IPC_EXCL | IPC_CREAT);
    if(client_msgid == -1) 
    {
        printf("%sFatal Error: unique client MQ creation failed!\n%s", BRED, CRESET);  // AJAOOOOO VISE
        exit(EXIT_FAILURE);
    }
    else printf("Created a Client Message Queue with id: %s%d%s!\n",BGRN,client_msgid,CRESET);
    
    // Sending request

    key_t server_key = ftok(SERVER_PATHNAME, SERVER_PROJ_ID);
    int server_msgid = msgget(server_key, 0666 | IPC_CREAT);    // Mozda i ne treba IPC_CREAT
    message.mesg_type = 1;

    char request_text[32];  // proveriti duzinu fajla...
    sprintf(request_text, "%d", client_msgid);
    strcat(request_text, ":");
    strcat(request_text, requested_file);

    strcpy(message.mesg_text, request_text);
    
    printf("In queue...\n");
    sem_wait(mutex);
    printf("Sending request...\n");
    msgsnd(server_msgid, &message, sizeof(message), 0);
    sem_post(mutex);
    printc("Request sent!\n", GRN);

    // Receiving a response

    bool caught_name = false;
    size_t file_size = 0;
    size_t byte_counter = 0;
    unsigned char* file_chunks;
    while (1)
    {

        // Hvala onome ko je napravio man
        if (msgrcv(client_msgid, &message, sizeof(message), 1, 0) < 0)
            break;

        if (strcmp(message.mesg_text, "END") == 0)
        {
            printc("\nEnd of transmission.\n", YEL);
            file_assembler(requested_file, file_chunks, file_size);
            byte_counter = 0;
            break;
        }
        if (!caught_name)
        {
            if(strcmp(message.mesg_text, "404") == 0)
            {
                printc("Error 404: File not found.\n", BRED);
                printf("Closing a message queue with id: %d... ", client_msgid);
                msgctl(client_msgid, IPC_RMID, NULL);
                printc("Queue closed.\n", GRN);
                return;
            }
            char *size;
            char *name;
            char *delimeter = ":";
            name = strtok(message.mesg_text, delimeter);
            size = strtok(NULL, delimeter);
            file_size = atoi(size);
            printf("Receiving a file: %s%s%s\n", YEL, name, COLOR_RESET);
            printf("File size: %s%s bytes%s\n", YEL, size, COLOR_RESET);
            file_chunks = (unsigned char*)malloc(file_size);
            caught_name = true;
            continue;
        }
        // printf("%s\n", message.mesg_text);
        file_chunks[byte_counter] = hex_to_byte(message.mesg_text);
        byte_counter++;

    }

    free(file_chunks);  // Ovo da se zaobidje ako ne uspe prenos... NE OSLOBADJATI NULL POINTER!
    printf("Closing a message queue with id: %d... ", client_msgid);
    msgctl(client_msgid, IPC_RMID, NULL);
    printc("Queue closed.\n", GRN);

}


int main(int argc, char **argv)
{

    mutex = sem_open(SEM_NAME, O_EXCL, S_IRUSR | S_IWUSR, 1);   // Ajao elegantno

    if(mutex == SEM_FAILED) {
        printc("Fatal error: Failed to link a semaphore. Exiting...", BRED);
        exit(EXIT_FAILURE);
    } else {
        printc("Semaphore linked successfully.\n", BGRN);
    }

    if (argv[1] == NULL)
        return 1;

    else if (!strcmp(argv[1], "--h"))
    {
        print_help(CLI);
        printf(COLOR_RESET);
        return 0;
    }

    else if (!strcmp(argv[1], "-r"))
    {
        request();
    }
    printf(COLOR_RESET);
    sem_close(mutex);

    return 0;
}