#include <pthread.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// struct sockaddr_in
// {
//     short int sin_family;        // Address family, AF_INET
//     unsigned short int sin_port; // Port number
//     struct in_addr sin_addr;     // Internet address
//     unsigned char sin_zero[8];   // Same size as struct sockaddr
// };
// sockaddr_in struct format


// page 15 of Beej's Guide to Network Programming

int socketInit(int *MyPort)
{
    int socketDescriptor;
    struct sockaddr_in sin;

    memset(&sin, 0, sizeof(sin)); // makes sure that the struct is empty

    // filling sender information
    sin.sin_family = AF_INET; // IPv4 
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons(*MyPort); // port will be fed in from main function

    // initialize socket + error check
    if ((socketDescriptor = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("failed to generate socket\n");
        exit(EXIT_FAILURE);
    }
    // bind
    // page 15 says that we can cast sockaddr_in to sockaddr, so for anything that requires sockaddr, we can just cast it
    if (bind(socketDescriptor, (struct sockaddr *)&sin, sizeof(sin)) < 0)
    {
        perror("failed to bind\n");
        exit(EXIT_FAILURE);
    }

    return socketDescriptor;
}
