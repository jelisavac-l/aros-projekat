#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>

struct mesg_buffer
{
    long mesg_type;
    char mesg_text[100];
} message;

int main()
{
    key_t key;
    int msgid;

    // ftok i slicno da se razumemo medju procesima
    key = ftok("progfile", 65);

    // msgget kreira msgQ i vraca njen id
    msgid = msgget(key, 0666 | IPC_CREAT);

    printf("Client is listening...\nPress Ctrl + C to abort.\n---------\n");

    // Prijem poruke, dok ih ima, jer ako zatvorimo Q pre nego sto
    // sve procitamo, ode ostatak poruke u zaborav

    while (1)
    {
        // Hvala onome ko je napravio man
        if (msgrcv(msgid, &message, sizeof(message), 1, 0) < 0)
            break;

        printf("Data Received is : %s \n",
               message.mesg_text);
    }

    // Sad moze, zatvaranje msgQ -a.
    msgctl(msgid, IPC_RMID, NULL);

    return 0;
}