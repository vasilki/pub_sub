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


void core_thread(void)
{
    unsigned int loc_takt = 0;

    while(loc_takt < 200)
    {
        /*LOCK BLOCKING BUFFER PORT-CORE*/

        sem_wait(&GL_SHARED_BUFFER[K_INDEX_CORE].shmem->mutex);
        printf("CORE received: %s\n",GL_SHARED_BUFFER[K_INDEX_CORE].shmem->app_in_data);
        sprintf((char*)GL_SHARED_BUFFER[K_INDEX_CORE].shmem->app_out_data,"CORE_%d",loc_takt);
        GL_SHARED_BUFFER[K_INDEX_CORE].shmem->app_out_data_size = strnlen(GL_SHARED_BUFFER[K_INDEX_CORE].shmem->app_out_data,K_DATA_SIZE);
        printf("core sent: %s\n",GL_SHARED_BUFFER[K_INDEX_CORE].shmem->app_out_data);
        sem_post(&GL_SHARED_BUFFER[K_INDEX_CORE].shmem->mutex);

        usleep(150);
        loc_takt++;
    }

    return;
}


void core_init()
{

    return;
}






