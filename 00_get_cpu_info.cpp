#include <stdio.h>
#include <mpi.h>

int main(int argc, char **argv)
{
    int id, n_proc;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &n_proc);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);

    if (id == 0)
    {
        printf("Run %d processes\n", n_proc);
    }

    MPI_Finalize();
    return 0;
}

