#include <unistd.h>  
#include <stdlib.h>  
#include <stdio.h>  
#include <string.h>  
#include <errno.h>  
#include <sys/msg.h>  
  
struct msg_st  
{  
    long int msg_type;  
    char text[BUFSIZ];  
};  

struct type_st
{
	key_t key_id;
	int msg_id;	
};

void recvmsg(key_t key_id,int msgid,int msgtype,int *running)
{
	struct msg_st data; 
	if(msgrcv(msgid, (void*)&data, BUFSIZ, msgtype, 0) == -1)  
    {  
        fprintf(stderr, "msgrcv failed with errno: %d\n", errno);  
        exit(EXIT_FAILURE);  
    }  
    printf("You wrote pthread%d: %s\n",msgtype,data.text);  
    //遇到end结束  
    if(strncmp(data.text, "end", 3) == 0)  
        *running = 0; 
	printf("running1:%d\n",*running);
}

void *thr_fun1(void *arg)
{
	struct type_st *p = (struct type_st*) arg;
	int running = 1; 
	long int msgtype = 1; //注意1
	key_t key_id = p->key_id;
	int msg_id = p->msg_id;
	//从队列中获取消息，直到遇到end消息为止  
    while(running)  
    {  
        recvmsg(key_id,msg_id,msgtype,&running); 
    } 
}

void *thr_fun2(void *arg)
{
	struct type_st *p = (struct type_st*) arg;
	int running = 1; 
	long int msgtype = 2; //注意1
	key_t key_id = p->key_id;
	int msg_id = p->msg_id;
	//从队列中获取消息，直到遇到end消息为止  
    while(running)  
    {  
       recvmsg(key_id,msg_id,msgtype,&running);  
    } 
}

void *thr_fun3(void *arg)
{
	struct type_st *p = (struct type_st*) arg;
	int running = 1; 
	long int msgtype = 3; //注意1
	key_t key_id = p->key_id;
	int msg_id = p->msg_id;
	//从队列中获取消息，直到遇到end消息为止  
    while(running)  
    {
    	recvmsg(key_id,msg_id,msgtype,&running);   
    } 
}
  
int main()  
{  
 
    int msgid = -1;  
    struct msg_st data;  
    //long int msgtype = 1; //注意1  
  	struct type_st st_type;
  	pthread_t td1, td2, td3;//创建3个线程用来接收3种不同的消息
    //建立消息队列  
    msgid = msgget((key_t)1234, 0666 | IPC_CREAT);  
    if(msgid == -1)  
    {  
        fprintf(stderr, "msgget failed with error: %d\n", errno);  
        exit(EXIT_FAILURE);  
    }
    
    st_type.key_id = 1234;
    st_type.msg_id = msgid;
    int ret = pthread_create(&td1, NULL, thr_fun1, &st_type);  
    if ( 0 != ret )  
    {  
        fprintf(stderr, "sync thread create error!\n");  
        exit(1);  
    }  
    
    ret = pthread_create(&td2, NULL, thr_fun2, &st_type);  
    if ( 0 != ret )  
    {  
        fprintf(stderr, "sync thread create error!\n");  
        exit(1);  
    }
    
    ret = pthread_create(&td3, NULL, thr_fun3, &st_type);  
    if ( 0 != ret )  
    {  
        fprintf(stderr, "sync thread create error!\n");  
        exit(1);  
    }
   
    pthread_join(td1, NULL);	//等待线程1结束
    //pthread_join(td2, NULL);
    //pthread_join(td3, NULL);
    //删除消息队列  
    if(msgctl(msgid, IPC_RMID, 0) == -1)  
    {  
        fprintf(stderr, "msgctl(IPC_RMID) failed\n");  
        exit(EXIT_FAILURE);  
    }  
    exit(EXIT_SUCCESS);  
}  
