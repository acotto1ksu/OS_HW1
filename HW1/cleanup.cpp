#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <unistd.h>
#include <iostream>

#define TABLE_SIZE 2
#define PROD_ITERATIONS 20

struct mem {
    int buffer[TABLE_SIZE];
    int in = 0;
    int out = 0;
};

int table_counter = 0;

int main() {
    //Open shared memory table
    int shm = shm_open("/os_shmem", O_CREAT | O_RDWR, 0600);
    ftruncate(shm, sizeof(mem));
    mem* data = static_cast<mem*>(mmap(NULL, sizeof(mem), PROT_READ | PROT_WRITE, MAP_SHARED, shm, 0));

    //Open semaphores
    sem_t* sem_mutex = sem_open("/os_semmutex", O_CREAT, S_IRUSR | S_IWUSR, 1); //Mutual exclusivity semaphore
    sem_t* sem_can_consume = sem_open("/os_canconsume", O_CREAT, S_IRUSR | S_IWUSR, 0); //Init at 0, cause table starts with no elements to consume
    sem_t* sem_can_produce = sem_open("/os_canproduce", O_CREAT, S_IRUSR | S_IWUSR, TABLE_SIZE); //Init at size of table, cause this is the number of vacant slots on the table
    
    munmap(data, sizeof(mem));
    shm_unlink("/os_shmem");

    sem_close(sem_mutex);
    sem_unlink("/os_semmutex");
    sem_close(sem_can_produce);
    sem_unlink("/os_canproduce");
    sem_close(sem_can_consume);
    sem_unlink("/os_canconsume");
}

