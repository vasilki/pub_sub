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
#include "app.h"

static T_SHMEM GL_SHMEM_SUBSCRIBERS[] =
{
    {"SHMEM_APP_SUB1", 0, { 0, 0, 0, 0, K_DATA_SIZE + 3*sizeof(sem_t), 0}}
};
static unsigned int GL_SHMEM_SUBSCRIBERS_NUMBER = sizeof(GL_SHMEM_SUBSCRIBERS)/sizeof(GL_SHMEM_SUBSCRIBERS[0]);

static T_SHMEM GL_SHMEM_PUBLISHERS[] =
{
    {"SHMEM_APP_PUB1", 0, { 0, 0, 0, 0, K_DATA_SIZE + 3*sizeof(sem_t), 0}}
};
static unsigned int GL_SHMEM_PUBLISHERS_NUMBER = sizeof(GL_SHMEM_PUBLISHERS)/sizeof(GL_SHMEM_PUBLISHERS[0]);

T_SHARED_BUFFER GL_SHARED_BUFFER;
static unsigned char GL_TEMP_BUFFER_PUB[K_DATA_SIZE];
static unsigned char GL_TEMP_BUFFER_SUB[K_DATA_SIZE];

void port_shmem_init()
{
    unsigned int loc_count;
    int loc_fd;

    for(loc_count = 0; loc_count < GL_SHMEM_PUBLISHERS_NUMBER; loc_count++)
    {

        loc_fd = shm_open(GL_SHMEM_PUBLISHERS[loc_count].shmem_name, O_RDWR, K_SHMEM_MODE);

        GL_SHMEM_PUBLISHERS[loc_count].shmem_mmap =
                mmap(NULL, GL_SHMEM_PUBLISHERS[loc_count].shmem.shmem_size, K_MMAP_FLAG, MAP_SHARED,
                     loc_fd, 0);

        close(loc_fd);

        GL_SHMEM_PUBLISHERS[loc_count].shmem.mutex = (sem_t *)GL_SHMEM_PUBLISHERS[loc_count].shmem_mmap;
        GL_SHMEM_PUBLISHERS[loc_count].shmem.published = (sem_t *)(GL_SHMEM_PUBLISHERS[loc_count].shmem_mmap + sizeof(sem_t));
        GL_SHMEM_PUBLISHERS[loc_count].shmem.received = (sem_t *)(GL_SHMEM_PUBLISHERS[loc_count].shmem_mmap + 2 * sizeof(sem_t));

        GL_SHMEM_PUBLISHERS[loc_count].shmem.shmem_data = GL_SHMEM_PUBLISHERS[loc_count].shmem_mmap + 3 * sizeof(sem_t);
        GL_SHMEM_PUBLISHERS[loc_count].shmem.shmem_data_size = GL_SHMEM_PUBLISHERS[loc_count].shmem.shmem_size - 3 * sizeof(sem_t);
    }


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


void port_thread(void)
{
    unsigned int loc_count;
    unsigned int loc_takt = 0;
    char loc_buff[20];

    while(loc_takt < 200)
    {
        /*LOCK BLOCKING BUFFER PORT-CORE*/
        /*
        sem_wait(&GL_SHARED_BUFFER.mutex);
        memcpy(GL_TEMP_BUFFER_PUB,GL_SHARED_BUFFER.buffer_pub,GL_SHARED_BUFFER.size_pub);
        memcpy(GL_SHARED_BUFFER.buffer_sub,GL_TEMP_BUFFER_SUB,GL_SHARED_BUFFER.size_sub);
        sem_post(&GL_SHARED_BUFFER.mutex);
*/

        /*READ DATA FROM BROKER*/
        for(loc_count = 0; loc_count < GL_SHMEM_SUBSCRIBERS_NUMBER; loc_count++)
        {
            sem_wait(GL_SHMEM_SUBSCRIBERS[loc_count].shmem.published);
            sem_wait(GL_SHMEM_SUBSCRIBERS[loc_count].shmem.mutex);
            memcpy(GL_TEMP_BUFFER_SUB,GL_SHMEM_SUBSCRIBERS[loc_count].shmem.shmem_data,GL_SHMEM_SUBSCRIBERS[loc_count].shmem.shmem_data_size);
            printf(">SUB received message:%s\n",(char*)loc_buff);
            sem_post(GL_SHMEM_SUBSCRIBERS[loc_count].shmem.mutex);
            sem_post(GL_SHMEM_SUBSCRIBERS[loc_count].shmem.received);
        }

        /*SEND DATA TO BROKER*/
        for(loc_count = 0; loc_count < GL_SHMEM_PUBLISHERS_NUMBER; loc_count++)
        {
            sem_wait(GL_SHMEM_PUBLISHERS[loc_count].shmem.received);
            sem_wait(GL_SHMEM_PUBLISHERS[loc_count].shmem.mutex);
            memcpy(GL_SHMEM_PUBLISHERS[loc_count].shmem.shmem_data,GL_TEMP_BUFFER_PUB,strnlen(loc_buff,GL_SHMEM_PUBLISHERS[loc_count].shmem.shmem_data_size));
            printf(">PUB sent message PUB:\n");
            sem_post(GL_SHMEM_PUBLISHERS[loc_count].shmem.mutex);
            sem_post(GL_SHMEM_PUBLISHERS[loc_count].shmem.published);
        }
        loc_takt++;


    }

    return;
}


void port_init()
{
    static pthread_t loc_thread_port;
    static pthread_attr_t loc_thread_attr;
    static unsigned char loc_attr[8];

    port_shmem_init();
    sem_init(&GL_SHARED_BUFFER.mutex, 1, 1);

    pthread_attr_init(&loc_thread_attr);
    pthread_create(&loc_thread_port,&loc_thread_attr,(void*)port_thread,(void*)&loc_attr[0]);
    pthread_join(loc_thread_port,NULL);

    return;
}






