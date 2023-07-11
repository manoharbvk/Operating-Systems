#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
#include <stdbool.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#define CONNECT_CHANNEL 100
#define MAX_CLIENT_NAME 100
#define MAX_CLIENTS 100
#define SERVER_BUSY 11
#define SERVER_READY 9
#define SUCCESSFULL 7
#define NOT_SUCCESSFULL 75
#define USER_EXIST 5
#define CLIENT_LIMIT_EXCEEDED 2
#define CLIENT_REQUESTED 8
#define FINISHED 6
#define NOT_REPLIED 10
#define ACK 4
#define NACK 2
#define PROCESSING 45
#define STOP 56
#define NOT_CLIENT_REQUESTED 89
#define MSG_REC 69
#define PRINT_INFO(MSG, ...) { \
    int ii=rand(); \
	printf ( "%s %d INFO %d:%d %ld %s %s %d : " MSG ";;\n", \
	"TODO_PRINT_TIME",ii, getpid(), getppid(), pthread_self(), __FILE__, __FUNCTION__, \
	__LINE__,  ##__VA_ARGS__); \
}
typedef struct Response
{
    key_t key;
    int status;
    int server_reply;
    int ack;
}response;
typedef struct Arithmetic{
    float x;
    float y;
    char action;
}arithmetic;
typedef struct EvenOrOdd{
    int x;
}evenOrOdd;
typedef struct IsPrime{
    int x;
}isPrime;


typedef struct com_Response
{
    char msg[100];
    float ans;
    int status;
    int ack;
}com_response;

typedef struct Request
{
    char name[MAX_CLIENT_NAME];
    int client_status;

}request;

typedef struct comRequest
{
    int request_type;
    int client_status;
    arithmetic arth;
    evenOrOdd eoo;
    isPrime ip;

}com_request;

typedef struct Communication
{
    com_response Server_response;
    com_request Client_request;
}communication;

typedef struct Channel
{
    response Server_response;
    request Client_request;
    sem_t sem;
    
}channel;

typedef struct Com_channel
{
    com_response Server_response;
    com_request Client_request;
    
}Com_channel;

int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        PRINT_INFO("Wrong arguments\n");
        exit(1);
    }
    channel *connection;
    int shmid;
    int sem_value;
    key_t key =CONNECT_CHANNEL;
    // shmget returns an identifier in shmid
    if ((shmid = shmget(key,sizeof(channel), 0666)) < 0) {
        perror("Server not reachable.");
        exit(1);
    }
  
    // shmat to attach to shared memory
    connection = (channel*) shmat(shmid,(void*)0,0);
    PRINT_INFO("Client waiting for connection channel\n");
        sem_getvalue(&connection->sem, &sem_value);
        PRINT_INFO("3sem value: %d\n",sem_value);

    sem_wait(&connection->sem);
        sem_getvalue(&connection->sem, &sem_value);
        PRINT_INFO("2sem value: %d\n",sem_value);

    PRINT_INFO("client got connection channel\n");
    sleep(5);
    while(connection->Server_response.status!= SERVER_READY )
    {
        usleep(1);
    }
    connection->Server_response.ack=NACK;
    strcpy(connection->Client_request.name, argv[1]);
    connection-> Client_request.client_status=CLIENT_REQUESTED;
    while(connection->Server_response.ack == NACK)
    {
        usleep(1);
    }
    while(connection->Server_response.status == SERVER_BUSY)
    {
        usleep(1);
    }
    if(connection->Server_response.server_reply == USER_EXIST)
    {
        PRINT_INFO("user already existed with name as %s\n",connection->Client_request.name);
        sem_post(&connection->sem);
    }
    else if( connection->Server_response.server_reply == SUCCESSFULL)
    {
        PRINT_INFO("Connection successfull with Communication channel with key value=%d \n",connection->Server_response.key);

        int shm;
        if ((shm = shmget(connection->Server_response.key,sizeof(communication), 0666)) < 0) {
        perror("Server not reachable.");
        exit(1);}
    communication *data_comm;
    data_comm = (communication*) shmat(shm,(void*)0,0);
        sem_getvalue(&connection->sem, &sem_value);
        PRINT_INFO("1sem value: %d\n",sem_value);
        sem_post(&connection->sem);
        sem_getvalue(&connection->sem, &sem_value);
        PRINT_INFO("0sem value: %d\n",sem_value);
    int x = 0;
    int ik = 0;
    while(1){
        ik++;
        PRINT_INFO("Press 1: for Arthemetic operation\n");
        PRINT_INFO("Press 2: for Even or Odd operation\n");
        PRINT_INFO("Press 3: for Is Prime operation\n");
        PRINT_INFO("Press -1: to close communication channel\n");
        PRINT_INFO("enter the number:\n");
        scanf("%d",&x);
        if(data_comm == NULL){
            PRINT_INFO("server stopped\n");
            exit(1);
        }
        if(x==-1){
                    data_comm->Client_request.client_status = STOP;
                    break;
        }
        else if(x==1){
            PRINT_INFO("Please enter two operands separated by space and the type of arthimaetic operation\n");
            data_comm->Client_request.request_type=1;
            scanf("%f",&data_comm->Client_request.arth.x);       
            scanf("%f",&data_comm->Client_request.arth.y);       
            scanf(" %c",&data_comm->Client_request.arth.action);       
        }
        else if(x==2){
            PRINT_INFO("Please enter operands (evenorodd)\n");
            data_comm->Client_request.request_type=2;
            scanf("%d",&data_comm->Client_request.eoo.x);       
        }
        else if(x==3){
            PRINT_INFO("Please enter operands (isPrime)\n");
            data_comm->Client_request.request_type=3;
            scanf("%d",&data_comm->Client_request.ip.x);         
        }
        else{
            data_comm->Client_request.request_type=x;
        }
        while(data_comm!=NULL && data_comm->Server_response.status!= SERVER_READY){
            usleep(1);
        }
        if(data_comm==NULL)
        {
            PRINT_INFO("server stopped\n");
            exit(1);
        }
        data_comm->Server_response.ack = NACK;
        data_comm->Client_request.client_status = CLIENT_REQUESTED;
        PRINT_INFO("A request for %d operation is sent\n",x);
            while(data_comm->Server_response.ack == NACK){
                usleep(1);
            }
            while (data_comm->Server_response.status == PROCESSING)
            {
                usleep(1);
            }
            if(data_comm->Server_response.status == SUCCESSFULL) {
                PRINT_INFO("%s\n",data_comm->Server_response.msg);
                if(data_comm->Client_request.request_type==1){
                    PRINT_INFO("A response is received from the communication channel")
                    PRINT_INFO("The operation result is:%f\n",data_comm->Server_response.ans);
                }
                data_comm->Client_request.client_status = MSG_REC;
            }
            else {
                PRINT_INFO("%s\n",data_comm->Server_response.msg);
                data_comm->Client_request.client_status = MSG_REC;
            }
        
    }
    }
    else if(connection->Server_response.server_reply== CLIENT_LIMIT_EXCEEDED)
    {
        PRINT_INFO("Server too busy, Try after some time\n");
        sem_post(&connection->sem);
        
    }
    
    
    //detach from shared memory 
    shmdt(connection);
}