#include "common.h"


// USAGE: Run using ./tcpclient <server address>
// EXAMPLE: ping google to get address: ping www.google.com
//          run: ./tcpclient 142.250.217.78


int main(int argc, char **argv){
    int sockfd, n;
    int sendbytes;
    struct sockaddr_in servaddr;
    char sendline[MAXLINE];
    char recvline[MAXLINE];

    if(argc != 2)
        err_n_die("usage: %s <server address>", argv[0]);
    
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)  // create stream socket, AF_INET = IPv4, 0 = TCP
        err_n_die("Error while creating socket");
    
    bzero(&servaddr, sizeof(servaddr)); // zero out the address
    servaddr.sin_family = AF_INET;  // specify AF_INET
    servaddr.sin_port = htons(SERVER_PORT); // specify port, htons = host to network short

    if(inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0) // translates the address from text to binary form
        err_n_die("inet_pton error for %s", argv[1]);
    
    if(connect(sockfd, (SA *) &servaddr, sizeof(servaddr)) < 0) // connect to the server
        err_n_die("connect failed");
    
    sprintf(sendline, "GET / HTTP/1.1\r\n\r\n"); // send the request
    sendbytes = strlen(sendline);

    if (write(sockfd, sendline, sendbytes) != sendbytes) // send the request
        err_n_die("write error");
    
    memset(recvline, 0, MAXLINE); // clear the buffer

    while((n = read(sockfd, recvline, MAXLINE - 1)) > 0){ // read the response
        printf("%s", recvline);
        memset(recvline, 0, MAXLINE); // clear the buffer
    }
    if (n < 0)
        err_n_die("read error");
    
    exit(0); // exit
}