FROM ubuntu:22.04 as base

ENV DEBIAN_FRONTEND noninteractive

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
    vim \
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

RUN apt-get update                                      && \
    apt-get upgrade --yes                               && \
    apt-get install --yes \
    bc 