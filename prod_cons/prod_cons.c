#include <stdio.h>          /* printf()                 */
#include <stdlib.h>         /* exit(), malloc(), free() */
#include <unistd.h>
#include <sys/types.h>      /* key_t, sem_t, pid_t      */
#include <string.h>
#include <errno.h>          /* errno, ECHILD            */
#include <semaphore.h>      /* sem_open(), sem_destroy(), sem_wait().. */
#include <fcntl.h>          /* O_CREAT, O_EXEC          */
#include <signal.h>
#include <pthread.h>

#include "linked_list.h"
#include "queue.h" 
#include "cbuffer.h"

#define RED	printf("\033[1;31m")
#define BLUE printf("\033[1;36m")
#define MAGENTA	printf("\033[1;35m")
#define YELLOW printf("\033[1;33m")
#define GREEN printf("\033[1;32m")
#define DEFAULT	printf("\033[0m")

#define NUM_OF_THREADS (MAX_IN)
#define MAX_IN (20)

static void *threadFunctionC(void *buffer);
static void *threadFunctionP(void *buffer);
static void *removeNodeThread2(void *list);
static void *insertNodeThread2(void *list);
static void *removeNodeThread3(void *list);
static void *insertNodeThread3(void *list);
static void *EnqueueThread4(void *queue);
static void *DequeueThread4(void *queue);
static void *WriteThread5(void *queue);
static void *ReadThread5(void *queue);
static void *EnqueueThread6(void *queue);
static void *DequeueThread6(void *queue);
static void *WriteThread7(void *cbuff);
static void *ReadThread7(void *cbuff);
static void *WriteThread8(void *cbuff);
static void *ReadThread8(void *cbuff);

static void exs1();
static void exs2();
static void exs3();
static void exs4();
static void exs5();
static void exs6();
static void exs7();
static void exs8();

int atomic_consumer_flag_g = 0;
int atomic_producer_flag_g = 0;

sem_t *sem_g = NULL;
sem_t *sem2_g = NULL;
pthread_mutex_t lock; 
pthread_mutex_t lock2; 
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

int main()
{
     exs1(); 
    /* exs2(); */
    /* exs3(); */
    /* exs4(); */
    /* exs5(); */
    /* exs6(); */
    /* exs7(); */
    /* exs8(); */

    return 0;
}



static void exs1()
{
    pthread_t threadC;
    pthread_t threadP;
    char *buffer = NULL;
    int ret_val = 0;

    buffer = (char*)malloc(100);

    ret_val = pthread_create(&threadC, NULL, threadFunctionC, (void *)buffer);
    if (0 != ret_val)
    {
        RED;
        printf("failed to create , ret_val is %d\n", ret_val);
        DEFAULT;
    }

    ret_val = pthread_create(&threadP, NULL, threadFunctionP, (void *)buffer);
    if (0 != ret_val)
    {
        RED;
        printf("failed to create ,ret_val is %d\n", ret_val);
        DEFAULT;
    }

    pthread_join(threadC, NULL);

    free(buffer);
}


static void *threadFunctionC(void *buffer)
{
    while(1)
    {
        if(atomic_consumer_flag_g == 1)
        {
            atomic_consumer_flag_g = 0;
            GREEN;
            printf("--> %s ", (char*)buffer);
            DEFAULT;
            atomic_producer_flag_g = 1;
        }
    }
    return NULL;
}

static void *threadFunctionP(void *buffer)
{
    atomic_producer_flag_g = 1;

    while(1)
    {
        if(atomic_producer_flag_g == 1)
        {
            YELLOW;
            printf("Please write something to buffer\n");
            DEFAULT;
            atomic_producer_flag_g = 0;
            fgets(buffer, MAX_IN, stdin);
            atomic_consumer_flag_g = 1;
        }
    }
    return NULL;
}



static void exs2()
{
    pthread_t thread[NUM_OF_THREADS];
    int ret_val = 0;
    slist_t *slist = NULL;
    size_t i = 0;
    
    slist = SListCreate();
    if (NULL == slist)
    {
        return;
    }

    if (pthread_mutex_init(&lock, NULL) != 0) 
    { 
        printf("\n mutex init has failed\n"); 
        return ; 
    } 


    for(i=0; i<NUM_OF_THREADS; i+=2)
    {
        ret_val = pthread_create(&thread[i], NULL, removeNodeThread2, (void *)slist);
        if (0 != ret_val)
        {
            RED;
            printf("failed to create , ret_val is %d\n", ret_val);
            DEFAULT;
        }
        ret_val = pthread_create(&thread[i+1], NULL, insertNodeThread2, (void *)slist);
        if (0 != ret_val)
        {
            RED;
            printf("failed to create , ret_val is %d\n", ret_val);
            DEFAULT;
        }
    }


    for(i = 0 ; i < NUM_OF_THREADS; i++)
    {
        pthread_join(thread[i], NULL);
    }

    pthread_mutex_destroy(&lock);
}



static void *removeNodeThread2(void *list)
{
    while(1)
    {
        pthread_mutex_lock(&lock); 
        if(SListCount(list) > 0)
        {
            SListRemove(SListBegin(list));
            RED;
            printf("list count is %lu\n", SListCount(list)) ;
            DEFAULT;
        }
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}

static void *insertNodeThread2(void *list)
{
    int data = 55;

    while(1)
    {
        pthread_mutex_lock(&lock);
        SListInsertBefore(SListBegin(list) , &data);
        GREEN;
        printf("list count is %lu\n", SListCount(list)) ;
        DEFAULT;
        pthread_mutex_unlock(&lock);
    }

    return NULL;
}


static void exs3()
{
    pthread_t thread[NUM_OF_THREADS];
    int ret_val = 0;
    slist_t *slist = NULL;
    size_t i = 0;
    

    slist = SListCreate();
    if (NULL == slist)
    {
        return;
    }

    if (pthread_mutex_init(&lock, NULL) != 0) 
    { 
        printf("\n mutex init has failed\n"); 
        return ; 
    } 

    sem_g = sem_open("/sem_name3", O_CREAT , 0666, 0);
    if(sem_g == SEM_FAILED)
    {
        perror("Failed to create semaphore\n");
    }

    for(i=0; i<NUM_OF_THREADS; i+=2)
    {
        ret_val = pthread_create(&thread[i], NULL, removeNodeThread3, (void *)slist);
        if (0 != ret_val)
        {
            RED;
            printf("failed to create , ret_val is %d\n", ret_val);
            DEFAULT;
        }
        ret_val = pthread_create(&thread[i+1], NULL, insertNodeThread3, (void *)slist);
        if (0 != ret_val)
        {
            RED;
            printf("failed to create , ret_val is %d\n", ret_val);
            DEFAULT;
        }
    }


    for(i = 0 ; i < NUM_OF_THREADS; i++)
    {
        pthread_join(thread[i], NULL);
    }

    pthread_mutex_destroy(&lock);
}



static void *removeNodeThread3(void *list)
{
    int sval = 0;

    while(1)
    {
        sem_wait(sem_g);
        pthread_mutex_lock(&lock); 
        
        sem_getvalue(sem_g, &sval);
        GREEN;
        printf("value of semaphore is %d\n",sval);
        DEFAULT;
  
        SListRemove(SListBegin(list));
        RED;
        printf("list count is %lu\n", SListCount(list)) ;
        DEFAULT;
        
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}

static void *insertNodeThread3(void *list)
{
    int data = 55;
    int sval = 0;

    while(1)
    {
        pthread_mutex_lock(&lock);
        sem_post(sem_g);

        sem_getvalue(sem_g, &sval);
        BLUE;
        printf("value of semaphore is %d\n",sval);
        DEFAULT;

        SListInsertBefore(SListBegin(list) , &data);
        GREEN;
        printf("list count is %lu\n", SListCount(list)) ;
        DEFAULT;

        pthread_mutex_unlock(&lock);
    }

    return NULL;
}



static void exs4()
{
    pthread_t thread[NUM_OF_THREADS];
    int ret_val = 0;
    queue_t *queue = NULL;
    size_t i = 0;

    queue = QCreate();
    if(queue == NULL)
    {
        return;
    }

    if (pthread_mutex_init(&lock, NULL) != 0) 
    { 
        printf("\n mutex init has failed\n"); 
        return ; 
    } 

    sem_g = sem_open("/sem1_FS", O_CREAT , 0666, MAX_IN);
    if(sem_g == SEM_FAILED)
    {
        perror("Failed to create semaphore\n");
    }

    sem_init(sem_g, 1, MAX_IN);

    sem2_g = sem_open("/sem2", O_CREAT , 0666, 0);
    if(sem2_g == SEM_FAILED)
    {
        perror("Failed to create semaphore\n");
    }

    sem_init(sem2_g, 1, 0);

    for(i=0; i<NUM_OF_THREADS; i+=2)
    {
        ret_val = pthread_create(&thread[i], NULL, DequeueThread4, (void *)queue);
        if (0 != ret_val)
        {
            RED;
            printf("failed to create , ret_val is %d\n", ret_val);
            DEFAULT;
        }
        ret_val = pthread_create(&thread[i+1], NULL, EnqueueThread4, (void *)queue);
        if (0 != ret_val)
        {
            RED;
            printf("failed to create , ret_val is %d\n", ret_val);
            DEFAULT;
        }
    }


    for(i = 0 ; i < NUM_OF_THREADS; i++)
    {
        pthread_join(thread[i], NULL);
    }

    pthread_mutex_destroy(&lock); 
}


static void *EnqueueThread4(void *queue)
{
    int sval = 0;
    int data = 55;

    while(1)
    {
        sem_wait(sem_g);
        pthread_mutex_lock(&lock); 
        
        sem_getvalue(sem_g, &sval);
        GREEN;
        printf("value of semaphore 1 is %d\n",sval);
        DEFAULT;
  
        QEnqueue(queue, &data);
        YELLOW;
        printf("queue count is %lu\n", QCount(queue)) ;
        DEFAULT;
        
        sem_post(sem2_g);
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}


static void *DequeueThread4(void *queue)
{
    int sval = 0;

    while(1)
    {
        sem_wait(sem2_g);
        pthread_mutex_lock(&lock);

        sem_getvalue(sem2_g, &sval);
        BLUE;
        printf("value of semaphore 2 is %d\n",sval);
        DEFAULT;

        Qdequeue(queue);
        RED;
        printf("queue count is %lu\n", QCount(queue)) ;
        DEFAULT;

        sem_post(sem_g);
        pthread_mutex_unlock(&lock);
    }

    return NULL;
}



static void exs5()
{
    pthread_t thread[NUM_OF_THREADS];
    int ret_val = 0;
    cb_t *cbuff = NULL;
    size_t i = 0;
    int capacity = MAX_IN;

    cbuff = CBCreate(capacity);
    if(cbuff == NULL)
    {
        return;
    }

    if(pthread_mutex_init(&lock, NULL) != 0) 
    { 
        printf("\n mutex init has failed\n"); 
        return ; 
    } 

    sem_g = sem_open("/sem1_FS", O_CREAT , 0666, MAX_IN);
    if(sem_g == SEM_FAILED)
    {
        perror("Failed to create semaphore\n");
    }

    sem_init(sem_g, 1, MAX_IN);

    sem2_g = sem_open("/sem2", O_CREAT , 0666, 0);
    if(sem2_g == SEM_FAILED)
    {
        perror("Failed to create semaphore\n");
    }

    sem_init(sem2_g, 1, 0);

    for(i=0; i<NUM_OF_THREADS; i+=2)
    {
        ret_val = pthread_create(&thread[i], NULL, ReadThread5, (void *)cbuff);
        if (0 != ret_val)
        {
            RED;
            printf("failed to create , ret_val is %d\n", ret_val);
            DEFAULT;
        }
        ret_val = pthread_create(&thread[i+1], NULL, WriteThread5, (void *)cbuff);
        if (0 != ret_val)
        {
            RED;
            printf("failed to create , ret_val is %d\n", ret_val);
            DEFAULT;
        }
    }


    for(i = 0 ; i < NUM_OF_THREADS; i++)
    {
        pthread_join(thread[i], NULL);
    }

    pthread_mutex_destroy(&lock); 
}


static void *WriteThread5(void *cbuff)
{
    int sval = 0;
    int data = 55;
    size_t count = 1;
    char temp_buff = 'W';


    while(1)
    {
        pthread_mutex_lock(&lock); 
        sem_wait(sem_g);
        
        sem_getvalue(sem_g, &sval);
        GREEN;
        printf("value of semaphore 1 is %d\n",sval);
        DEFAULT;
  
        CBWrite(cbuff, &temp_buff, count);

        sem_post(sem2_g);
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}


static void *ReadThread5(void *cbuff)
{
    int sval = 0;
    size_t count = 1;
    char temp_buff = 'R';

    while(1)
    {
        pthread_mutex_lock(&lock2);
        sem_wait(sem2_g);

        sem_getvalue(sem2_g, &sval);
        BLUE;
        printf("value of semaphore 2 is %d\n",sval);
        DEFAULT;

        CBRead(cbuff, &temp_buff, count);
        RED;
        printf("buffer input is %c\n", temp_buff);
        DEFAULT;
        BLUE;
        printf("buffer size is %lu\n", CBFreeSpace(cbuff)) ;
        DEFAULT;


        sem_post(sem_g);
        pthread_mutex_unlock(&lock2);
    }

    return NULL;
}




static void exs6()
{
    pthread_t thread[NUM_OF_THREADS];
    int ret_val = 0;
    queue_t *queue = NULL;
    size_t i = 0;
    atomic_producer_flag_g = 0;
    atomic_consumer_flag_g = 0;

    if(pthread_mutex_init(&lock, NULL) != 0) 
    { 
        printf("\n mutex init has failed\n"); 
        return ; 
    } 

    queue = QCreate();
    if(queue == NULL)
    {
        return;
    }

    for(i=0; i<NUM_OF_THREADS; i+=2)
    {
        ret_val = pthread_create(&thread[i], NULL, DequeueThread6, (void *)queue);
        if (0 != ret_val)
        {
            RED;
            printf("failed to create , ret_val is %d\n", ret_val);
            DEFAULT;
        }
        ret_val = pthread_create(&thread[i+1], NULL, EnqueueThread6, (void *)queue);
        if (0 != ret_val)
        {
            RED;
            printf("failed to create , ret_val is %d\n", ret_val);
            DEFAULT;
        }
    }


    for(i = 0 ; i < NUM_OF_THREADS; i++)
    {
        pthread_join(thread[i], NULL);
    }

    pthread_mutex_destroy(&lock); 
}



static void *EnqueueThread6(void *queue)
{
    int data = 55;

    while(1)
    {
        pthread_mutex_lock(&lock); 
        if(atomic_producer_flag_g < MAX_IN)
        {
    
            QEnqueue(queue, &data);
            YELLOW;
            printf("queue count is %lu\n", QCount(queue)) ;
            DEFAULT;
            atomic_producer_flag_g++;
            atomic_consumer_flag_g++;
        }
        else
        {
            Qdequeue(queue);
            QEnqueue(queue, &data);
        }
        
        pthread_mutex_unlock(&lock);
        
    }
    return NULL;
}


static void *DequeueThread6(void *queue)
{
    int sval = 0;

    while(1)
    {
        pthread_mutex_lock(&lock);
        if(atomic_consumer_flag_g > 0)
        {

            Qdequeue(queue);

            RED;
            printf("queue count is %lu\n", QCount(queue)) ;
            DEFAULT;
            atomic_consumer_flag_g--;
            atomic_producer_flag_g--;
        }
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}





static void exs7()
{
    pthread_t thread[NUM_OF_THREADS];
    int ret_val = 0;
    cb_t *cbuff = NULL;
    size_t i = 0;
    int capacity = MAX_IN;

    cbuff = CBCreate(capacity);

    if(pthread_mutex_init(&lock, NULL) != 0) 
    { 
        printf("\n mutex init has failed\n"); 
        return ; 
    }


    for(i=0; i<NUM_OF_THREADS; i+=2)
    {
        ret_val = pthread_create(&thread[i], NULL, ReadThread7, (void *)cbuff);
        if (0 != ret_val)
        {
            RED;
            printf("failed to create , ret_val is %d\n", ret_val);
            DEFAULT;
        }
        ret_val = pthread_create(&thread[i+1], NULL, WriteThread7, (void *)cbuff);
        if (0 != ret_val)
        {
            RED;
            printf("failed to create , ret_val is %d\n", ret_val);
            DEFAULT;
        }
    }


    for(i = 0 ; i < NUM_OF_THREADS; i++)
    {
        pthread_join(thread[i], NULL);
    }

    pthread_mutex_destroy(&lock); 
}



static void *WriteThread7(void *cbuff)
{
    int sval = 0;
    int data = 55;
    size_t count = 1;
    char temp_buff = 'W';


    while(1)
    {
        pthread_mutex_lock(&lock); 
        
        CBWrite(cbuff, &temp_buff, count);
        GREEN;
        printf("buffer size from Write is %lu\n", CBFreeSpace(cbuff)) ;
        DEFAULT;
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}


static void *ReadThread7(void *cbuff)
{
    int sval = 0;
    size_t count = 1;
    char temp_buff = 'R';

    while(1)
    {
        pthread_mutex_lock(&lock2);
        if(!CBIsEmpty(cbuff))
        {
            CBRead(cbuff, &temp_buff, count);
            RED;
            printf("buffer input is %c\n", temp_buff);
            DEFAULT;
            BLUE;
            printf("buffer size is from read %lu\n", CBFreeSpace(cbuff)) ;
            DEFAULT;
        }

        pthread_mutex_unlock(&lock2);
    }

    return NULL;
}



static void exs8()
{
    pthread_t thread[NUM_OF_THREADS];
    int ret_val = 0;
    size_t i = 0;
    int capacity = MAX_IN;
    char *buffer = NULL;

    buffer = (char*)malloc(100);

    if(pthread_mutex_init(&lock, NULL) != 0) 
    {
        printf("\n mutex init has failed\n"); 
        return ; 
    }

    sem_g = sem_open("/sem", O_CREAT , 0666, 0);
    if(sem_g == SEM_FAILED)
    {
        perror("Failed to create semaphore\n");
    }

    sem_init(sem_g, 1, 0);

    for(i=0; i<NUM_OF_THREADS - 1; i++)
    {
        ret_val = pthread_create(&thread[i], NULL, ReadThread8, (void *)buffer);
        if (0 != ret_val)
        {
            RED;
            printf("failed to create , ret_val is %d\n", ret_val);
            DEFAULT;
        }
    }
    ret_val = pthread_create(&thread[i], NULL, WriteThread8, (void *)buffer);
    if (0 != ret_val)
    {
        RED;
        printf("failed to create , ret_val is %d\n", ret_val);
        DEFAULT;
    }
    

    for(i = 0 ; i < NUM_OF_THREADS; i++)
    {
        pthread_join(thread[i], NULL);
    }

    pthread_mutex_destroy(&lock); 
}



static void *WriteThread8(void *buffer)
{
    size_t i = 0;

    while(1)
    {
        for(i = 0; i < NUM_OF_THREADS - 1; i++)
        {
            sem_wait(sem_g);
        }

        YELLOW;
        printf("Please write something to buffer\n");
        DEFAULT;

        pthread_mutex_lock(&lock); 
        fgets(buffer, MAX_IN, stdin);

        pthread_cond_broadcast(&cond);
        pthread_mutex_unlock(&lock);

        for(i = 0; i < NUM_OF_THREADS -1; i++)
        {
            sem_wait(sem_g);
        }

    }

    return NULL;
}


static void *ReadThread8(void *buffer)
{
    while(1)
    {

        pthread_mutex_lock(&lock);
        sem_post(sem_g);

        pthread_cond_wait(&cond, &lock);
        GREEN;
        printf("--> I am reading: %s ", (char*)buffer);
        DEFAULT;

        sem_post(sem_g);
        pthread_mutex_unlock(&lock);
    }

    return NULL;
}

