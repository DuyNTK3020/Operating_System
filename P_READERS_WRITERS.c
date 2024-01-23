#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_READERS 3
#define NUM_WRITERS 2

int data = 0;
int readers = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t can_read = PTHREAD_COND_INITIALIZER;
pthread_cond_t can_write = PTHREAD_COND_INITIALIZER;

void *writer(void *arg) {
    int id = *((int *)arg);
    free(arg);

    for (int i = 0; i < 5; ++i) {
        pthread_mutex_lock(&mutex);
        while (readers > 0) {
            pthread_cond_wait(&can_write, &mutex);
        }

        data = id * 10 + i;
        printf("Writer %d writes data: %d\n", id, data);

        pthread_mutex_unlock(&mutex);
        pthread_cond_broadcast(&can_read);
        usleep(200000); // Sleep for 200 milliseconds
    }

    return NULL;
}

void *reader(void *arg) {
    int id = *((int *)arg);
    free(arg);

    for (int i = 0; i < 5; ++i) {
        pthread_mutex_lock(&mutex);
        while (readers > 0) {
            pthread_cond_wait(&can_read, &mutex);
        }

        ++readers;
        pthread_mutex_unlock(&mutex);

        // Reading data
        usleep(100000); // Sleep for 100 milliseconds
        pthread_mutex_lock(&mutex);
        printf("Reader %d reads data: %d\n", id, data);
        --readers;

        if (readers == 0) {
            pthread_cond_broadcast(&can_write);
        }

        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}

int main() {
    pthread_t readers[NUM_READERS];
    pthread_t writers[NUM_WRITERS];

    for (int i = 0; i < NUM_READERS; ++i) {
        int *id = malloc(sizeof(int));
        *id = i + 1;
        pthread_create(&readers[i], NULL, reader, id);
    }

    for (int i = 0; i < NUM_WRITERS; ++i) {
        int *id = malloc(sizeof(int));
        *id = i + 1;
        pthread_create(&writers[i], NULL, writer, id);
    }

    for (int i = 0; i < NUM_READERS; ++i) {
        pthread_join(readers[i], NULL);
    }

    for (int i = 0; i < NUM_WRITERS; ++i) {
        pthread_join(writers[i], NULL);
    }

    return 0;
}
