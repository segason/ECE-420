#include <stdio.h>
#include <stdlib.h>
#include "timer.h"
#include "common.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

void *Operate(void *args);

int array_size;
int port;
int request_counter = 0;
char *server_ip;
char **theArray;
double mean = 0;
double * requestTimes;
pthread_mutex_t mutex;


int main(int argc, char *argv[])
{

    int i;
    requestTimes = malloc (COM_NUM_REQUEST * sizeof(double));
    /* Get array size, IP address and port number from command line */
    array_size = strtol(argv[1], NULL, 10);
    server_ip = argv[2];
    port = strtol(argv[3], NULL, 10);
    pthread_mutex_init(&mutex, NULL);

    /* Create the memory and fill in the initial values for theArray */
    theArray = (char **)malloc(array_size * sizeof(char *));
    for (i = 0; i < array_size; i++)
    {
        theArray[i] = (char *)malloc(COM_BUFF_SIZE * sizeof(char));
        sprintf(theArray[i], "theArray[%d]: initial value", i);
        printf("%s\n\n", theArray[i]);
    }

    struct sockaddr_in sock_var;
    int serverFileDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    int clientFileDescriptor;
    pthread_t *t;
    t = malloc(COM_NUM_REQUEST * sizeof(pthread_t));

    sock_var.sin_addr.s_addr = inet_addr(server_ip);
    sock_var.sin_port = port;
    sock_var.sin_family = AF_INET;
    if (bind(serverFileDescriptor, (struct sockaddr *)&sock_var, sizeof(sock_var)) >= 0)
    {
        printf("socket has been created\n");
        listen(serverFileDescriptor, 2000);

        while (1)
        {
            for (i = 0; i < COM_NUM_REQUEST; i++) //can support COM_NUM_REQUEST clients at a time
            {
                clientFileDescriptor = accept(serverFileDescriptor, NULL, NULL);
                //printf("Connected to client %d\n", clientFileDescriptor);
                pthread_create(&t[i], NULL, Operate, (void *)clientFileDescriptor);
            }
            
            for (i = 0; i < COM_NUM_REQUEST; i++) //can support COM_NUM_REQUEST clients at a time
            {
                pthread_join(t[i], NULL);
            }
            saveTimes(requestTimes, request_counter);
            request_counter = 0;
        }

        close(serverFileDescriptor);
        pthread_mutex_destroy(&mutex);
        for (i = 0; i < array_size; ++i)
        {
            free(theArray[i]);
        }
        free(theArray);
        free(t);
        free(requestTimes);
    }
    else
    {
        printf("socket creation failed\n");
    }
    return 0;
}

void *Operate(void *args)
{
    double start, finish, elapsed;
    GET_TIME(start);
    int clientFileDescriptor = (int)args;
    char str[COM_BUFF_SIZE];
    ClientRequest rqst = {-1, -1, ""};

    read(clientFileDescriptor, str, COM_BUFF_SIZE);
    //printf("reading from client:%s\n", str);
    ParseMsg(str, &rqst);

    pthread_mutex_lock(&mutex);
    if (rqst.is_read == 0)
    {
        setContent(&rqst.msg, rqst.pos, (char **)theArray);
        char *msg = theArray[rqst.pos];
        //printf("%s\n\n", msg);
        write(clientFileDescriptor, msg, COM_BUFF_SIZE);
    }

    else
    {
        char dst[COM_BUFF_SIZE];
        getContent(&dst, rqst.pos, (char **)theArray);
        //printf("%s\n\n", dst);
        write(clientFileDescriptor, dst, COM_BUFF_SIZE);
    }

    GET_TIME(finish);
    elapsed = finish - start;
    //printf("The elapsed time is %e seconds\n", elapsed);
    if(request_counter < COM_NUM_REQUEST){
        requestTimes[request_counter] = elapsed;
        request_counter++;
        mean += elapsed;
        printf("Sum = %lf ns\n", mean);
        printf("Req Counter = %d \n", request_counter);
    }
    
    pthread_mutex_unlock(&mutex);


    close(clientFileDescriptor);
    
    return NULL;
}