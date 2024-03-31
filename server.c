#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdbool.h>
// #include <libgen.h>
#include "extern/printC.h"

#include "uiplusplus.h"

#define MAX 16
#define MAX_LINE_LENGTH 1000
#define MAX_LINES 1000

struct mesg_buffer
{
    long mesg_type;
    char mesg_text[100];
} message;

int respond(char *path, char* target)
{    

    char line[MAX_LINE_LENGTH];
    char lines[MAX_LINES][MAX_LINE_LENGTH];
    int line_counter = 0;

    FILE *fileptr;
    fileptr = fopen(path, "r");
    if (fileptr == NULL)
    {
        printc("Error: invalid path or nonexistant file!\n", RED);
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

    char* target = get_file_name(argv[1]);
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
