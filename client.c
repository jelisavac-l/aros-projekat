#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "extern/printC.h"

struct mesg_buffer
{
    long mesg_type;
    char mesg_text[100];
} message;

int main()
{

    printf("Client is listening for upcoming messages...\n"
           "DON'T Press Ctrl + C to abort. (MessageQueue won't close)\n"
           "%s-----%s-----%s-----%s-----%s-----%s-----%s\n",
           RED, BLU, WHT, RED, BLU, WHT, CRESET);

    key_t key;
    int msgid;

    // ftok i slicno da se razumemo medju procesima
    key = ftok("progfile", 65);

    // msgget kreira msgQ i vraca njen id
    msgid = msgget(key, 0666 | IPC_CREAT);

    // Prijem poruke, dok ih ima, jer ako zatvorimo Q pre nego sto
    // sve procitamo, ode ostatak poruke u zaborav

    while (1)
    {

        // Hvala onome ko je napravio man
        if (msgrcv(msgid, &message, sizeof(message), 1, 0) < 0)
            break;
        if (strcmp(message.mesg_text, "END") == 0)
        {
            printc("End of transmission.\n", YEL);
            break;
        }

        printf("Data Received is : %s \n",
               message.mesg_text);
    }

    // Sad moze, zatvaranje msgQ -a.
    printf("Closing a message queue with id: %d... ", msgid);
    msgctl(msgid, IPC_RMID, NULL);
    printc("Queue closed.\n", GRN);
    return 0;
}