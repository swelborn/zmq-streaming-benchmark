#!/bin/bash

srun -n 1 -N 1 podman-hpc run --rm --net=host -v /global/homes/s/swelborn/gits/zmq-streaming-benchmark/scripts:/scripts samwelborn/zmq-streaming-benchmark:f32679f /scripts/push_mt.sh