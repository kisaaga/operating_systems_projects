// Mehmet Efe Kisaaga	25255	CS307-HW1

#include <iostream>
#include <stdlib.h>
#include "pthread.h"
#include <stdio.h>
#include <time.h>


using namespace std;

// There are 100 seats: 2x50
int seats[2][50];
int rem_seats = 100;

// we need in busy-waiting
int turn = 0;

// Agency 1:	First, produce random seat and check it wether it reserved or not
//				If not reserved, reserve it and marked the seat as 1
//				End the thread if there is no seat left
void* agency1(void*) {
	while (true)
	{
		int row = rand() % 2;
		int col = rand() % 50;

		while (turn != 0);

		if (rem_seats == 0)
		{
			printf("\nAgency 1 Entered Critical Region\n");
			printf("Agency 1 Exit Critical Region\n");
		}

		if (seats[row][col] == 0) // if generated seat is not marked
		{
			printf("\nAgency 1 Entered Critical Region\n");
			seats[row][col] = 1; // mark it
			rem_seats--;
			int seat_num = row * 50 + col + 1;
			printf("Seat Number %d is reserved by Agency 1\n", seat_num);
			printf("Agency 1 Exit Critical Region\n");
		}

		turn = 1;

		if (rem_seats == 0)
		{
			pthread_exit(NULL);
		}
	}
}

// Agency 2:	First, produce random seat and check it wether it reserved or not
//				If not reserved, reserve it and marked the seat as 2
//				End the thread if there is no seat left
void* agency2(void*) {
	while (true)
	{
		int row = rand() % 2;
		int col = rand() % 50;

		while (turn != 1);

		if (rem_seats == 0)
		{
			printf("\nAgency 2 Entered Critical Region\n");
			printf("Agency 2 Exit Critical Region\n");
		}

		if (seats[row][col] == 0) // if generated seat is not marked
		{
			printf("\nAgency 2 Entered Critical Region\n");
			seats[row][col] = 2; // mark it
			rem_seats--;
			int seat_num = row * 50 + col + 1;
			printf("Seat Number %d is reserved by Agency 2\n", seat_num);
			printf("Agency 2 Exit Critical Region\n");
		}

		turn = 0;

		if (rem_seats == 0)
		{
			pthread_exit(NULL);
		}
	}
}

int main() {

	srand(time(NULL)); // To produce different random numbers

	// Initialization of seats, firsly all seats are 0
	for (int j = 0; j < 2; j++)
	{
		for (int k = 0; k < 50; k++)
		{
			seats[j][k] = 0;
		}
	}

	pthread_t thread1, thread2;

	// Start the threads
	pthread_create(&thread1, NULL, agency1, NULL);
	pthread_create(&thread2, NULL, agency2, NULL);

	// Hold main thread until thread1 and thread2 finish their jobs and join to main thread when no seats left
	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);

	if (rem_seats == 0)
	{
		cout << "No Seats Left\n\nPlane is full:\n\n";
	}

	// Printing the seats - Agency1 marked the seats as 1 and Agency2 marked as 2
	for (int r = 0; r < 2; r++)
	{
		for (int c = 0; c < 50; c++)
		{
			cout << seats[r][c] << " ";
		}
		cout << endl;
	}

	return 0;
}