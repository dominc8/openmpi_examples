#include <cstdio>
#include <mpi.h>
#include <cstdint>
#include <algorithm>
#include <random>
#include <cassert>
#include <cmath>

#ifndef NDEBUG
#define LOG(format, ...) printf("%d: " format "\n", id, ##__VA_ARGS__)
#else
#define LOG(format, ...) do {} while (0)
#endif

bool is_prime(int32_t x)
{
    if (x < 2)
    {
        return false;
    }
    else
    {
        int32_t x_sqrt = static_cast<int32_t>(sqrt(static_cast<float>(x)));
        for (int32_t i = 2; i <= x_sqrt; ++i)
        {
            if (x % i == 0)
            {
                return false;
            }
        }
        return true;
    }
}

int main(int argc, char **argv)
{
    constexpr int scheduler_id = 0;
    constexpr int ctrl_tag = 14;
    constexpr int data_tag = 88;
    constexpr int32_t start = 0;
    constexpr int32_t stop = 10000000;
    constexpr int32_t step = 50000;
    int id, n_proc;
    bool run = true;
    MPI_Status status;
    int32_t arr[2];

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &n_proc);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);

    if (scheduler_id == id)
    {
        int32_t sum_primes = 0;
        int proc_id = 0;
        int32_t recv_sum;

        for (int32_t i = start; i < stop;)
        {
            if (scheduler_id != proc_id)
            {
                arr[0] = i;
                arr[1] = i + step;
                i += step;
                run = true;
                MPI_Send(&run, 1, MPI_CXX_BOOL, proc_id, ctrl_tag, MPI_COMM_WORLD);
                MPI_Send(&arr[0], 2, MPI_INT32_T, proc_id, data_tag, MPI_COMM_WORLD);
                LOG("Sent <%d, %d) and run to proc %d", arr[0], arr[1], proc_id);
            }
            ++proc_id;
            if (proc_id >= n_proc)
            {
                proc_id = 0;
                for (int recv_id = 0; recv_id < n_proc; ++recv_id)
                {
                    if (scheduler_id != recv_id)
                    {
                        MPI_Recv(&recv_sum, 1, MPI_INT32_T, recv_id, data_tag, MPI_COMM_WORLD, &status);
                        LOG("Received %d from proc %d", recv_sum, recv_id);
                        sum_primes += recv_sum;
                    }
                }
            }
        }

        LOG("After main loop");

        /* Receive data from all left slaves */
        if (proc_id < n_proc)
        {
            for (int recv_id = 0; recv_id < proc_id; ++recv_id)
            {
                if (scheduler_id != recv_id)
                {
                    MPI_Recv(&recv_sum, 1, MPI_INT32_T, recv_id, data_tag, MPI_COMM_WORLD, &status);
                    LOG("Received %d from proc %d", recv_sum, recv_id);
                    sum_primes += recv_sum;
                }
            }
        }

        /* Stop all slaves */
        run = false;
        for (int recv_id = 0; recv_id < n_proc; ++recv_id)
        {
            if (scheduler_id != recv_id)
            {
                MPI_Send(&run, 1, MPI_CXX_BOOL, recv_id, ctrl_tag, MPI_COMM_WORLD);
                LOG("Sent stop to %d", recv_id);
            }
        }

        printf("Sum of primes in range <%d, %d) is %d\n", start, stop, sum_primes);
    }
    else
    {
        MPI_Recv(&run, 1, MPI_CXX_BOOL, scheduler_id, ctrl_tag, MPI_COMM_WORLD, &status);
        LOG("Received run: %d", run);
        while(run)
        {
            MPI_Recv(&arr[0], 2, MPI_INT32_T, scheduler_id, data_tag, MPI_COMM_WORLD, &status);
            LOG("Receive <%d, %d)", arr[0], arr[1]);
            int32_t range_start = arr[0];
            int32_t range_stop = arr[1];
            int32_t sum_val = 0;

            for (int32_t i = range_start; i < range_stop; ++i)
            {
                if (is_prime(i))
                {
                    sum_val += i;
                }
            }
            
            MPI_Send(&sum_val, 1, MPI_INT32_T, scheduler_id, data_tag, MPI_COMM_WORLD);
            LOG("Sent sum %d", sum_val);
            MPI_Recv(&run, 1, MPI_CXX_BOOL, scheduler_id, ctrl_tag, MPI_COMM_WORLD, &status);
            LOG("Received run: %d", run);
        };
        LOG("Proc %d ends", id);
    }

    MPI_Finalize();
    return 0;
}

