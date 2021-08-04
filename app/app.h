#ifndef __APP_H__
#define __APP_H__
#include <semaphore.h>
#include "shmem_pub_sub.h"

typedef struct
{
    sem_t mutex;
    unsigned char buffer_pub[K_DATA_SIZE];
    unsigned char buffer_sub[K_DATA_SIZE];
    unsigned int size_pub;
    unsigned int size_sub;
}T_SHARED_BUFFER;

extern T_SHARED_BUFFER GL_SHARED_BUFFER;



#endif /*__APP_H__*/
