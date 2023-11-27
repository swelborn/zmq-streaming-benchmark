/*
    Copyright (c) 2007-2016 Contributors as noted in the AUTHORS file

    This file is part of libzmq, the ZeroMQ core engine in C++.

    libzmq is free software; you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License (LGPL) as published
    by the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    As a special exception, the Contributors give you permission to link
    this library with independent modules to produce an executable,
    regardless of the license terms of these independent modules, and to
    copy and distribute the resulting executable under terms of your choice,
    provided that you also meet, for each linked independent module, the
    terms and conditions of the license of that module. An independent
    module is a module which is not derived from or based on this library.
    If you modify this library, you must extend this exception to your
    version of the library.

    libzmq is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
    License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <zmq.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <sched.h>
#include <pthread.h>

// keys are arbitrary but must match local_lat.cpp
const char server_pubkey[] = "DX4nh=yUn{-9ugra0X3Src4SU-4xTgqxcYY.+<SH";
const char client_pubkey[] = "<n^oA}I:66W+*ds3tAmi1+KJzv-}k&fC2aA5Bj0K";
const char client_prvkey[] = "9R9bV}[6z6DC-%$!jTVTKvWc=LEL{4i4gzUe$@Zx";

inline void set_context_cpu_mask(zmq::context_t &context,
                                 const std::vector<int> &cpus)
{
  int hardware_concurrency = std::thread::hardware_concurrency();

  for (unsigned int i = 0; i < hardware_concurrency; i++)
  {
    if (std::find(cpus.begin(), cpus.end(), i) != cpus.end())
    {
      // 7 = ZMQ_THREAD_AFFINITY_CPU_ADD
      zmq_ctx_set((void *)context, 7, i);
    }
    else
    {
      // 8 = ZMQ_THREAD_AFFINITY_CPU_REMOVE
      zmq_ctx_set((void *)context, 8, i);
    }
  }
}

void set_cpu_affinity(std::thread &t, const std::vector<int> &cpus)
{
  cpu_set_t cpuset;
  CPU_ZERO(&cpuset);

  for (int cpu : cpus)
  {
    CPU_SET(cpu, &cpuset);
  }

  int rc = pthread_setaffinity_np(t.native_handle(), sizeof(cpu_set_t), &cpuset);
  if (rc != 0)
  {
    std::cerr << "Error calling pthread_setaffinity_np: " << rc << "\n";
  }
}

// Function to receive data on the socket
void push_data(std::string bind_to, int message_count, size_t message_size, uint64_t i, const std::vector<int> &cpus)
{
  zmq::context_t context(2);

  // Get the maximum priority for the specified scheduler type
  int scheduler_priority = sched_get_priority_max(SCHED_RR);

  // Set the thread scheduler policy and priority for the context
  zmq_ctx_set((void *)context, ZMQ_THREAD_SCHED_POLICY, SCHED_RR);
  zmq_ctx_set((void *)context, ZMQ_THREAD_PRIORITY, scheduler_priority);
  set_context_cpu_mask(context, cpus);

  zmq::socket_t push_socket(context, zmq::socket_type::push);
  push_socket.set(zmq::sockopt::sndbuf, 4194304);
  uint64_t affinity = 1ULL << i;
  try
  {
    // push_socket.set(zmq::sockopt::affinity, affinity);
    push_socket.set(zmq::sockopt::immediate, 1);
    push_socket.bind(bind_to);
  }
  catch (zmq::error_t &e)
  {
    std::cerr << "Error: unable to bind push socket to: " << bind_to << std::endl;
    std::cerr << "Error (ZMQ): " << e.what() << std::endl;
    std::exit(1);
  }
  std::cout << "Affinity" << push_socket.get(zmq::sockopt::affinity) << std::endl;

  std::cout << "Push socket bound to: " << bind_to << std::endl;

  for (int i = 0; i != message_count; i++)
  {
    zmq::message_t msg(message_size);
    auto rc = push_socket.send(msg, 0);
    if (rc < 0)
    {
      printf("error in sending message: %s\n", zmq_strerror(errno));
    }
  }
}

int main(int argc, char *argv[])
{
  int first_port;
  int message_count;
  int message_size;
  int n_threads;
  int io_threads;
  void *s;
  int rc;
  int i;
  std::string bind_ip;
  zmq_msg_t msg;

  if (argc != 7)
  {
    printf("usage: push_mt_mc <bind_ip> <first-port> <message-size> "
           "<message-count> <threads> <io_threads>\n");
    return 1;
  }
  bind_ip = argv[1];
  first_port = atoi(argv[2]);
  message_size = atoi(argv[3]);
  message_count = atoi(argv[4]);
  n_threads = atoi(argv[5]);
  io_threads = atoi(argv[6]);

  int message_count_per_thread = message_count / n_threads;
  std::cout << "Message count per thread: " << message_count_per_thread << std::endl;

  std::vector<std::string> bind_to(n_threads);
  for (int i = 0; i < n_threads; i++)
  {
    bind_to[i] = "tcp://" + bind_ip + ":" + std::to_string(first_port + i);
  }

  zmq::context_t ctx = zmq::context_t(io_threads);
  std::cout << "Number of IO threads set: " << ctx.get(zmq::ctxopt::io_threads) << std::endl;
  std::vector<std::thread> push_threads;

  std::vector<std::vector<int>> cpu_sets = {{0, 1, 10, 11, 8}, {2, 3, 12, 13, 9}, {4, 5, 14, 15, 18}, {6, 7, 16, 17, 19}};
  // std::vector<std::vector<int>> cpu_sets = {{0, 1, 8, 9}, {2, 3, 10, 11}, {4, 5, 12, 13}, {6, 7, 14, 15}};

  for (uint64_t i = 0; i < n_threads; i++)
  {
    push_threads.emplace_back(push_data, bind_to[i], message_count_per_thread, message_size, i, cpu_sets[i]);
    set_cpu_affinity(push_threads[i], cpu_sets[i]);
  }

  for (auto &thread : push_threads)
  {
    thread.join();
  }

  zmq_ctx_destroy(&ctx);

  return 0;
}
