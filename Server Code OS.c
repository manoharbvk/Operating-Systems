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

typedef struct Client_data 
{
    /* data */
    char client_name[MAX_CLIENT_NAME];
    key_t key;
    int request_count;
    pthread_t thread_number;
    bool Comm_channel_isCreated;
    int cur_run;

}Client_data;
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
int total_req=0;
typedef struct Com_channel
{
    com_response Server_response;
    com_request Client_request;
    
}Com_channel;
char check;
bool is_prime(int n){
    //logic;
    if (n <= 1)
        return false;
 
    // Check from 2 to n-1
    for (int i = 2; i < n; i++)
        if (n % i == 0)
            return false;
 
    return true;
}
void* test()
{

    scanf("%c",&check);
    pthread_exit(0);
}
void *worker(void *data)
{   
    PRINT_INFO("worker thread created\n");
    // int shmid;
    Client_data *client_data;
    client_data = (Client_data*) data;
    int shmid2 = shmget(client_data->key,sizeof(communication),0666|IPC_CREAT);
        communication *data_comm;
        data_comm = (communication*) shmat(shmid2,(void*)0,0);
        data_comm->Server_response.status  = SERVER_READY;
        client_data->Comm_channel_isCreated= true;
    fflush(stdout);
    while(data_comm->Client_request.client_status!=STOP && check=='\0'){
    data_comm->Server_response.status  = SERVER_READY; 
    
    while(data_comm->Client_request.client_status!=CLIENT_REQUESTED && data_comm->Client_request.client_status!=STOP && check=='\0')
    {
        usleep(1);
    }
    if(data_comm->Client_request.client_status==STOP || check!='\0')
    break;
    data_comm->Server_response.ack = ACK;
    data_comm->Server_response.status = PROCESSING;
    switch(data_comm->Client_request.request_type)
    {
        case 1:PRINT_INFO("%s requested Arithmetic operation\n",client_data->client_name);
                break;
        case 2:PRINT_INFO("%s requested EvenOrOdd operation\n",client_data->client_name);
                break;
        case 3:PRINT_INFO("%s requested IsPrime operation\n",client_data->client_name);
                break;
        case -1:PRINT_INFO("%s requested Unregister operation \n",client_data->client_name);
                break;
        default: PRINT_INFO("%s requested for an invalid operation \n",client_data->client_name);
                break;
    }
    
    if(data_comm->Client_request.request_type==1){
        float x = data_comm->Client_request.arth.x;
        float y = data_comm->Client_request.arth.y;
        char op = data_comm->Client_request.arth.action;
        if(op =='+'){
            float ans = x+y;
            data_comm->Server_response.ans = ans;
            char* str = "addition successfull";
            strcpy(data_comm->Server_response.msg,str);
            data_comm->Server_response.status = SUCCESSFULL;
        }
        else if(op =='-'){
            float ans = x-y;
            data_comm->Server_response.ans = ans;
            char* str = "subtraction successfull";
            strcpy(data_comm->Server_response.msg,str);
            data_comm->Server_response.status = SUCCESSFULL;
        }
        else if(op =='/'){
            if (y==0){
                data_comm->Server_response.status = NOT_SUCCESSFULL;
                char* str = "divide by zero error";
                strcpy(data_comm->Server_response.msg,str); 
            }
            else{
            float ans = x/y;
            data_comm->Server_response.ans = ans;
            char* str = "division successfull";
            strcpy(data_comm->Server_response.msg,str);
            data_comm->Server_response.status = SUCCESSFULL;

            }
        }
        else if(op =='*'){
            float ans = x*y;
            data_comm->Server_response.ans = ans;
            char* str = "multiplication successfull";
            strcpy(data_comm->Server_response.msg,str);
            data_comm->Server_response.status = SUCCESSFULL;
        }
        else{
            char* str = "no such arth operation";
            strcpy(data_comm->Server_response.msg,str);
            data_comm->Server_response.status = NOT_SUCCESSFULL;
        }
    }
    else if(data_comm->Client_request.request_type==2){
        int x = data_comm->Client_request.eoo.x;
        if(x%2){
            char* str = "the number is odd";
            strcpy(data_comm->Server_response.msg,str);
            data_comm->Server_response.status = SUCCESSFULL;   
            data_comm->Server_response.ans = 1;         
        }
        else{
            char* str = "the number is even";
            strcpy(data_comm->Server_response.msg,str);
            data_comm->Server_response.status = SUCCESSFULL; 
            data_comm->Server_response.ans = 0;         
        }
    }
    else if(data_comm->Client_request.request_type==3){
        int x = data_comm->Client_request.ip.x;
        if(is_prime(x)){
            char* str = "the number is prime";
            strcpy(data_comm->Server_response.msg,str);
            data_comm->Server_response.status = SUCCESSFULL;   
            data_comm->Server_response.ans = 1;            
        }
        else{
            char* str = "the number is not prime";
            strcpy(data_comm->Server_response.msg,str);
            data_comm->Server_response.status = SUCCESSFULL;   
            data_comm->Server_response.ans = 0;              
        }
    }
    else{
            char* str = "operation not correct";
            strcpy(data_comm->Server_response.msg,str);
            data_comm->Server_response.status = NOT_SUCCESSFULL;   
            data_comm->Server_response.ans = 0; 
    }
    fflush(stdout);
    while(data_comm->Client_request.client_status!=MSG_REC){
        usleep(1);
    }
    client_data->request_count++;
    total_req++;
    }
    // PRINT_INFO("goodbuye\n");
    client_data->cur_run=0;
    PRINT_INFO("total client (%s) requests  = %d\n",client_data->client_name,client_data->request_count);
    shmdt(data_comm);
    shmctl(shmid2, IPC_RMID, NULL);
    PRINT_INFO("Server unregisterd client %s \n",client_data->client_name);
    PRINT_INFO("communication close\n");
    pthread_exit(0);
}
int main(int argc, char *argv[])
{
    pthread_t th1;
    pthread_t worker_thread;
    check='\0';
    pthread_create(&th1, NULL, test,NULL);
    Client_data client_list[MAX_CLIENTS];
    int no_of_clients=0;
    key_t key =CONNECT_CHANNEL;
    channel *connection;

    // shmget returns an identifier in shmid
    int shmid = shmget(key,sizeof(channel),0666|IPC_CREAT);
    
    // shmat to attach to shared memory
    connection = (channel*) shmat(shmid,(void*)0,0);
    sem_init(&connection->sem, 1, 2);
    sem_wait(&connection->sem);
    connection->Server_response.ack=NACK;
    connection->Server_response.status = SERVER_READY;
    while(check == '\0'){
    PRINT_INFO("Server is waiting for client\n");
    bool temp=false;
    while(connection->Client_request.client_status != CLIENT_REQUESTED && check=='\0')
    {
    
        usleep(1);
    }
    connection->Server_response.server_reply = SUCCESSFULL;
    if(check!= '\0')break;
    connection->Server_response.ack=ACK;
    connection-> Server_response.status = SERVER_BUSY;
    PRINT_INFO("Server is processing %s request\n", connection->Client_request.name);
    for(int i=0 ;i< MAX_CLIENTS ;i++)
    {
        if(!strcmp(connection-> Client_request.name, client_list[i].client_name ) && client_list[i].cur_run==1)
        {
            connection->Server_response.server_reply=USER_EXIST;
            PRINT_INFO("user already existed with name as %s\n",connection->Client_request.name);
            break;
        }

    }
    if(connection->Server_response.server_reply != USER_EXIST)
    {
        connection->Server_response.server_reply = SUCCESSFULL;
        
        
        PRINT_INFO("Communication channel for %s is created\n",connection->Client_request.name);
        strcpy(client_list[no_of_clients].client_name , connection->Client_request.name);
        connection->Server_response.key= no_of_clients + 1234;
        // ftok(connection->Client_request.name , 69);
        // int shmid2 = shmget(connection->Server_response.key,sizeof(communication),0666|IPC_CREAT);
        client_list[no_of_clients].key= connection->Server_response.key;
        // communication *data_comm;
        // data_comm = (communication*) shmat(shmid,(void*)0,0);
        // PRINT_INFO("before%s",data_comm->test);
        // data_comm->test[0] ='$';
        // PRINT_INFO("agfshgdtfnb%c",data_comm->test[0]);
        // shmdt(data_comm);
        client_list[no_of_clients].Comm_channel_isCreated=false;
        client_list[no_of_clients].cur_run = 1;
        pthread_create(&worker_thread, NULL, worker,(void*) &client_list[no_of_clients]);
        // pthread_create(&worker_thread, NULL, worker, (void*) &shmid2);
        while(!client_list[no_of_clients].Comm_channel_isCreated)usleep(1);
        client_list[no_of_clients].thread_number=worker_thread;
        no_of_clients++;
    }

    connection->Server_response.status =SERVER_READY;
    connection->Client_request.client_status= FINISHED;
    }
    pthread_join(th1, NULL);
    // pthread_join(worker_thread, NULL);

    for(int i=0; i< no_of_clients;i++)
    {
        pthread_join(client_list[i].thread_number,NULL);
    }
    for(int i=0 ;i<no_of_clients;i++)
    {
        PRINT_INFO("%s requested %d resquests\n",client_list[i].client_name,client_list[i].request_count);
    }
    PRINT_INFO("Server gave total %d responses for %d clients\n ",total_req,no_of_clients);
    sem_post(&connection->sem);
    shmdt(connection);
    shmctl(shmid, IPC_RMID, NULL);
}