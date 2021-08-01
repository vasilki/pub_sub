TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

LIBS += -pthread -lrt

SOURCES += \
        main.c \
        publishers.c \
        broker.c \
        shmem_pub_sub.c \
        subscribers.c

HEADERS += \
    shmem_pub_sub.h
