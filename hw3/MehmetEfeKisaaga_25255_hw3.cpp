#include <iostream>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <string>
#include <stdlib.h> 
#include <queue> 
#include <semaphore.h>
using namespace std;

#define NUM_THREADS 10
#define MEMORY_SIZE 150

struct node
{
	int id;
	int size;
};


queue<node> myqueue; // shared que
pthread_mutex_t sharedLock = PTHREAD_MUTEX_INITIALIZER; // mutex
pthread_t server; // server thread handle
sem_t semlist[NUM_THREADS]; // thread semaphores

int thread_message[NUM_THREADS]; // thread memory information
char  memory[MEMORY_SIZE]; // memory size





void my_malloc(int thread_id, int size)
{
	//This function will add the struct to the queue
	node temp;
	temp.id = thread_id;
	temp.size = size;
	pthread_mutex_lock(&sharedLock); //     ???????????????????
	myqueue.push(temp);
	pthread_mutex_unlock(&sharedLock);
}

void* server_function(void*)
{
	//This function should grant or decline a thread depending on memory size.
	int curr_index = 0;
	while (true)
	{
		pthread_mutex_lock(&sharedLock);
		if (!myqueue.empty())
		{
			node temp = myqueue.front();
			if (MEMORY_SIZE - curr_index < temp.size)
			{
				thread_message[temp.id] = -1; //not enough memory error
			}
			else
			{
				thread_message[temp.id] = curr_index;
				curr_index = curr_index + temp.size;
			}
			sem_post(&semlist[temp.id]);
			myqueue.pop();
		}
		pthread_mutex_unlock(&sharedLock);
	}
}

void* thread_function(void* id)
{
	
	//This function will create a random size, and call my_malloc
	//Block
	//Then fill the memory with id's or give an error prompt
	int maximum = MEMORY_SIZE / 6;
	int mem_size = rand() % maximum + 1; // create a random memory size
	int* myid = (int*)id;
	my_malloc(*myid, mem_size);
	sem_wait(&semlist[*myid]);
	int response = thread_message[*myid];
	if (response == -1)
	{
		printf("Thread %d: Not enough memory\n", *myid);
	}
	else
	{
		pthread_mutex_lock(&sharedLock);
		for (int i = 0; i < mem_size; i++)
		{
			char id_mark = '0' + *myid;
			memory[response + i] = id_mark;
		}
		pthread_mutex_unlock(&sharedLock);
	}
}

void init()
{
	pthread_mutex_lock(&sharedLock);	//lock
	for (int i = 0; i < NUM_THREADS; i++) //initialize semaphores
	{
		sem_init(&semlist[i], 0, 0);
	}
	for (int i = 0; i < MEMORY_SIZE; i++)	//initialize memory 
	{
		char zero = '0'; memory[i] = zero;
	}
	pthread_create(&server, NULL, server_function, NULL); //start server 
	pthread_mutex_unlock(&sharedLock); //unlock
}



void dump_memory()
{
	// You need to print the whole memory array here.
	for (int i = 0; i < MEMORY_SIZE; i++)
	{
		cout << memory[i] << " ";
	}
	cout << endl;
}

int main(int argc, char* argv[])
{
	srand(time(NULL));
	//You need to create a thread ID array here
	pthread_t thread_id[NUM_THREADS];
	int id_of_threads[NUM_THREADS];

	init();	// call init

	//You need to create threads with using thread ID array, using pthread_create()
	for (int i = 0; i < NUM_THREADS; i++)
	{
		id_of_threads[i] = i;
		pthread_create(&thread_id[i], NULL, thread_function, &id_of_threads[i]);
	}

	//You need to join the threads
	for (int i = 0; i < NUM_THREADS; i++)
	{
		pthread_join(thread_id[i], NULL);
	}

	dump_memory(); // this will print out the memory

	printf("\nMemory Indexes:\n");
	for (int i = 0; i < NUM_THREADS; i++)
	{
		printf("[%d]", thread_message[i]); // this will print out the memory indexes
	}
	printf("\nTerminating...\n");
}