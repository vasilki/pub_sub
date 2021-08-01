#ifndef __SHMEM_PUB_SUB__
#define __SHMEM_PUB_SUB__
#include <semaphore.h>

#define K_SHMEM_FLAG (O_RDWR | O_CREAT | O_EXCL)
#define K_SHMEM_MODE (S_IRWXU | S_IRWXG | S_IRWXO)
#define K_MMAP_FLAG (PROT_READ | PROT_WRITE)

typedef struct
{
    sem_t *mutex;
    sem_t *published;
    sem_t *received;
    void *shmem_data;
    const unsigned int shmem_size;
    unsigned int shmem_data_size;
}T_BLOCKING_SHMEM;

typedef struct
{
    char mutex_name[32];
    char shmem_name[32];
    void *shmem_mmap;
    T_BLOCKING_SHMEM shmem;
}T_SHMEM;


extern T_SHMEM GL_BLOCKING_SHMEM_PUBLISHERS[];
extern unsigned int GL_BLOCKING_SHMEM_PUBLISHERS_NUMBER;
extern unsigned int GL_BLOCKING_SHMEM_SUBSCRIBERS_NUMBER;



#endif /*__SHMEM_PUB_SUB__*/
