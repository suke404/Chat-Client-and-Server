#ifndef THREAD_H
#define THREAD_H
#include "list.h"
#include <pthread.h>

// only need to include initializer and destructor


void initializeThreads();
void destroyThreads();

#endif