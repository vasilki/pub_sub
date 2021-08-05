#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "shmem_pub_sub.h"

extern void publishers(void);



extern void broker_thread(void *par_args);
extern void publisher_thread(void *par_args);
extern void subscriber_thread(void *par_args);
extern void publisher_init();
extern void port_init();
extern void port_thread(void *par_args);
extern void core_init();
extern void core_thread(void *par_args);

extern void broker_init();

int main()
{
    static pthread_t loc_thread_broker;
    static pthread_t loc_thread_publisher[K_MAX_PUBLISHERS];
    static pthread_t loc_thread_port;
    static pthread_t loc_thread_core;
    static pthread_attr_t loc_thread_attr;
    static unsigned char loc_attr[100];
    unsigned int loc_count;

    printf("Hello World!\n");

    pthread_attr_init(&loc_thread_attr);

    broker_init();
    publisher_init();


    pthread_create(&loc_thread_broker,&loc_thread_attr,(void*)broker_thread,(void*)&loc_attr[0]);

//    subscriber_init();

    for(loc_count = 0; loc_count < K_MAX_PUBLISHERS; loc_count++)
    {
        pthread_create(&loc_thread_publisher[loc_count],&loc_thread_attr,(void*)publisher_thread,(void*)&loc_attr[0]);
    }

    port_init();

    pthread_create(&loc_thread_port,&loc_thread_attr,(void*)port_thread,(void*)&loc_attr[0]);
    pthread_create(&loc_thread_core,&loc_thread_attr,(void*)core_thread,(void*)&loc_attr[0]);

    pthread_join(loc_thread_broker,NULL);
    pthread_join(loc_thread_port,NULL);
    pthread_join(loc_thread_core,NULL);

    for(loc_count = 0; loc_count < K_MAX_PUBLISHERS; loc_count++)
    {
        pthread_join(loc_thread_publisher[loc_count],NULL);
      //  pthread_detach(loc_thread_publisher[loc_count]);
    }


    return 0;
}






