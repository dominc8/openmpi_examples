#include <mpi.h>
#include <iostream>

#define ARRAY_SIZE 10000
#define SEED 1488

/* Asynchronus example: FINDING MAXIMUM AND MINIMUM IN AN ARRAY */
/* Global variables */
int num_of_procs;
int my_array[ARRAY_SIZE];

/* Function declaration */
int find_min(int start, int stop);
int find_max(int start, int stop);
void array_fill(int* array, int size);

int main(int argc, char *argv[]) 
{

    // Initialize my variables
    int my_rank;
    int range;

    int my_start;   // Start of the array fragment
    int my_stop;    // End of the array fragment

    int my_min;     // Minimum found in this array fragment
    int my_max;     // Maximum found in this array fragment

    MPI_Request rq_min[100], rq_max[100];  // Status variables
    MPI_Status my_status;

    int others_min[50];  // Array for process 0 to store MIN values form others
    int others_max[50];  // Array for process 0 to store ARRAY_SIZE values form others

    array_fill(my_array, ARRAY_SIZE);  // Fill array with numbers

    MPI_Init(&argc, &argv);                        // Initialize

    MPI_Comm_size(MPI_COMM_WORLD, &num_of_procs);  // Get number of processes
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);       // Get my rank

    // Precess greetings!
    std::cout << "Rank: " << my_rank << " time to work!" << std::endl;

    // Specify start and end of proces array fragment
    range = ARRAY_SIZE / num_of_procs;
    my_start = my_rank * range;

    if (my_rank != num_of_procs - 1)
    {
        my_stop = my_start + range;
    }
    else
    {
        my_stop = ARRAY_SIZE;
    }

    // Find both minimum and maximum
    my_min = find_min(my_start, my_stop);
    my_max = find_max(my_start, my_stop);

    // Send if no the first process or recive data
    // For minimum
    if (my_rank == 0)
    {
        for (size_t i = 1; i < num_of_procs; i++)
        {
            MPI_Irecv(&others_min[i], 1, MPI_INT, i, 0, MPI_COMM_WORLD, &rq_min[i]);
        }
    }
    else
    {
        MPI_Isend(&my_min, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &rq_min[0]);
    }

    // For maximum
    if (my_rank == 0)
    {
        for (size_t i = 1; i < num_of_procs; i++)
        {
            MPI_Irecv(&others_max[i], 1, MPI_INT, i, 0, MPI_COMM_WORLD, &rq_max[i]);
        }
    }
    else
    {
        MPI_Isend(&my_max, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &rq_max[0]);
    }

    // Synchronize and print results
    if (my_rank == 0) 
    {
        for (size_t i = 1; i < num_of_procs; i++) 
        {
            MPI_Wait(&rq_min[i], &my_status);
            if (my_min > others_min[i])
            {
                my_min = others_min[i];
            }
        }

        for (size_t i = 1; i < num_of_procs; i++) 
        {
            MPI_Wait(&rq_max[i], &my_status);
            if (my_max < others_max[i])
            {
                my_max = others_max[i];
            }
        }

        std::cout << "Found min = " << my_min << std::endl;
        std::cout << "Found max = " << my_max << std::endl;
    }
    else 
    {
        // Other processes wait for the status "RECIVED" of their message before exiting
        MPI_Wait(&rq_min[0], &my_status);
        MPI_Wait(&rq_max[0], &my_status);
    }

    // Print off rank id
    std::cout << "Rank: " << my_rank << " FINISHED!" << std::endl;

    // Finalize the MPI environment.
    MPI_Finalize();

    return 0;
}

int find_min(int start, int stop) 
{

    // Find minimum
    int min = my_array[start];

    for (size_t i = start + 1; i < stop; i++) 
    {
        if (my_array[i] < min)
        {
            min = my_array[i];
        }
    }
    
    return min;
}

int find_max(int start, int stop) 
{

    // Find maximum
    int max = my_array[start];

    for (size_t i = start + 1; i < stop; i++) 
    {
        if (my_array[i] > max)
        {
            max = my_array[i];
        }
    }

    return max;
}

void array_fill(int* array, int size) 
{
    srand(SEED); //Seed the random system
    int start = rand() % 1500;

    for (int i = 0; i < size; i++)
    {
        array[i] = start + i;
    }
}
