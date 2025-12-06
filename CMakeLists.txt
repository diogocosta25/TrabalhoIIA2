cmake_minimum_required(VERSION 3.25)
project(TP2_Turismo C)

set(CMAKE_C_STANDARD 11)

# Flags de debug (compatível com MSVC e GCC)
if(MSVC)
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /Zi")
else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
endif()

add_executable(TP2_Turismo
    main.c
    utils.c
    funcao.c      
    algoritmo.c
    algoritmo2.c
    algoritmo3.c
)