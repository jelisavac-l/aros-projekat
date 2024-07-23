#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdbool.h>
#include "extern/printC.h"

#include "uiplusplus.h"

#define SERVER_PATHNAME "/server"
#define SERVER_PROJ_ID 69

/*
    OSIGURATI NA NEKI NACIN DA SE KREIRAJU JEDINSTVENE VREDNOSTI ZA SVAKOG KLIJENTA
*/

#define CLIENT_PATHNAME "/home/luka/"
#define CLIENT_PROJ_ID 'C'

struct mesg_buffer
{
    long mesg_type;
    char mesg_text[32];
} message;

void request() // TODO: don't create a new message queue if server is down
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

    // ZNACI MRTVI IPC_CREAT ZALI BOZE 3 SATA DEBAGOVANJA
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

    msgsnd(server_msgid, &message, sizeof(message), 0);
    printc("Request sent!\n", GRN);

    // TODO: OVO IMPLEMENTIRATI STO PRE
    // Receiving a response

    bool caught_name = false;
    while (1)
    {

        // Hvala onome ko je napravio man
        if (msgrcv(client_msgid, &message, sizeof(message), 1, 0) < 0)
            break;

        if (strcmp(message.mesg_text, "END") == 0)
        {
            printc("\nEnd of transmission.\n", YEL);
            break;
        }
        if (!caught_name)
        {
            printf("Receiving a file: %s%s%s\n", YEL, message.mesg_text, COLOR_RESET);
            caught_name = true;
            continue;
        }

        printf("%s", message.mesg_text);
    }

    // Sad moze, zatvaranje msgQ -a.
    printf("Closing a message queue with id: %d... ", client_msgid);
    msgctl(client_msgid, IPC_RMID, NULL);
    printc("Queue closed.\n", GRN);

}


int main(int argc, char **argv)
{

    // printf("Client is listening for upcoming messages...\n"
    //        "DON'T Press Ctrl + C to abort. (MessageQueue won't close)\n"
    //        "%s-----%s-----%s-----%s-----%s-----%s-----%s\n",
    //        RED, BLU, WHT, RED, BLU, WHT, CRESET);

    // TODO: Catch SIGINT instead of bitching around :(

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
    printf(COLOR_RESET); // So the cursor doesn't stay colored...
    return 0;
}