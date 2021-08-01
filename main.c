#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

extern void publishers(void);

#define K_MAX_PUBLISHERS 1
#define K_MAX_SUBSCRIBERS K_MAX_PUBLISHERS

extern void broker_thread(void *par_args);
extern void publisher_thread(void *par_args);
extern void subscriber_thread(void *par_args);


int main()
{
    static pthread_t loc_thread_broker;
    static pthread_t loc_thread_publisher[K_MAX_PUBLISHERS];
    static pthread_t loc_thread_subscriber[K_MAX_SUBSCRIBERS];
    static pthread_attr_t loc_thread_attr;
    static unsigned char loc_attr[100];
    unsigned int loc_count;

    printf("Hello World!\n");

    pthread_attr_init(&loc_thread_attr);

    pthread_create(&loc_thread_broker,&loc_thread_attr,(void*)broker_thread,(void*)&loc_attr[0]);

    for(loc_count = 0; loc_count < K_MAX_PUBLISHERS; loc_count++)
    {
        pthread_create(&loc_thread_publisher[loc_count],&loc_thread_attr,(void*)publisher_thread,(void*)&loc_attr[0]);
    }

    for(loc_count = 0; loc_count < K_MAX_SUBSCRIBERS; loc_count++)
    {
    //    pthread_create(&loc_thread_subscriber[loc_count],&loc_thread_attr,(void*)subscriber_thread,(void*)&loc_attr[0]);
    }


    pthread_join(loc_thread_broker,NULL);

    for(loc_count = 0; loc_count < K_MAX_PUBLISHERS; loc_count++)
    {
        pthread_join(loc_thread_publisher[loc_count],NULL);
      //  pthread_detach(loc_thread_publisher[loc_count]);
    }

    for(loc_count = 0; loc_count < K_MAX_SUBSCRIBERS; loc_count++)
    {
      //  pthread_join(loc_thread_subscriber[loc_count],NULL);
    }

    return 0;
}






