FROM ubuntu:22.04 as base

ENV DEBIAN_FRONTEND noninteractive

# Change the source list to a faster mirror
RUN echo "deb http://mirror.math.princeton.edu/pub/ubuntu/ jammy main restricted universe multiverse" > /etc/apt/sources.list && \
    echo "deb http://mirror.math.princeton.edu/pub/ubuntu/ jammy-updates main restricted universe multiverse" >> /etc/apt/sources.list && \
    echo "deb http://mirror.math.princeton.edu/pub/ubuntu/ jammy-backports main restricted universe multiverse" >> /etc/apt/sources.list && \
    echo "deb http://mirror.math.princeton.edu/pub/ubuntu/ jammy-security main restricted universe multiverse" >> /etc/apt/sources.list

RUN apt-get update                                      && \
    apt-get upgrade --yes                               && \
    apt-get install --yes \
    autoconf \
    automake \
    build-essential \
    git \
    libkrb5-dev \
    libsodium-dev \
    libtool \
    pkg-config \
    cmake \
    && apt-get clean all                                && \
    rm -rf /var/lib/apt/lists/*                                                

RUN git clone https://github.com/zeromq/libzmq.git                                  && \
    cd libzmq && mkdir build && cd build                                            && \
    cmake .. -DBUILD_SHARED=ON -DENABLE_DRAFTS=ON                 && \
    cmake --build . -j4                                                             && \
    cmake --install . 

RUN git clone https://github.com/zeromq/cppzmq.git                                  && \
    cd cppzmq && mkdir build && cd build                                            && \
    cmake .. -DENABLE_DRAFTS=ON                                                     && \
    cmake --build . -j4                                                             && \
    cmake --install . 

COPY ./src /src

RUN cd /src && mkdir build && cd build                                              && \
    cmake ..                                                                        && \
    cmake --build . -j4                                                             && \
    cmake --install .

COPY ./scripts /scripts
