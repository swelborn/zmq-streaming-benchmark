#!/bin/bash

cd /src/build

n_msgs=100000
msg_size=2000000
n_ports=4
port_base=5525
n_threads=4
io_threads=16

# Define the array of ports
for ((i=0; i<$n_ports; i++)); do
  PORTS[$i]=$((port_base+i))
done

# Loop through the array and run the command with each port
./push_mt_mc "*" $port_base $msg_size $n_msgs $n_threads $io_threads
