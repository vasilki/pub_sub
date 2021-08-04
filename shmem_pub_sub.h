#ifndef __SHMEM_PUB_SUB__
#define __SHMEM_PUB_SUB__
#include <semaphore.h>

#define K_MAX_PUBLISHERS 1
#define K_MAX_SUBSCRIBERS K_MAX_PUBLISHERS
#define K_DATA_SIZE 20

#define K_SHMEM_FLAG (O_RDWR | O_CREAT | O_EXCL)
#define K_SHMEM_MODE (S_IRWXU | S_IRWXG | S_IRWXO)
#define K_MMAP_FLAG (PROT_READ | PROT_WRITE)

typedef struct
{
    sem_t *mutex;
    sem_t *published;
    sem_t *received;
    void *shmem_data;
    const unsigned int shmem_size; /*include semaphores*/
    unsigned int shmem_data_size; /*pure data without semaphores*/
}T_BLOCKING_SHMEM;

typedef struct
{
    char shmem_name[32];
    void *shmem_mmap;
    T_BLOCKING_SHMEM shmem;
}T_SHMEM;

typedef struct
{
    char shmem_name_sub[32];
    void *shmem_data;
    unsigned int shmem_data_size;
}T_SHMEM_TEMP_BUFFER;



extern T_SHMEM GL_BLOCKING_SHMEM_PUBLISHERS[];
extern T_SHMEM GL_BLOCKING_SHMEM_SUBSCRIBERS[];
extern unsigned int GL_BLOCKING_SHMEM_PUBLISHERS_NUMBER;
extern unsigned int GL_BLOCKING_SHMEM_SUBSCRIBERS_NUMBER;



#endif /*__SHMEM_PUB_SUB__*/
