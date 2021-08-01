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


void subscriber_init()
{
    unsigned int loc_count;
    int loc_fd;

    return;
}


void subscriber_thread(void)
{
    unsigned int loc_count;
    unsigned int loc_takt = 0;
    char loc_buff[20];

    subscriber_init();

    while(1)
    {
        for(loc_count = 0; loc_count < GL_BLOCKING_SHMEM_PUBLISHERS_NUMBER; loc_count++)
        {

        }
        loc_takt = (loc_takt + 1)%100;
    }

    return;
}
