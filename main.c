#include<stdio.h>
#include<pthread.h>
#include "bst.h"
#define NUMTHREADS 65536

typedef struct op_code
{
    int opcode;
    int value;
}opcode_t;

typedef struct thread_info
{
    pthread_t thread;
    int threadIdx;
}thread_t;

node_t* g_root = NULL;
thread_t threads[NUMTHREADS];
pthread_mutex_t tree_lock;
int threadIdx = 0;
int search_retval = 0;


void* worker(void* work_args)
{
    opcode_t* work = (opcode_t*)work_args;
    int code = work->opcode, data = work->value;

    if(code==1)
        insert(data, g_root, NULL, threadIdx);
    else if(code==2)
        del(data, g_root, threadIdx);
    else if(code==3)
        search(data, g_root, NULL);
    else
        traverse(g_root);
    pthread_exit((void*)0);
}


int main()
{
    opcode_t work;
    int ret = 0;
    pthread_mutex_init(&tree_lock, NULL);
    int ch, data, t=0;
    do
    {
        scanf("%d ", &ch);
        switch(ch)
        {
            case 1:
                scanf("%d", &data);
                //printf("Inserting %d\n", data);
                t=0;
                while(t < NUMTHREADS)
                {
                    ret = pthread_join(threads[t].thread, NULL);
                    t++;
                }
                work.opcode = 1;
                work.value = data;
                ret = pthread_create(&threads[threadIdx].thread, NULL, worker, (void*)&work);
                if(ret != 0)
                    printf("Failed to create thread %d\n", threadIdx);
                threadIdx++;
                break;
            case 2:
                scanf("%d", &data);
                //printf("Deleting %d\n", data);
                t=0;
                while(t < NUMTHREADS)
                {
                    ret = pthread_join(threads[t].thread, NULL);
                    t++;
                }
                work.opcode = 2;
                work.value = data;
                ret = pthread_create(&threads[threadIdx].thread, NULL, worker, (void*)&work);
                if(ret != 0)
                    printf("Failed to create thread %d\n", threadIdx);
                threadIdx++;
                break;
            case 3:
                scanf("%d", &data);
                //printf("Searching %d\n", data);
                t=0;
                while(t < NUMTHREADS)
                {
                    ret = pthread_join(threads[t].thread, NULL);
                    t++;
                }
                threadIdx = 0;
                work.opcode = 3;
                work.value = data;
                ret = pthread_create(&threads[threadIdx].thread, NULL, worker, (void*)&work);
                if(ret != 0)
                    printf("Failed to create thread %d\n", threadIdx);
                threadIdx++;
                break;
            case 4:
                //scanf("%d", &data);
                threadIdx = 0;
                while(threadIdx < NUMTHREADS)
                {
                    ret = pthread_join(threads[threadIdx].thread, NULL);
                    threadIdx++;
                }
                work.opcode = 4;
                work.value = data;
                ret = pthread_create(&threads[threadIdx].thread, NULL, worker, (void*)&work);
                if(ret != 0)
                    printf("Failed to create thread %d\n", threadIdx);
                printf("\n");
                threadIdx++;
                break;
            default:
                break;
        }
    } while (ch>=1 && ch<=4);
    threadIdx = 0;
    while(threadIdx < NUMTHREADS)
    {
        ret = pthread_join(threads[threadIdx].thread, NULL);
        threadIdx++;
    }
    pthread_mutex_destroy(&tree_lock);
    printf("\n");
    return 0;
}