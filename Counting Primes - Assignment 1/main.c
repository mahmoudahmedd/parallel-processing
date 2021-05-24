#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

int isPrime(int i)
{
	int flag = 1;
    int j;

    if (i == 1 || i == 0)
        return 0;

    for(j = 2; j <= i / 2; ++j)
    {
        if (i % j == 0) {
            flag = 0;
            break;
        }
    }

    return flag;
}

int main(int argc, char * argv[])
{
	// Rank of process
	int my_rank;
	/* number of process	*/	
	int p;
	/* return status for 	*/
	MPI_Status status;
				

	// Start up MPI
	MPI_Init(&argc, &argv);

	// Find out process rank
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	// Find out number of process
	MPI_Comm_size(MPI_COMM_WORLD, &p);


	if (my_rank == 0)
	{
		int x, y;
		int res = 0;

		printf("Enter lower bound: ");
		scanf("%d", &x);

		printf("Enter upper bound: ");
		scanf("%d", &y);


		int len = (y - x + 1);
		int * arr = malloc(len * sizeof(int));
		
		int i = x;
		for (; i <= len; i++) 
		{
			arr[i] = i;
		}

		int portionSize = (y-x)/(p - 1);

		for (i = 1; i < p; i++)
		{
			MPI_Send(&portionSize, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
			MPI_Send(arr + ((i - 1)* portionSize), portionSize, MPI_INT, i, 0, MPI_COMM_WORLD);
		}

		for (i = 1; i < p; i++) {
			int subSum;
			MPI_Recv(&subSum, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
			res += subSum;
		}

		printf("Total number of prime numbers is: = %d\n", res);
	}
	else
	{
		int portionSize;
		MPI_Recv(&portionSize, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
		int* arr = malloc(portionSize * sizeof(int));
		MPI_Recv(arr, portionSize, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
		int i = 0;
		int localSum = 0;

		for (; i < portionSize; i++)
		{
			if(isPrime(arr[i]) == 1)
			{
				localSum += 1;
			}
		}

		printf("Total number of prime numbers in P%d", my_rank);
		printf(" is: %d\n", localSum);

		MPI_Send(&localSum, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
	}

	// Shutdown MPI
	MPI_Finalize();
	return 0;
}
