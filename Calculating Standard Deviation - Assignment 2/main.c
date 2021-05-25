#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "mpi.h"

int main(int argc, char * argv[])
{
    int rank, numOfProcesses, n;
    int *arr;

    float localSum = 0, globalSum = 0, globalMean = 0, mean = 0;
    float localSumOfTheSquaredDifferences = 0, globalSumOfTheSquaredDifferences = 0;

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numOfProcesses);

    if(rank == 0)
    {
        printf("Enter number of elements per each process: ");
        scanf("%d", &n);
    }

    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if(rank != 0)
    {
        srand(rank);
        arr = malloc(n * sizeof(int));

        int i = 0;
        for(; i < n; i++)
        {
            int random = rand() % 100 + 1;
            arr[i] = random;
            localSum += random;
            printf("%d\n", arr[i]);
        }
    }

    MPI_Allreduce(&localSum, &globalSum, 1, MPI_FLOAT,MPI_SUM,MPI_COMM_WORLD);

    if(rank != 0)
    {
        mean = globalSum / (n  * (numOfProcesses - 1));
        
        int i = 0;
        for(; i < n; i++)
        {
            localSumOfTheSquaredDifferences += (arr[i] - mean) * (arr[i] - mean);
        }
    }

    MPI_Reduce(&localSumOfTheSquaredDifferences , &globalSumOfTheSquaredDifferences, 1, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD);

    if(rank == 0)
    {
        double meanOfTheSquaredDifferences = globalSumOfTheSquaredDifferences / ((numOfProcesses - 1) * n);
        double sd = sqrt(meanOfTheSquaredDifferences);
        printf("Standard Deviation: %f\n", sd);
    }

    /* shutdown MPI */
    MPI_Finalize();

    return 0;
}
