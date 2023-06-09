#!/bin/bash

cd /src/build

n_msgs=1000000
msg_size=2000000
n_ports=1
port_base=5535
num_io_threads=20

# Define the array of ports
for ((i=0; i<$n_ports; i++)); do
  PORTS[$i]=$((port_base+i))
done

# Loop through the array and run the command with each port
# for port in "${PORTS[@]}"
# do
./push "tcp://$ZMQ_BIND_IP:$port_base" $msg_size $n_msgs $num_io_threads
# done
