#include <cstdio>
#include <mpi.h>
#include <cstdint>
#include <algorithm>
#include <random>

int main(int argc, char **argv)
{
    constexpr int32_t arr_size = 100;
    int tag = 1234;
    int id, n_proc;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &n_proc);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);

    if (0 == id)
    {
        int32_t arr[2*arr_size];
        int32_t conv_val;

        std::uniform_int_distribution<int32_t> uniform_dist(-10, 10);
        std::default_random_engine eng(0);


        std::generate_n(&arr[0], 2*arr_size,
                        [&](void) -> int32_t { return uniform_dist(eng); });

        MPI_Send(&arr[0], 2*arr_size, MPI_INT32_T, 1, tag, MPI_COMM_WORLD);
        MPI_Recv(&conv_val, 1, MPI_INT32_T, 1, tag, MPI_COMM_WORLD, &status);

        printf("Received convolution value: %d\n", conv_val);
    }
    else if (1 == id)
    {
        int32_t arr[2*arr_size];
        int32_t conv_val = 0;

        MPI_Recv(&arr[0], 2*arr_size, MPI_INT32_T, 0, tag, MPI_COMM_WORLD, &status);

        int32_t *x = &arr[0];
        int32_t *y = &arr[2*arr_size - 1];
        for (int32_t i = 0; i < arr_size; ++i, ++x, --y)
        {
            conv_val += *x * *y;
        }

        MPI_Send(&conv_val, 1, MPI_INT32_T, 0, tag, MPI_COMM_WORLD);
    }
    else
    {
        /* Ignore */
    }

    MPI_Finalize();
    return 0;
}

