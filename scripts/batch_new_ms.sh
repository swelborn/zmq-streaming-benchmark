#!/bin/bash

#SBATCH --qos=realtime
#SBATCH --constraint=cpu
#SBATCH --nodes=1
#SBATCH --time=00:05:00
#SBATCH --exclusive
#SBATCH --account=m3795

srun -n 1 -N 1 podman-hpc run --rm --net=host --env-file="/global/homes/s/swelborn/gits/zmq-streaming-benchmark/config/conf" -v /global/homes/s/swelborn/gits/zmq-streaming-benchmark/scripts:/scripts samwelborn/zmq-streaming-benchmark:716ccca /scripts/pull_mt.sh 100000