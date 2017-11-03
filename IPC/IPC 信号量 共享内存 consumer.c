#include <unistd.h>  
#include <stdlib.h>  
#include <stdio.h>  
#include <string.h>
#include <sys/shm.h>
#include <sys/sem.h>

struct shared_use_st  
{  
	int index[5];			//为0表示对应的缓冲区未被生产者使用，可分配 不可消费；为1表示对应的缓冲区已经被生产者使用，不可分配 可消费
	char buffer[5][100];	//5个字符串缓冲区
};

static int sem_id = 0;

static int semaphore_p();
static int semaphore_v();

int main()  
{
	FILE *out;
	int shmid;	//共享内存标识符
    void *shm = NULL;	//指向共享内存第一个字节的指针
    struct shared_use_st *pshared;
    
    if((out=fopen("/home/zero/learn/linux_lab/write.txt","w"))==NULL)
	{
        printf("写文件打开失败！\n");  
        exit(1);  
    }
	
	//创建共享内存  
    shmid = shmget((key_t)3, sizeof(struct shared_use_st), 0666|IPC_CREAT);  
    if(shmid == -1)  
    {  
    	fprintf(stderr, "shmget failed\n");  
        exit(EXIT_FAILURE);  
    }  
    //将共享内存连接到当前进程的地址空间  
    shm = shmat(shmid, (void *)0, 0);  
    if(shm == (void*)-1)  
   	{  
    	fprintf(stderr, "shmat failed\n");  
        exit(EXIT_FAILURE);  
    }  
    printf("Memory attached at %X\n", (int)shm);  
    //设置共享内存  
    pshared = (struct shared_use_st*)shm;
    struct shared_use_st shared = *pshared;
	//创建信号量  
    sem_id = semget((key_t)10, 1, 0666 | IPC_CREAT);
	
	for (int i = 0; i < 10; i++){
		int n;	//n为访问的buffer序号
		int flag = 0;
		//P操作
		if (!semaphore_p()){
			printf("1\n");
			exit(0);		
		}
		
		for (n = 0; n < 5; n++){
			if (pshared->index[n] == 1){
				flag = 1;
				break;
			}
		}
		
		if (flag){
			printf("%s\n=================buffer[%d]================\n", pshared->buffer[n], n);
			fwrite(pshared->buffer[n], 1, 100, out);
			pshared->index[n] = 0;
			if (!semaphore_v())	//V操作
				exit(0);
		}
		else{
			if (!semaphore_v())	//V操作
				exit(0);
			sleep(2);
		}
	}//for
	
	//把共享内存从当前进程中分离  
	if(shmdt(shm) == -1)  
	{  
        fprintf(stderr, "shmdt failed\n");  
        exit(EXIT_FAILURE);  
    }
	//删除共享内存  
    if(shmctl(shmid, IPC_RMID, 0) == -1)  
    {  
        fprintf(stderr, "shmctl(IPC_RMID) failed\n");  
        exit(EXIT_FAILURE);  
    }
	
	fclose(out);
	
}//main

static int semaphore_p()  
{  
    //对信号量做减1操作，即等待P（sv）  
    struct sembuf sem_b;  
    sem_b.sem_num = 0;  
    sem_b.sem_op = -1;//P()  
    sem_b.sem_flg = SEM_UNDO;  
    if(semop(sem_id, &sem_b, 1) == -1)  
    {  
        fprintf(stderr, "consumer semaphore_p failed\n");  
        return 0;  
    }  
    return 1;  
}  
  
static int semaphore_v()  
{  
    //这是一个释放操作，它使信号量变为可用，即发送信号V（sv）  
    struct sembuf sem_b;  
    sem_b.sem_num = 0;  
    sem_b.sem_op = 1;//V()  
    sem_b.sem_flg = SEM_UNDO;  
    if(semop(sem_id, &sem_b, 1) == -1)  
    {  
        fprintf(stderr, "semaphore_v failed\n");  
        return 0;  
    }  
    return 1;  
}

