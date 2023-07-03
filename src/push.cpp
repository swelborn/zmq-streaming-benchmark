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
#include <string.h>

// keys are arbitrary but must match local_lat.cpp
const char server_pubkey[] = "DX4nh=yUn{-9ugra0X3Src4SU-4xTgqxcYY.+<SH";
const char client_pubkey[] = "<n^oA}I:66W+*ds3tAmi1+KJzv-}k&fC2aA5Bj0K";
const char client_prvkey[] = "9R9bV}[6z6DC-%$!jTVTKvWc=LEL{4i4gzUe$@Zx";

int main(int argc, char *argv[])
{
    const char *bind_to;
    int message_count;
    int message_size;
    int num_io_threads;
    void *ctx;
    void *s;
    int rc;
    int i;
    zmq_msg_t msg;

    if (argc != 4 && argc != 5)
    {
        printf("usage: push <bind-to> <message-size> "
               "<message-count> <io_threads>\n");
        return 1;
    }
    bind_to = argv[1];
    message_size = atoi(argv[2]);
    message_count = atoi(argv[3]);
    num_io_threads = atoi(argv[4]);

    ctx = zmq_init(num_io_threads);
    if (!ctx)
    {
        printf("error in zmq_init: %s\n", zmq_strerror(errno));
        return -1;
    }

    s = zmq_socket(ctx, ZMQ_PUSH);
    if (!s)
    {
        printf("error in zmq_socket: %s\n", zmq_strerror(errno));
        return -1;
    }

    //  Add your socket options here.
    rc = zmq_bind(s, bind_to);
    if (rc != 0)
    {
        printf("error in zmq_connect: %s\n", zmq_strerror(errno));
        return -1;
    }

    for (i = 0; i != message_count; i++)
    {
        rc = zmq_msg_init_size(&msg, message_size);
        if (rc != 0)
        {
            printf("error in zmq_msg_init_size: %s\n", zmq_strerror(errno));
            return -1;
        }
        rc = zmq_sendmsg(s, &msg, 0);
        if (rc < 0)
        {
            printf("error in zmq_sendmsg: %s\n", zmq_strerror(errno));
            return -1;
        }
        rc = zmq_msg_close(&msg);
        if (rc != 0)
        {
            printf("error in zmq_msg_close: %s\n", zmq_strerror(errno));
            return -1;
        }
    }

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
