#!/bin/bash

TAG=`git log -1 --pretty=%h`
sudo docker build -t samwelborn/zmq-streaming-benchmark:${TAG} ../
