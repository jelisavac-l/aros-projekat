#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

#include "extern/printC.h"

#include "uiplusplus.h"

#define MAX 10

// Procedura za fork, jaci od drzave

// Struktura jaca od drzave
struct mesg_buffer
{
    long mesg_type;
    char mesg_text[100];
} message;

void respond(char *sample)
{
    key_t key;
    int msgid;

    // Da se razumemo medju procesima
    key = ftok("progfile", 65);

    msgid = msgget(key, 0666 | IPC_CREAT);
    message.mesg_type = 1;

    if (sample != NULL)
    {
        strcpy(message.mesg_text, sample);
    }
    else
    {
        printf("Argument list is empty, write data: ");
        fgets(message.mesg_text, MAX, stdin);
    }
    msgsnd(msgid, &message, sizeof(message), 0);
    printf("Data sent is : %s%s%s \n", GRN, message.mesg_text, CRESET);
}

int main(int argc, char **argv)
{
    //print_splash_screen(SRV);

    respond(argv[1]);
    return 0;
}
