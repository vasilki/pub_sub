TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

LIBS += -pthread -lrt

SOURCES += \
        app/core.c \
        app/port.c \
        main.c \
        publishers.c \
        broker.c \
        shmem_pub_sub.c

HEADERS += \
    app/app.h \
    shmem_pub_sub.h
