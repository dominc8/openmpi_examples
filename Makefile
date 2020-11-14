all: 00 01


00:
	mpic++ 00_get_cpu_info.cpp -o 00_get_cpu_info

run_hw_00:
	mpirun --use-hwthread-cpus 00_get_cpu_info

01:
	mpic++ 01_convolution.cpp -o 01_convolution

run_hw_01:
	mpirun --use-hwthread-cpus 01_convolution

