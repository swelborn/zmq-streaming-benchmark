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

#include <zmq.h>
#include <stdio.h>
#include <stdlib.h>

// keys are arbitrary but must match remote_lat.cpp
const char server_prvkey[] = "{X}#>t#jRGaQ}gMhv=30r(Mw+87YGs+5%kh=i@f8";

int main(int argc, char *argv[])
{
    const char *connect_to;
    int message_count;
    size_t message_size;
    int num_connections;
    int num_io_threads;
    void *ctx;
    void *s;
    int rc;
    int i;
    int j;
    zmq_msg_t msg;
    void *watch;
    unsigned long elapsed;
    double throughput;
    double megabits;

    if (argc != 6 && argc != 5)
    {
        printf("usage: pull <connect-to> <message-size> <message-count> <num_io_threads> <num-connections> "
               "\n");
        return 1;
    }
    connect_to = argv[1];
    message_size = atoi(argv[2]);
    message_count = atoi(argv[3]);
    num_io_threads = atoi(argv[4]);
    num_connections = atoi(argv[5]);

    ctx = zmq_init(num_io_threads);
    if (!ctx)
    {
        printf("error in zmq_init: %s\n", zmq_strerror(errno));
        return -1;
    }

    s = zmq_socket(ctx, ZMQ_PULL);
    if (!s)
    {
        printf("error in zmq_socket: %s\n", zmq_strerror(errno));
        return -1;
    }
    for (j = 0; j != num_connections - 1; j++)
    {
        rc = zmq_connect(s, connect_to);
    }
    if (rc != 0)
    {
        printf("error in zmq_connect: %s\n", zmq_strerror(errno));
        return -1;
    }

    rc = zmq_msg_init(&msg);
    if (rc != 0)
    {
        printf("error in zmq_msg_init: %s\n", zmq_strerror(errno));
        return -1;
    }

    rc = zmq_recvmsg(s, &msg, 0);
    if (rc < 0)
    {
        printf("error in zmq_recvmsg: %s\n", zmq_strerror(errno));
        return -1;
    }
    if (zmq_msg_size(&msg) != message_size)
    {
        printf("message of incorrect size received\n");
        return -1;
    }

    watch = zmq_stopwatch_start();

    for (i = 0; i != message_count - 1; i++)
    {
        rc = zmq_recvmsg(s, &msg, 0);
        if (rc < 0)
        {
            printf("error in zmq_recvmsg: %s\n", zmq_strerror(errno));
            return -1;
        }
        if (zmq_msg_size(&msg) != message_size)
        {
            printf("message of incorrect size received\n");
            return -1;
        }
    }

    elapsed = zmq_stopwatch_stop(watch);
    if (elapsed == 0)
        elapsed = 1;

    rc = zmq_msg_close(&msg);
    if (rc != 0)
    {
        printf("error in zmq_msg_close: %s\n", zmq_strerror(errno));
        return -1;
    }

    throughput = ((double)message_count / (double)elapsed * 1000000);
    megabits = ((double)throughput * message_size * 8) / 1000000;

    printf("message size: %d [B]\n", (int)message_size);
    printf("message count: %d\n", (int)message_count);
    printf("mean throughput: %d [msg/s]\n", (int)throughput);
    printf("mean throughput: %.3f [Mb/s]\n", (double)megabits);

    rc = zmq_close(s);
    if (rc != 0)
    {
        printf("error in zmq_close: %s\n", zmq_strerror(errno));
        return -1;
    }

    rc = zmq_ctx_term(ctx);
    if (rc != 0)
    {
        printf("error in zmq_ctx_term: %s\n", zmq_strerror(errno));
        return -1;
    }

    return 0;
}
