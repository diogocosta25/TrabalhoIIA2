cmake_minimum_required(VERSION 3.25)
project(TP2_Turismo C)

set(CMAKE_C_STANDARD 11)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")

add_executable(TP2_Turismo
    main.c
    utils.c
    funcao.c      
    algoritmo.c
)