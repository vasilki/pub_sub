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
 /*       sem_wait(&GL_SHARED_BUFFER.mutex);
        printf("core received: %s\n",GL_SHARED_BUFFER.buffer_sub);
        snprintf((char*)GL_SHARED_BUFFER.buffer_pub,sizeof(GL_SHARED_BUFFER.buffer_pub),"CORE_%d",loc_takt);
        printf("core sent: %s\n",GL_SHARED_BUFFER.buffer_pub);
        sem_post(&GL_SHARED_BUFFER.mutex);
*/
        loc_takt++;
    }

    return;
}


void core_init()
{
    static pthread_t loc_thread_core;
    static pthread_attr_t loc_thread_attr;
    static unsigned char loc_attr[8];


    pthread_attr_init(&loc_thread_attr);
    pthread_create(&loc_thread_core,&loc_thread_attr,(void*)core_thread,(void*)&loc_attr[0]);
    pthread_join(loc_thread_core,NULL);

    return;
}






