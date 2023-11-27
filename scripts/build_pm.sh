#!/bin/bash

TAG=`git log -1 --pretty=%h`

podman-hpc build -t samwelborn/zmq-streaming-benchmark:${TAG} -t registry.nersc.gov/ncemhub/swelborn/zmq-streaming-benchmark:${TAG} ../
