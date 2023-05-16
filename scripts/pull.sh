#!/bin/bash

cd /src/build

n_msgs=$1
n_msgs=1000000
msg_size=200000
n_ports=16
port_base=5535

# Define the array of ports
for ((i=0; i<$n_ports; i++)); do
  PORTS[$i]=$((port_base+i))
done

# Start the timer
start=$(date +%s.%N)

# Loop through the array and run the command with each port
for port in "${PORTS[@]}"
do
  ./pull "tcp://$ZMQ_CONNECT_IP:$port" $msg_size $n_msgs &
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
data_sent=$(echo "($n_msgs * $n_ports * $msg_size) / $total_time / 1024 / 1024 / 1024 * 8" | bc)
echo "Data sent: $data_sent Gbits/sec"