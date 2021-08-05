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


static T_SHMEM GL_SHMEM_APP[] =
{
    {"SHMEM_APP", 0}
};
static unsigned int GL_SHMEM_APP_NUMBER = sizeof(GL_SHMEM_APP)/sizeof(GL_SHMEM_APP[0]);

T_SHMEM GL_SHARED_BUFFER[2];
unsigned int GL_SHARED_BUFFERS_NUMBER = sizeof(GL_SHARED_BUFFER)/sizeof(GL_SHARED_BUFFER[0]);


void port_shmem_init()
{
    unsigned int loc_count;
    int loc_fd;

    for(loc_count = 0; loc_count < GL_SHARED_BUFFERS_NUMBER; loc_count++)
    {
        GL_SHARED_BUFFER[loc_count].shmem = malloc(sizeof(T_BLOCKING_SHMEM));
        memset(GL_SHARED_BUFFER[loc_count].shmem,0,sizeof(T_BLOCKING_SHMEM));
        sem_init(&GL_SHARED_BUFFER[loc_count].shmem->mutex, 1, 1);
    }

    for(loc_count = 0; loc_count < GL_SHMEM_APP_NUMBER; loc_count++)
    {

        loc_fd = shm_open(GL_SHMEM_APP[loc_count].shmem_name, O_RDWR, K_SHMEM_MODE);

        GL_SHMEM_APP[loc_count].shmem =
                mmap(NULL, sizeof(T_BLOCKING_SHMEM), K_MMAP_FLAG, MAP_SHARED, loc_fd, 0);

        close(loc_fd);
    }

    return;
}


void port_thread(void)
{
    unsigned int loc_count;
    unsigned int loc_takt = 0;

    while(loc_takt < 200)
    {

        /*READ DATA FROM BROKER*/
        for(loc_count = 0; loc_count < GL_SHMEM_APP_NUMBER; loc_count++)
        {
            sem_wait(&GL_SHMEM_APP[loc_count].shmem->ready);
            sem_wait(&GL_SHMEM_APP[loc_count].shmem->mutex);

            /*Store data from broker to internal shared buffer*/
            memcpy(GL_SHARED_BUFFER[K_INDEX_PORT].shmem->app_in_data,
                   GL_SHMEM_APP[loc_count].shmem->app_in_data,
                   GL_SHMEM_APP[loc_count].shmem->app_in_data_size);
            GL_SHARED_BUFFER[K_INDEX_PORT].shmem->app_in_data_size = GL_SHMEM_APP[loc_count].shmem->app_in_data_size;
            printf(">PORT received message:%s\n",(char*)GL_SHARED_BUFFER[K_INDEX_PORT].shmem->app_in_data);

            /*Send data to broker from internal shared buffer*/
            memcpy(GL_SHMEM_APP[loc_count].shmem->app_out_data,
                   GL_SHARED_BUFFER[K_INDEX_PORT].shmem->app_out_data,
                   GL_SHARED_BUFFER[K_INDEX_PORT].shmem->app_out_data_size);
            GL_SHMEM_APP[loc_count].shmem->app_out_data_size = GL_SHARED_BUFFER[K_INDEX_PORT].shmem->app_out_data_size;
            sem_post(&GL_SHMEM_APP[loc_count].shmem->mutex);
        }

        sem_wait(&GL_SHARED_BUFFER[K_INDEX_CORE].shmem->mutex);


        memcpy(GL_SHARED_BUFFER[K_INDEX_CORE].shmem->app_in_data,
               GL_SHARED_BUFFER[K_INDEX_PORT].shmem->app_in_data,
               GL_SHARED_BUFFER[K_INDEX_PORT].shmem->app_in_data_size);
        GL_SHARED_BUFFER[K_INDEX_CORE].shmem->app_in_data_size = GL_SHARED_BUFFER[K_INDEX_PORT].shmem->app_in_data_size;

        memcpy(GL_SHARED_BUFFER[K_INDEX_PORT].shmem->app_out_data,
               GL_SHARED_BUFFER[K_INDEX_CORE].shmem->app_out_data,
               GL_SHARED_BUFFER[K_INDEX_CORE].shmem->app_out_data_size);
        GL_SHARED_BUFFER[K_INDEX_PORT].shmem->app_out_data_size = GL_SHARED_BUFFER[K_INDEX_CORE].shmem->app_out_data_size;


        sem_post(&GL_SHARED_BUFFER[K_INDEX_CORE].shmem->mutex);

        usleep(100);
        loc_takt++;


    }

    return;
}

extern void core_init();

void port_init()
{
    static pthread_t loc_thread_port;
    static pthread_attr_t loc_thread_attr;
    static unsigned char loc_attr[8];

    port_shmem_init();

    core_init();

    return;
}






