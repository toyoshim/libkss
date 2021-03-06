cmake_minimum_required(VERSION 3.4)

set(CMAKE_C_FLAGS "-O3 -Wall")

if(NOT TARGET emu2149) 
	add_subdirectory(modules/emu2149) 
endif()

if(NOT TARGET emu2212)
	add_subdirectory(modules/emu2212)
endif()

if(NOT TARGET emu2413)
	add_subdirectory(modules/emu2413)
endif()

if(NOT TARGET emu8950)
	add_subdirectory(modules/emu8950)
endif()

if(NOT TARGET emu76489)
	add_subdirectory(modules/emu76489)
endif()

if(NOT TARGET kmz80)
	add_subdirectory(modules/kmz80)
endif()

include_directories(
	modules/drivers 
	modules/kmz80 
	modules/emu2149 
	modules/emu2212 
	modules/emu2413 
	modules/emu8950 
	modules/emu76489
)

file(GLOB SOURCES src/*.c src/**/*.c)

add_library(kss STATIC ${SOURCES})
target_link_libraries(kss emu2149 emu2212 emu2413 emu8950 emu76489 kmz80)

add_executable(kss2wav EXCLUDE_FROM_ALL src/kss2wav.c)
target_link_libraries(kss2wav kss)
