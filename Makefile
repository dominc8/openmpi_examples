all: 00 01 02 03


00:
	mpic++ 00_get_cpu_info.cpp -o 00_get_cpu_info

run_hw_00:
	mpirun --use-hwthread-cpus 00_get_cpu_info

01:
	mpic++ 01_convolution.cpp -o 01_convolution

run_hw_01:
	mpirun --use-hwthread-cpus 01_convolution

02:
	mpic++ -DNDEBUG -Ofast 02_sum_primes.cpp -o 02_sum_primes

run_hw_02:
	mpirun --use-hwthread-cpus 02_sum_primes

03:
	mpic++ -DNDEBUG -Ofast 03_max_min_async.cpp -o 03_max_min_async

run_hw_03:
	mpirun --use-hwthread-cpus 03_max_min_async

