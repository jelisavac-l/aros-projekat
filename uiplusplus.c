#include <stdio.h>
#include "uiplusplus.h"
#include "extern/printC.h"

void print_splash_screen(int proc_type)
{
    if (proc_type == SRV)
    {
        printc(" ░▒▓███████▓▒░▒▓████████▓▒░▒▓███████▓▒░░▒▓█▓▒░░▒▓█▓▒░▒▓████████▓▒░▒▓███████▓▒░  \n", GRN);
        printc("░▒▓█▓▒░      ░▒▓█▓▒░      ░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░      ░▒▓█▓▒░░▒▓█▓▒░ \n", GRN);
        printc("░▒▓█▓▒░      ░▒▓█▓▒░      ░▒▓█▓▒░░▒▓█▓▒░░▒▓█▓▒▒▓█▓▒░░▒▓█▓▒░      ░▒▓█▓▒░░▒▓█▓▒░ \n", CYN);
        printc(" ░▒▓██████▓▒░░▒▓██████▓▒░ ░▒▓███████▓▒░ ░▒▓█▓▒▒▓█▓▒░░▒▓██████▓▒░ ░▒▓███████▓▒░  \n", CYN);
        printc("       ░▒▓█▓▒░▒▓█▓▒░      ░▒▓█▓▒░░▒▓█▓▒░ ░▒▓█▓▓█▓▒░ ░▒▓█▓▒░      ░▒▓█▓▒░░▒▓█▓▒░ \n", CYN);
        printc("       ░▒▓█▓▒░▒▓█▓▒░      ░▒▓█▓▒░░▒▓█▓▒░ ░▒▓█▓▓█▓▒░ ░▒▓█▓▒░      ░▒▓█▓▒░░▒▓█▓▒░ \n", GRN);
        printc("░▒▓███████▓▒░░▒▓████████▓▒░▒▓█▓▒░░▒▓█▓▒░  ░▒▓██▓▒░  ░▒▓████████▓▒░▒▓█▓▒░░▒▓█▓▒░ \n\n", GRN);
    }
    else if (proc_type == CLI)
    {
        printc(" ░▒▓██████▓▒░░▒▓█▓▒░      ░▒▓█▓▒░▒▓████████▓▒░▒▓███████▓▒░▒▓████████▓▒░ \n", YEL);
        printc("░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░      ░▒▓█▓▒░▒▓█▓▒░      ░▒▓█▓▒░░▒▓█▓▒░ ░▒▓█▓▒░     \n", YEL);
        printc("░▒▓█▓▒░      ░▒▓█▓▒░      ░▒▓█▓▒░▒▓█▓▒░      ░▒▓█▓▒░░▒▓█▓▒░ ░▒▓█▓▒░     \n", YEL);
        printc("░▒▓█▓▒░      ░▒▓█▓▒░      ░▒▓█▓▒░▒▓██████▓▒░ ░▒▓█▓▒░░▒▓█▓▒░ ░▒▓█▓▒░     \n", WHT);
        printc("░▒▓█▓▒░      ░▒▓█▓▒░      ░▒▓█▓▒░▒▓█▓▒░      ░▒▓█▓▒░░▒▓█▓▒░ ░▒▓█▓▒░     \n", YEL);
        printc("░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░      ░▒▓█▓▒░▒▓█▓▒░      ░▒▓█▓▒░░▒▓█▓▒░ ░▒▓█▓▒░     \n", YEL);
        printc(" ░▒▓██████▓▒░░▒▓████████▓▒░▒▓█▓▒░▒▓████████▓▒░▒▓█▓▒░░▒▓█▓▒░ ░▒▓█▓▒░ \n\n", YEL);
    }
}

extern void print_help(int proc_type)
{
    if(proc_type == SRV)
    {
        // print_splash_screen(proc_type);
        printf("Usage: ./server path_to_file\n");
        printf("Options:\n");
        printf("\t -l\tListen for requests on message queue. Work in progress.\n");
    }
    else if(proc_type == CLI)
    {
        // print_splash_screen(proc_type);
        printc("You are seriously asking for help on a client application, you little bitch?\n"
                "It still does nothing. Sometimes it accepts some messages. Usually it doesn't.\n", RED);

        printf("Options:\n");
        printf("\t -r\tSend request to a server. Work in progress.\n");
    }
}
