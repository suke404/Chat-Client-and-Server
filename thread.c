#include "list.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define _GNU_SOURCE

#include "sock_init.h"


#define MAX_DATA_SIZE 1024
static pthread_mutex_t listMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t receiveThreadLock = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t receiveListLock = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t outputLock = PTHREAD_MUTEX_INITIALIZER;

static pthread_cond_t listCond = PTHREAD_COND_INITIALIZER;
static pthread_cond_t receiveThreadCond = PTHREAD_COND_INITIALIZER;

static pthread_t inputT;
static pthread_t receiveT;
static pthread_t outputT;
static pthread_t sendT;

static int socketDescriptor;

static char *message = NULL;  // holds the message to be sent
//static char *received = NULL; // holds the message received

static char *remoteName;
static char *remotePort;

static int shutdownFlag = 0;

static List* sendList;
static List* receiveList;

static bool receiveCheck = false;


// thread destruction functions

int inputDestroy()
{
    shutdownFlag =1;
    pthread_cancel(inputT);
    pthread_join(inputT,NULL);
    

    return 0;
}

int receiveDestroy()
{
    shutdownFlag =1;
    pthread_cancel(receiveT);
    pthread_join(receiveT,NULL);
    
    return 0;
}

int outputDestroy()
{
    shutdownFlag =1;
    pthread_cancel(outputT);
    pthread_join(outputT,NULL);
    
    return 0;
}

int sendDestroy()
{
    shutdownFlag =1;
    pthread_cancel(sendT);
    pthread_join(sendT,NULL);
    
    return 0;
}

void destroyThreads()
{
    inputDestroy();
    receiveDestroy();
    outputDestroy();
    sendDestroy();

    // destroy mutex and condition variable
    pthread_mutex_destroy(&listMutex);
    pthread_cond_destroy(&listCond);
    pthread_mutex_destroy(&receiveThreadLock);
    pthread_cond_destroy(&receiveThreadCond);
    pthread_mutex_destroy(&receiveListLock);
    pthread_mutex_destroy(&outputLock);

}


void *inputThread()
{
    while (shutdownFlag == 0)
    {
       
        message = malloc(sizeof(char)*MAX_DATA_SIZE);
        if(message == NULL){
            perror("failed to allocate memory\n");
            exit(EXIT_FAILURE);
        }
    
        fgets(message, MAX_DATA_SIZE, stdin);
        //critical section
        pthread_mutex_lock(&listMutex);
        
        if(List_append(sendList, message) !=LIST_SUCCESS){
            perror("failed to append to list\n");
            exit(EXIT_FAILURE);
        }
    
        pthread_cond_signal(&listCond);
        pthread_mutex_unlock(&listMutex);

        if(strcmp(message, "!\n") == 0){
            // shutdown function
            // TODO
            printf("Shutdown initiated");
            shutdownFlag = 1;
            break;
        }
    }
    
    return NULL;
}

static struct addrinfo hints, *res;

// sends keyboard input
void *sendThread()
{
    int status;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = 0;

    if((status = getaddrinfo(remoteName, remotePort , &hints, &res)) != 0){
        perror("failed to get address info\n");
        exit(EXIT_FAILURE);
    }

    while(shutdownFlag == 0){
        //critical section
        pthread_mutex_lock(&listMutex);

        while(List_count(sendList) == 0){
            // wait for signal
            pthread_cond_wait(&listCond, &listMutex);
        }
        // save message from list
        char* sendMessage = NULL;
        memset(&sendMessage, 0, sizeof(sendMessage));
        sendMessage = List_trim(sendList);
        pthread_mutex_unlock(&listMutex);

        // end critical section, begin networking
        if(sendto(socketDescriptor, sendMessage, strlen(sendMessage), 0, res->ai_addr, res->ai_addrlen) < 0){
            perror("failed to send message\n");
            exit(EXIT_FAILURE);
        }

        if(strcmp(sendMessage, "!\n") == 0){
            shutdownFlag = 1;
            break;
        }
        
        free(sendMessage);
    }
    
  
    return NULL;
}

// receives messages
void *receiveThread()
{
    while(shutdownFlag ==0){
        struct sockaddr_in remoteAddr;
        memset(&remoteAddr, 0, sizeof(remoteAddr));
        unsigned int addrLen = sizeof(remoteAddr);

        char *received = malloc(sizeof(char)*MAX_DATA_SIZE);

        // check if the data is receieved successfully
        if((recvfrom(socketDescriptor, received, MAX_DATA_SIZE-1, 0,(struct sockaddr*)&remoteAddr, &addrLen )) == -1){
            pthread_mutex_lock(&outputLock);
            printf("Didnt receive anything... ");
            pthread_mutex_unlock(&outputLock);
            exit(1);
        }

        if( strcmp(received, "!\n") == 0){
            // close the connection...
            shutdownFlag = 1;
        }

        // Lock the receiveList
        pthread_mutex_lock(&receiveListLock);
            // Insert a new item from the buffer into the receiveList
            List_append(receiveList, received);
        // unlock 
        pthread_mutex_unlock(&receiveListLock);

        pthread_mutex_lock(&receiveThreadLock);
            // Send a siginal to receiveList
            receiveCheck = true;
            pthread_cond_signal(&receiveThreadCond);
        // reset
        pthread_mutex_unlock(&receiveThreadLock);

    }
    return NULL;
}

// prints out received messages
void *outputThread()
{
    while(shutdownFlag == 0){
       
        
        pthread_mutex_lock(&receiveThreadLock);

        while (receiveCheck == false){
            pthread_cond_wait(&receiveThreadCond, &receiveThreadLock);
        }
        
        // reset
        receiveCheck = false;
        
        pthread_mutex_unlock(&receiveThreadLock);
        
        pthread_mutex_lock(&receiveListLock);
            char *received = List_trim(receiveList);
        pthread_mutex_unlock(&receiveListLock);

        pthread_mutex_lock(&outputLock);
            if(received){
                fputs(received, stdout);
                fflush(stdout);
                if(received){
                    free(received);
                }
                
            }
            
        pthread_mutex_unlock(&outputLock);
    }

    return NULL;
}



// thread initialization functions

void inputInit()
{
    pthread_create(&inputT, NULL, inputThread, NULL);
}

void receiveInit()
{
    pthread_create(&receiveT, NULL, receiveThread, NULL);
}

void outputInit()
{
    pthread_create(&outputT, NULL, outputThread, NULL);
}

void sendInit()
{
    pthread_create(&sendT, NULL, sendThread, NULL);
}

void initializeThreads()
{
   
    inputInit();
    receiveInit();
    outputInit();
    sendInit();
}

void freeMessage(void* fr){
    if(fr){
        free(fr);
    }
}
int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        printf("Usage: ./s-talk [my port number] [remote machine name] [remote port number]\n");
        exit(1);
    }
    sendList = List_create();
    receiveList = List_create();

    char *myPort = argv[1];
    remoteName = argv[2];
    remotePort = argv[3];
    
    // convert port numbers to ints
    int myPortInt = atoi(myPort);
    socketDescriptor = socketInit(&myPortInt);

 
    initializeThreads();
    printf("Program Started\n");

    while(shutdownFlag == 0){
        // do nothing
    }
    
    
    // final cleanup of memory
    destroyThreads();
    List_free(sendList, freeMessage);
    List_free(receiveList, freeMessage);
     if(message){
        free(message);
    }
    close(socketDescriptor);
    freeaddrinfo(res);
  


    printf("Program Ended\n");
    exit(0);

}