cmake_minimum_required(VERSION 3.17)
project(Thread_cancellation C)

set(CMAKE_C_STANDARD 99)

add_executable(Thread_cancellationV1 mainV1.c)
add_executable(Thread_cancellationV2 mainV2.c)
add_executable(Test test.c)