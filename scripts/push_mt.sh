#!/bin/bash

cd /src/build

n_msgs=1000000
msg_size=200000
n_ports=16
port_base=5535
n_threads=20
io_threads=1

# Define the array of ports
for ((i=0; i<$n_ports; i++)); do
  PORTS[$i]=$((port_base+i))
done

# Loop through the array and run the command with each port
./push_mt "*" $port_base $msg_size $n_msgs $n_threads $io_threads &
