#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdbool.h>
#include "extern/printC.h"

#include "uiplusplus.h"

struct mesg_buffer
{
    long mesg_type;
    char mesg_text[100];
} message;

void accept()
{
    key_t key;
    int msgid;

    // ftok i slicno da se razumemo medju procesima
    key = ftok("progfile", 65);

    // msgget kreira msgQ i vraca njen id
    msgid = msgget(key, 0666 | IPC_CREAT);

    // Prijem poruke, dok ih ima, jer ako zatvorimo Q pre nego sto
    // sve procitamo, ode ostatak poruke u zaborav


    bool caught_name = false;
    while (1)
    {

        

        // Hvala onome ko je napravio man
        if (msgrcv(msgid, &message, sizeof(message), 1, 0) < 0)
            break;

        if (strcmp(message.mesg_text, "END") == 0)
        {
            printc("\nEnd of transmission.\n", YEL);
            break;
        }
        if(!caught_name) 
        {
            printf("Receiving a file: %s%s%s\n", YEL, message.mesg_text, COLOR_RESET);
            caught_name = true;
            continue;
        }


        printf("%s", message.mesg_text);
    }

    // Sad moze, zatvaranje msgQ -a.
    printf("Closing a message queue with id: %d... ", msgid);
    msgctl(msgid, IPC_RMID, NULL);
    printc("Queue closed.\n", GRN);
}

int main(int argc, char **argv)
{

    printf("Client is listening for upcoming messages...\n"
           "DON'T Press Ctrl + C to abort. (MessageQueue won't close)\n"
           "%s-----%s-----%s-----%s-----%s-----%s-----%s\n",
           RED, BLU, WHT, RED, BLU, WHT, CRESET);

    if (argv[1] == NULL)
        accept();

    else if (!strcmp(argv[1], "--h"))
    {
        print_help(CLI);
        printf(COLOR_RESET);
        return 0;
    }
    printf(COLOR_RESET); // So the cursor doesn't stay colored...
    return 0;
}