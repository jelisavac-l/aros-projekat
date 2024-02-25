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

    // Gospode Boze... Dok ne smislim nesto pametnije (novi thread)
    // Neka bude ovako...
    printf("Client is listening for 3 upcoming messages...\n"
           "DON'T Press Ctrl + C to abort. (MessageQueue won't close)\n"
           "==------------------------\n");

    // Prijem poruke, dok ih ima, jer ako zatvorimo Q pre nego sto
    // sve procitamo, ode ostatak poruke u zaborav

    int broj_poruka = 0;
    while (1)
    {

        // Hvala onome ko je napravio man
        if (msgrcv(msgid, &message, sizeof(message), 1, 0) < 0)
            break;

        printf("Data Received is : %s \n",
               message.mesg_text);

        broj_poruka++;
        if (broj_poruka >= 3)
            break;
    }

    // Sad moze, zatvaranje msgQ -a.
    printf("Closing a message queue with id: %d...\n", msgid);
    msgctl(msgid, IPC_RMID, NULL);
    printf("Queue closed. Return.\n");
    return 0;
}