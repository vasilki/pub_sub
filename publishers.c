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


static T_SHMEM GL_SHMEM_PUBLISHERS[] =
{
    {"MUTEX_PUB1", "SHMEM_PUB1", 0, { 0, 0, 0, 0, 20 + 3*sizeof(sem_t), 0}}
};
unsigned int GL_SHMEM_PUBLISHERS_NUMBER = sizeof(GL_SHMEM_PUBLISHERS)/sizeof(GL_SHMEM_PUBLISHERS[0]);


void publisher_init()
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

    return;
}


void publisher_thread(void)
{
    unsigned int loc_count;
    unsigned int loc_takt = 0;
    char loc_buff[20];

    publisher_init();

    while(1)
    {
        for(loc_count = 0; loc_count < GL_BLOCKING_SHMEM_PUBLISHERS_NUMBER; loc_count++)
        {
            memset(loc_buff,0,sizeof(loc_buff));
            sprintf(loc_buff,"PUB:%d",loc_takt);
            sem_wait(GL_SHMEM_PUBLISHERS[loc_count].shmem.received);
            sem_wait(GL_SHMEM_PUBLISHERS[loc_count].shmem.mutex);
            memcpy(GL_SHMEM_PUBLISHERS[loc_count].shmem.shmem_data,loc_buff,strnlen(loc_buff,GL_SHMEM_PUBLISHERS[loc_count].shmem.shmem_data_size));
            sem_post(GL_SHMEM_PUBLISHERS[loc_count].shmem.mutex);
            sem_post(GL_SHMEM_PUBLISHERS[loc_count].shmem.published);
        }
        loc_takt = (loc_takt + 1)%100;
    }

    return;
}
