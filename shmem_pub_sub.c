#include "shmem_pub_sub.h"
#include <semaphore.h>


T_SHMEM GL_BLOCKING_SHMEM_PUBLISHERS[] =
{
    {"SHMEM_PUB1", 0, { 0, 0, 0, 0, 20 + 3*sizeof(sem_t), 0}}
};

unsigned int GL_BLOCKING_SHMEM_PUBLISHERS_NUMBER = sizeof(GL_BLOCKING_SHMEM_PUBLISHERS)/sizeof(GL_BLOCKING_SHMEM_PUBLISHERS[0]);

T_SHMEM GL_BLOCKING_SHMEM_SUBSCRIBERS[] =
{
    {"SHMEM_SUB1", 0, { 0, 0, 0, 0, 20 + 3*sizeof(sem_t), 0}}
};

unsigned int GL_BLOCKING_SHMEM_SUBSCRIBERS_NUMBER = sizeof(GL_BLOCKING_SHMEM_SUBSCRIBERS)/sizeof(GL_BLOCKING_SHMEM_SUBSCRIBERS[0]);

