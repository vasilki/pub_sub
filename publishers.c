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
    {"SHMEM_PUB1", 0}
};
static unsigned int GL_SHMEM_PUBLISHERS_NUMBER = sizeof(GL_SHMEM_PUBLISHERS)/sizeof(GL_SHMEM_PUBLISHERS[0]);


void publisher_init()
{
    unsigned int loc_count;
    int loc_fd;

    for(loc_count = 0; loc_count < GL_SHMEM_PUBLISHERS_NUMBER; loc_count++)
    {

        loc_fd = shm_open(GL_SHMEM_PUBLISHERS[loc_count].shmem_name, O_RDWR, K_SHMEM_MODE);

        GL_SHMEM_PUBLISHERS[loc_count].shmem =
                mmap(NULL, sizeof(T_BLOCKING_SHMEM), K_MMAP_FLAG, MAP_SHARED, loc_fd, 0);

        close(loc_fd);

    }

    return;
}


void publisher_thread(void)
{
    unsigned int loc_count;
    unsigned int loc_pr = 0;
    unsigned int loc_takt = 0;
    char loc_buff[K_DATA_SIZE];

    while(loc_takt < 200)
    {
        for(loc_count = 0; loc_count < GL_SHMEM_PUBLISHERS_NUMBER; loc_count++)
        {
            memset(loc_buff,0,sizeof(loc_buff));
            sprintf(loc_buff,"PUB:%d",loc_pr);
            sem_wait(&GL_SHMEM_PUBLISHERS[loc_count].shmem->ready);
            sem_wait(&GL_SHMEM_PUBLISHERS[loc_count].shmem->mutex);
            GL_SHMEM_PUBLISHERS[loc_count].shmem->app_out_data_size = strnlen(loc_buff,K_DATA_SIZE);
            memcpy(GL_SHMEM_PUBLISHERS[loc_count].shmem->app_out_data,loc_buff,
                   GL_SHMEM_PUBLISHERS[loc_count].shmem->app_out_data_size);
            printf(">PUB sent message PUB:%d\n",loc_pr);
            sem_post(&GL_SHMEM_PUBLISHERS[loc_count].shmem->mutex);
        }
        loc_pr = (loc_pr + 1)%100;
        loc_takt++;
    }

    return;
}
