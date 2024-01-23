#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <iostream>

using namespace std;
void prefix_mpi(int *block_array, int block_size, int *block_prefix, MPI_Comm communicator)
{
    int my_rank;
    int com_size;
    MPI_Comm_rank(communicator, &my_rank);
    MPI_Comm_size(communicator, &com_size);

    int block_sum = 0;

    for (int ind = 0; ind < block_size; ind++)
    {
        block_sum += block_array[ind];
    }

    int previous_block_sum = 0;


    if (my_rank != 0)
    {
        MPI_Recv(&previous_block_sum, 1, MPI_INT, my_rank - 1, 0, communicator, MPI_STATUS_IGNORE);
    }

    if (my_rank != (com_size - 1))
    {
        int all_previous_block_sum = block_sum + previous_block_sum;
        MPI_Send(&all_previous_block_sum, 1, MPI_INT, my_rank + 1, 0, MPI_COMM_WORLD);
    }
    block_prefix[0] = previous_block_sum + block_array[0];

    for (int ind = 1; ind < block_size; ind++)
    {
        block_prefix[ind] = block_prefix[ind - 1] + block_array[ind];
    }
}

int main(int argc, char **args)
{
    MPI_Init(&argc, &args);

    int my_rank;
    int com_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &com_size);

    int total_array_size = 2048;

    if (total_array_size % com_size != 0)
        total_array_size = (total_array_size / com_size + 1) * com_size;

    int block_size = total_array_size / com_size;
    int *total_array = NULL;
    int *total_prefix = NULL;

    if (my_rank == 0)
    {
        total_array = (int *)malloc(total_array_size * sizeof(int));
        total_prefix = (int *)malloc(total_array_size * sizeof(int));
        for (int i = 0; i < total_array_size; i++)
        {
            // total_array[i] = rand() % 11;
            total_array[i] = 1;
        }
    }

    int *block_array = (int *)malloc(block_size * sizeof(int));
    int *block_prefix = (int *)malloc(block_size * sizeof(int));

    MPI_Scatter(total_array, block_size, MPI_INT,
                block_array, block_size, MPI_INT, 0, MPI_COMM_WORLD);

    prefix_mpi(block_array, block_size, block_prefix, MPI_COMM_WORLD);

    MPI_Gather(block_prefix, block_size, MPI_INT,
               total_prefix, block_size, MPI_INT, 0, MPI_COMM_WORLD);

    int accum = 0;
    if (my_rank == 0)
    {
        int num_of_errors = 0;
        for (int i = 0; i < total_array_size; i++)
        {
            // cout << "total_prefix  in i = " << i << ", equals =  " << total_prefix[i] << endl;
            accum += total_array[i];
            if (total_prefix[i] != accum)
            {
                cout << "Error at index " << i << "  " << accum << " expected, " << total_prefix[i] << "computed\n";
                num_of_errors++;
            }
        }
        cout << "number of errors = " << num_of_errors << " \n";
        cout << "Test completed!\n";
        free(total_array);
        free(total_prefix);
    }
    free(block_array);
    free(block_prefix);

    MPI_Finalize();

    return 0;
}