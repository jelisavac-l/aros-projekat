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

    // Prijem poruke
    msgrcv(msgid, &message, sizeof(message), 1, 0);

    printf("Data Received is : %s \n",
           message.mesg_text);

    // Brisanje otvorenog msgqueueua, staviti ovo u posebnu dat.
    msgctl(msgid, IPC_RMID, NULL);

    return 0;
}