#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>
#include "shmem_pub_sub.h"
#include <string.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>


static T_SHMEM GL_SHMEM_SUBSCRIBERS[] =
{
    {"SHMEM_SUB1", 0, { 0, 0, 0, 0, 20 + 3*sizeof(sem_t), 0}}
};
unsigned int GL_SHMEM_SUBSCRIBERS_NUMBER = sizeof(GL_SHMEM_SUBSCRIBERS)/sizeof(GL_SHMEM_SUBSCRIBERS[0]);



void subscriber_init()
{
    unsigned int loc_count;
    int loc_fd;

    for(loc_count = 0; loc_count < GL_SHMEM_SUBSCRIBERS_NUMBER; loc_count++)
    {

        loc_fd = shm_open(GL_SHMEM_SUBSCRIBERS[loc_count].shmem_name, O_RDWR, K_SHMEM_MODE);

        GL_SHMEM_SUBSCRIBERS[loc_count].shmem_mmap =
                mmap(NULL, GL_SHMEM_SUBSCRIBERS[loc_count].shmem.shmem_size, K_MMAP_FLAG, MAP_SHARED,
                     loc_fd, 0);

        close(loc_fd);

        GL_SHMEM_SUBSCRIBERS[loc_count].shmem.mutex = (sem_t *)GL_SHMEM_SUBSCRIBERS[loc_count].shmem_mmap;
        GL_SHMEM_SUBSCRIBERS[loc_count].shmem.published = (sem_t *)(GL_SHMEM_SUBSCRIBERS[loc_count].shmem_mmap + sizeof(sem_t));
        GL_SHMEM_SUBSCRIBERS[loc_count].shmem.received = (sem_t *)(GL_SHMEM_SUBSCRIBERS[loc_count].shmem_mmap + 2 * sizeof(sem_t));

        GL_SHMEM_SUBSCRIBERS[loc_count].shmem.shmem_data = GL_SHMEM_SUBSCRIBERS[loc_count].shmem_mmap + 3 * sizeof(sem_t);
        GL_SHMEM_SUBSCRIBERS[loc_count].shmem.shmem_data_size = GL_SHMEM_SUBSCRIBERS[loc_count].shmem.shmem_size - 3 * sizeof(sem_t);
    }

    return;
}


void subscriber_thread(void)
{
    unsigned int loc_count;
    unsigned int loc_takt = 0;
    char loc_buff[20];

    subscriber_init();

    while(loc_takt < 200)
    {
        for(loc_count = 0; loc_count < GL_BLOCKING_SHMEM_SUBSCRIBERS_NUMBER; loc_count++)
        {
            sem_wait(GL_BLOCKING_SHMEM_SUBSCRIBERS[loc_count].shmem.published);
            sem_wait(GL_BLOCKING_SHMEM_SUBSCRIBERS[loc_count].shmem.mutex);
            memcpy(loc_buff,GL_BLOCKING_SHMEM_SUBSCRIBERS[loc_count].shmem.shmem_data,GL_BLOCKING_SHMEM_SUBSCRIBERS[loc_count].shmem.shmem_data_size);
            sem_post(GL_BLOCKING_SHMEM_SUBSCRIBERS[loc_count].shmem.mutex);
            sem_post(GL_BLOCKING_SHMEM_SUBSCRIBERS[loc_count].shmem.received);
            printf("SUB received message:%s\n",(char*)loc_buff);
        }


        loc_takt++;
    }

    return;
}
