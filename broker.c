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


T_SHMEM GL_SHMEM_TEMP[K_MAX_CLIENTS];
T_SHMEM GL_BLOCKING_SHMEM[] =
{
    {"SHMEM_PUB1", 0}, /*only publisher*/
    {"SHMEM_APP", 0} /*application*/
};

unsigned int GL_BLOCKING_SHMEM_NUMBER = sizeof(GL_BLOCKING_SHMEM)/sizeof(GL_BLOCKING_SHMEM[0]);



void broker_init_temp_buffers()
{
    unsigned int loc_count;

    for(loc_count = 0; loc_count < K_MAX_CLIENTS; loc_count++)
    {
        GL_SHMEM_TEMP[loc_count].shmem = malloc(sizeof(T_BLOCKING_SHMEM));
        memset(GL_SHMEM_TEMP[loc_count].shmem,0,sizeof(T_BLOCKING_SHMEM));
        sprintf(GL_SHMEM_TEMP[loc_count].shmem_name,"TEMP");
    }

    return;
}


void broker_init()
{
    unsigned int loc_count;
    int loc_fd;

    broker_init_temp_buffers();

    for(loc_count = 0; loc_count < GL_BLOCKING_SHMEM_NUMBER; loc_count++)
    {
        /*check if shmem exists*/
        shm_unlink(GL_BLOCKING_SHMEM[loc_count].shmem_name);

        loc_fd = shm_open(GL_BLOCKING_SHMEM[loc_count].shmem_name, K_SHMEM_FLAG, K_SHMEM_MODE);

        ftruncate(loc_fd, sizeof(T_BLOCKING_SHMEM));
        GL_BLOCKING_SHMEM[loc_count].shmem =
                mmap(NULL, sizeof(T_BLOCKING_SHMEM), K_MMAP_FLAG, MAP_SHARED, loc_fd, 0);

        close(loc_fd);

        sem_init(&GL_BLOCKING_SHMEM[loc_count].shmem->mutex, 1, 1);
        sem_init(&GL_BLOCKING_SHMEM[loc_count].shmem->ready, 1, 0);
    }

    return;
}

void broker_exit(void)
{
    unsigned int loc_count;


    for(loc_count = 0; loc_count < GL_BLOCKING_SHMEM_NUMBER; loc_count++)
    {
        /*check if shmem exists*/
        shm_unlink(GL_BLOCKING_SHMEM[loc_count].shmem_name);
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
        for(loc_count = 0; loc_count < GL_BLOCKING_SHMEM_NUMBER; loc_count++)
        {
            if(sem_trywait(&GL_BLOCKING_SHMEM[loc_count].shmem->mutex) == 0)
            {
                switch(loc_count)
                {
                       case 0: /*publisher*/
                        /*only for data from publisher.c*/
                        /*Store data from publisher for delivering to subscribers*/
                        memset(GL_SHMEM_TEMP[loc_count].shmem->app_out_data,0,sizeof(GL_SHMEM_TEMP[loc_count].shmem->app_out_data));
                        memcpy(GL_SHMEM_TEMP[loc_count].shmem->app_out_data,
                               GL_BLOCKING_SHMEM[loc_count].shmem->app_out_data,
                               GL_BLOCKING_SHMEM[loc_count].shmem->app_out_data_size);
                        GL_SHMEM_TEMP[loc_count].shmem->app_out_data_size = GL_BLOCKING_SHMEM[loc_count].shmem->app_out_data_size;
                        printf("<BRO received message:%s\n",(char*)GL_SHMEM_TEMP[loc_count].shmem->app_out_data);
                       break;
                       case 1: /*app*/
                        /*Store data from app for delivering to subscribers*/
                        memset(GL_SHMEM_TEMP[loc_count].shmem->app_in_data,0,sizeof(GL_SHMEM_TEMP[loc_count].shmem->app_in_data));
                        memcpy(GL_SHMEM_TEMP[loc_count].shmem->app_in_data,
                               GL_BLOCKING_SHMEM[loc_count].shmem->app_out_data,
                               GL_BLOCKING_SHMEM[loc_count].shmem->app_out_data_size);
                        GL_SHMEM_TEMP[loc_count].shmem->app_in_data_size = GL_BLOCKING_SHMEM[loc_count].shmem->app_out_data_size;

                        /*Send data to app from broker storage*/
                        memset(GL_BLOCKING_SHMEM[loc_count].shmem->app_in_data,0,sizeof(GL_BLOCKING_SHMEM[loc_count].shmem->app_in_data));
                        memcpy(GL_BLOCKING_SHMEM[loc_count].shmem->app_in_data,
                               GL_SHMEM_TEMP[0].shmem->app_out_data,
                               GL_SHMEM_TEMP[0].shmem->app_out_data_size);
                        GL_BLOCKING_SHMEM[loc_count].shmem->app_in_data_size = GL_BLOCKING_SHMEM[0].shmem->app_out_data_size;
                        printf(">BRO sent message:%s\n",(char*)GL_BLOCKING_SHMEM[loc_count].shmem->app_in_data);
                       break;
                       default:
                       break;
                }
                sem_post(&GL_BLOCKING_SHMEM[loc_count].shmem->mutex);
                sem_post(&GL_BLOCKING_SHMEM[loc_count].shmem->ready);
            }
        }

        usleep(10000);
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
