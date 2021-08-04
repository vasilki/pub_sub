#ifndef __SHMEM_PUB_SUB__
#define __SHMEM_PUB_SUB__
#include <semaphore.h>

#define K_MAX_PUBLISHERS 1
#define K_MAX_SUBSCRIBERS 1
#define K_DATA_SIZE 20

#define K_SHMEM_FLAG (O_RDWR | O_CREAT | O_EXCL)
#define K_SHMEM_MODE (S_IRWXU | S_IRWXG | S_IRWXO)
#define K_MMAP_FLAG (PROT_READ | PROT_WRITE)

typedef struct
{
    sem_t mutex;
    sem_t ready;
    char app_in_data[K_DATA_SIZE];
    char app_out_data[K_DATA_SIZE];
    unsigned int app_in_data_size;
    unsigned int app_out_data_size;
}T_BLOCKING_SHMEM;

typedef struct
{
    char shmem_name[32];
    T_BLOCKING_SHMEM *shmem;
}T_SHMEM;



extern T_SHMEM GL_BLOCKING_SHMEM_PUBLISHERS[];
extern T_SHMEM GL_BLOCKING_SHMEM_SUBSCRIBERS[];
extern unsigned int GL_BLOCKING_SHMEM_PUBLISHERS_NUMBER;
extern unsigned int GL_BLOCKING_SHMEM_SUBSCRIBERS_NUMBER;



#endif /*__SHMEM_PUB_SUB__*/
