#!/bin/bash

srun -n 1 -N 1 podman-hpc run --rm --net=host --env-file="/global/homes/s/swelborn/gits/zmq-streaming-benchmark/config/conf" -v /global/homes/s/swelborn/gits/zmq-streaming-benchmark/scripts:/scripts samwelborn/zmq-streaming-benchmark:716ccca /scripts/pull_mt.sh 100000