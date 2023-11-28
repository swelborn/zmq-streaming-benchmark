#!/bin/bash

cd /src/build

n_msgs=$1
n_msgs=100000
msg_size=2000000
n_pushing_threads=8
n_ports=$n_pushing_threads
n_msgs_per_port=$(($n_msgs / $n_pushing_threads))
port_base=5525
n_io_threads=10
n_connections=100

# Define the array of ports
for ((i=0; i<$n_ports; i++)); do
  PORTS[$i]=$((port_base+i))
done

# Start the timer
start=$(date +%s.%N)
echo $

# Loop through the array and run the command with each port
for port in "${PORTS[@]}"
do
  ./pull "tcp://$ZMQ_CONNECT_IP:$port" $msg_size $n_msgs_per_port $n_io_threads $n_connections &
  PIDS+=($!) # add the PID of the most recently backgrounded process to the PIDS array
done

# Wait for all background processes to finish
for PID in "${PIDS[@]}"
do
  wait $PID
done

# End the timer and calculate data rate
end=$(date +%s.%N)
total_time=$(echo "$end - $start" | bc)
throughput=$(echo "($n_msgs / $total_time)" | bc)
data_sent=$(echo "($throughput * $msg_size * 8) / 1000000 " | bc)
echo "---------------" 
echo "Throughput: $throughput msgs/s"
echo "Time: $total_time s" 
echo "Data rate: $data_sent Mb/s"
echo "---------------"