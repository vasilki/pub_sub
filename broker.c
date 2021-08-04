#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
//#include <semaphore.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include "shmem_pub_sub.h"
#include <string.h>


T_SHMEM_TEMP_BUFFER GL_SHMEM_TEMP_BUFFER[K_MAX_SUBSCRIBERS];

void broker_init_temp_buffers()
{
    unsigned int loc_count;

    for(loc_count = 0; loc_count < K_MAX_SUBSCRIBERS; loc_count++)
    {
        memcpy(GL_SHMEM_TEMP_BUFFER[loc_count].shmem_name_sub, GL_BLOCKING_SHMEM_SUBSCRIBERS[loc_count].shmem_name,sizeof(GL_SHMEM_TEMP_BUFFER[loc_count].shmem_name_sub));
        GL_SHMEM_TEMP_BUFFER[loc_count].shmem_data = malloc(GL_BLOCKING_SHMEM_SUBSCRIBERS[loc_count].shmem.shmem_data_size);
        GL_SHMEM_TEMP_BUFFER[loc_count].shmem_data_size = GL_BLOCKING_SHMEM_SUBSCRIBERS[loc_count].shmem.shmem_data_size;
    }

    return;
}


void broker_init()
{
    unsigned int loc_count;
    int loc_fd;

    broker_init_temp_buffers();

    for(loc_count = 0; loc_count < GL_BLOCKING_SHMEM_PUBLISHERS_NUMBER; loc_count++)
    {
        /*check if shmem exists*/
        shm_unlink(GL_BLOCKING_SHMEM_PUBLISHERS[loc_count].shmem_name);

        loc_fd = shm_open(GL_BLOCKING_SHMEM_PUBLISHERS[loc_count].shmem_name, K_SHMEM_FLAG, K_SHMEM_MODE);

        ftruncate(loc_fd, GL_BLOCKING_SHMEM_PUBLISHERS[loc_count].shmem.shmem_size);
        GL_BLOCKING_SHMEM_PUBLISHERS[loc_count].shmem_mmap =
                mmap(NULL, GL_BLOCKING_SHMEM_PUBLISHERS[loc_count].shmem.shmem_size, K_MMAP_FLAG, MAP_SHARED,
                     loc_fd, 0);

        close(loc_fd);

        GL_BLOCKING_SHMEM_PUBLISHERS[loc_count].shmem.mutex = (sem_t *)GL_BLOCKING_SHMEM_PUBLISHERS[loc_count].shmem_mmap;
        GL_BLOCKING_SHMEM_PUBLISHERS[loc_count].shmem.published = (sem_t *)(GL_BLOCKING_SHMEM_PUBLISHERS[loc_count].shmem_mmap + sizeof(sem_t));
        GL_BLOCKING_SHMEM_PUBLISHERS[loc_count].shmem.received = (sem_t *)(GL_BLOCKING_SHMEM_PUBLISHERS[loc_count].shmem_mmap + 2 * sizeof(sem_t));
        sem_init(GL_BLOCKING_SHMEM_PUBLISHERS[loc_count].shmem.mutex, 1, 1);
        sem_init(GL_BLOCKING_SHMEM_PUBLISHERS[loc_count].shmem.published, 1, 0);
        sem_init(GL_BLOCKING_SHMEM_PUBLISHERS[loc_count].shmem.received, 1, 1);

        GL_BLOCKING_SHMEM_PUBLISHERS[loc_count].shmem.shmem_data = GL_BLOCKING_SHMEM_PUBLISHERS[loc_count].shmem_mmap + 3 * sizeof(sem_t);
        GL_BLOCKING_SHMEM_PUBLISHERS[loc_count].shmem.shmem_data_size = GL_BLOCKING_SHMEM_PUBLISHERS[loc_count].shmem.shmem_size - 3 * sizeof(sem_t);
    }

    for(loc_count = 0; loc_count < GL_BLOCKING_SHMEM_SUBSCRIBERS_NUMBER; loc_count++)
    {
        /*check if shmem exists*/
        shm_unlink(GL_BLOCKING_SHMEM_SUBSCRIBERS[loc_count].shmem_name);

        loc_fd = shm_open(GL_BLOCKING_SHMEM_SUBSCRIBERS[loc_count].shmem_name, K_SHMEM_FLAG, K_SHMEM_MODE);

        ftruncate(loc_fd, GL_BLOCKING_SHMEM_SUBSCRIBERS[loc_count].shmem.shmem_size);
        GL_BLOCKING_SHMEM_SUBSCRIBERS[loc_count].shmem_mmap =
                mmap(NULL, GL_BLOCKING_SHMEM_SUBSCRIBERS[loc_count].shmem.shmem_size, K_MMAP_FLAG, MAP_SHARED,
                     loc_fd, 0);

        close(loc_fd);

        GL_BLOCKING_SHMEM_SUBSCRIBERS[loc_count].shmem.mutex = (sem_t *)GL_BLOCKING_SHMEM_SUBSCRIBERS[loc_count].shmem_mmap;
        GL_BLOCKING_SHMEM_SUBSCRIBERS[loc_count].shmem.published = (sem_t *)(GL_BLOCKING_SHMEM_SUBSCRIBERS[loc_count].shmem_mmap + sizeof(sem_t));
        GL_BLOCKING_SHMEM_SUBSCRIBERS[loc_count].shmem.received = (sem_t *)(GL_BLOCKING_SHMEM_SUBSCRIBERS[loc_count].shmem_mmap + 2 * sizeof(sem_t));
        sem_init(GL_BLOCKING_SHMEM_SUBSCRIBERS[loc_count].shmem.mutex, 1, 1);
        sem_init(GL_BLOCKING_SHMEM_SUBSCRIBERS[loc_count].shmem.published, 1, 0);
        sem_init(GL_BLOCKING_SHMEM_SUBSCRIBERS[loc_count].shmem.received, 1, 1);

        GL_BLOCKING_SHMEM_SUBSCRIBERS[loc_count].shmem.shmem_data = GL_BLOCKING_SHMEM_SUBSCRIBERS[loc_count].shmem_mmap + 3 * sizeof(sem_t);
        GL_BLOCKING_SHMEM_SUBSCRIBERS[loc_count].shmem.shmem_data_size = GL_BLOCKING_SHMEM_SUBSCRIBERS[loc_count].shmem.shmem_size - 3 * sizeof(sem_t);
    }


    return;
}

void broker_exit(void)
{
    unsigned int loc_count;


    for(loc_count = 0; loc_count < GL_BLOCKING_SHMEM_PUBLISHERS_NUMBER; loc_count++)
    {
        /*check if shmem exists*/
        shm_unlink(GL_BLOCKING_SHMEM_PUBLISHERS[loc_count].shmem_name);
    }

    return;
}


void broker_thread(void)
{
    unsigned int loc_count;
    unsigned int loc_takt = 0;
  //  char loc_buff[20];


    while(loc_takt < 200)
    {
        for(loc_count = 0; loc_count < GL_BLOCKING_SHMEM_PUBLISHERS_NUMBER; loc_count++)
        {
            if(sem_trywait(GL_BLOCKING_SHMEM_PUBLISHERS[loc_count].shmem.published) == 0)
            {

                sem_wait(GL_BLOCKING_SHMEM_PUBLISHERS[loc_count].shmem.mutex);
    //            memcpy(loc_buff,GL_BLOCKING_SHMEM_PUBLISHERS[loc_count].shmem.shmem_data,GL_BLOCKING_SHMEM_PUBLISHERS[loc_count].shmem.shmem_data_size);
                if(loc_count == 0)
                {
                    /*only for data from publisher.c*/
                    memcpy(GL_SHMEM_TEMP_BUFFER[loc_count].shmem_data,GL_BLOCKING_SHMEM_PUBLISHERS[loc_count].shmem.shmem_data,GL_BLOCKING_SHMEM_PUBLISHERS[loc_count].shmem.shmem_data_size);
                }
                printf("<BRO received message:%s\n",(char*)GL_SHMEM_TEMP_BUFFER[loc_count].shmem_data);
                sem_post(GL_BLOCKING_SHMEM_PUBLISHERS[loc_count].shmem.mutex);
                sem_post(GL_BLOCKING_SHMEM_PUBLISHERS[loc_count].shmem.received);
            }
        }

        for(loc_count = 0; loc_count < GL_BLOCKING_SHMEM_SUBSCRIBERS_NUMBER; loc_count++)
        {
            if(sem_wait(GL_BLOCKING_SHMEM_SUBSCRIBERS[loc_count].shmem.received) == 0)
            {
                sem_wait(GL_BLOCKING_SHMEM_SUBSCRIBERS[loc_count].shmem.mutex);
                memcpy(GL_BLOCKING_SHMEM_SUBSCRIBERS[loc_count].shmem.shmem_data,GL_SHMEM_TEMP_BUFFER[loc_count].shmem_data,GL_BLOCKING_SHMEM_SUBSCRIBERS[loc_count].shmem.shmem_data_size);
                printf(">BRO sent message:%s\n",(char*)GL_SHMEM_TEMP_BUFFER[loc_count].shmem_data);
                sem_post(GL_BLOCKING_SHMEM_SUBSCRIBERS[loc_count].shmem.mutex);
                sem_post(GL_BLOCKING_SHMEM_SUBSCRIBERS[loc_count].shmem.published);
            }
        }



        loc_takt++;
    }

    return;
}





/*
   S_IRWXU
   (00700 пользователь (владелец файла) имеет права на чтение, запись и выполнение
    файла);
   S_IRUSR (S_IREAD)
   (00400 пользователь имеет права на чтение файла);
   S_IWUSR (S_IWRITE)
   (00200 пользователь имеет права на запись информации в файл);
   S_IXUSR (S_IEXEC)
   (00100 пользователь имеет права на выполнение файла);
   S_IRWXG
   (00070 группа имеет права на чтение, выполнение файла и запись в него информации);
   S_IRGRP
   (00040 группа имеет права на чтение файла);
   S_IWGRP
   (00020 группа имеет права на запись информации в файл);
   S_IXGRP
   (00010 группа имеет права на выполнение
    файла);
   S_IRWXO
   (00007 все остальные имеют права на чтение, выполнение файла и запись в него информации);
   S_IROTH
   (00004 все остальные имеют права на чтение файла);
   S_IWOTH
   (00002 все остальные имеют права на запись информации в файл);
   S_IXOTH
   (00001 все остальные имеют права на выполнение
    файла).
*/
