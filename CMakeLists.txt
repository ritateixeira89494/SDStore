cmake_minimum_required(VERSION 3.21)
project(SDStore C)

set(CMAKE_C_STANDARD 99)

add_executable(SDStore src/sdstore.c include/request.h include/reply.h src/aux.c include/aux.h src/filtros.c include/filtros.h include/filtros.h include/request.h src/filtros.c include/filtros.h src/request.c include/request.h src/queue.c include/queue.h)
