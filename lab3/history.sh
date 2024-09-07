#!/bin/sh

gcc sequential_min_max.c find_min_max.c utils.c -lm -o seq_min_max.out
./seq_min_max.out 23123 10

gcc parallel_min_max.c find_min_max.c utils.c -lm -o parallel_min_max.out
./parallel_min_max.out --seed 40 --array_size 1000 --pnum 10 --by_files
#Waited child process (pid = 90582)
#Waited child process (pid = 90583)
#Waited child process (pid = 90584)
#Waited child process (pid = 90585)
#Waited child process (pid = 90586)
#Waited child process (pid = 90587)
#Waited child process (pid = 90588)
#Waited child process (pid = 90589)
#Waited child process (pid = 90590)
#Waited child process (pid = 90591)
#Min: 0
#Max: 9988
#Elapsed time: 1.831000ms
./parallel_min_max.out --seed 40 --array_size 1000 --pnum 10
# Waited child process (pid = 90909)
# Waited child process (pid = 90910)
# Waited child process (pid = 90911)
# Waited child process (pid = 90912)
# Waited child process (pid = 90913)
# Waited child process (pid = 90914)
# Waited child process (pid = 90915)
# Waited child process (pid = 90916)
# Waited child process (pid = 90917)
# Waited child process (pid = 90918)
# Min: 0
# Max: 9988
# Elapsed time: 1.290000ms
