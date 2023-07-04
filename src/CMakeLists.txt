cmake_minimum_required(VERSION 3.12)
project(zmq_perf_test)

set(CMAKE.cpp_STANDARD 14)
set(CMAKE.cpp_STANDARD_REQUIRED ON)

# Find packages
find_package(ZeroMQ REQUIRED)
find_package(cppzmq REQUIRED)
find_package(Threads REQUIRED)

add_executable(pull
    pull.cpp
)

add_executable(push
    push.cpp
)

add_executable(push_mt
    push_mt.cpp
)

add_executable(push_mt_mc
    push_mt_mc.cpp
)

foreach(target push_mt push pull push_mt_mc)
    target_link_libraries(${target} PUBLIC Threads::Threads libzmq cppzmq)
    target_include_directories(${target} PUBLIC ${ZeroMQ_INCLUDE_DIR})
endforeach()
