cmake_minimum_required(VERSION 3.21)
project(SDStore C)

set(CMAKE_C_STANDARD 99)

add_executable(SDStore ../../Desktop/SDStore-transf/src/sdstore.c ../../Desktop/SDStore-transf/include/request.h ../../Desktop/SDStore-transf/include/reply.h src/aux.c ../../Desktop/SDStore-transf/include/aux.h ../../Desktop/SDStore-transf/src/transformacoes.c ../../Desktop/SDStore-transf/include/transformacoes.h ../../Desktop/SDStore-transf/include/transformacoes.h ../../Desktop/SDStore-transf/include/request.h ../../Desktop/SDStore-transf/src/transformacoes.c ../../Desktop/SDStore-transf/include/transformacoes.h ../../Desktop/SDStore-transf/src/request.c ../../Desktop/SDStore-transf/include/request.h ../../Desktop/SDStore-transf/src/queue.c ../../Desktop/SDStore-transf/include/queue.h src/aux.c src/request.h src/aux.c include/aux.h include/sdstored.h)
