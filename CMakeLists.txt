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
	src
	modules/drivers 
	modules/kmz80 
	modules/emu2149 
	modules/emu2212 
	modules/emu2413 
	modules/emu8950 
	modules/emu76489
	modules/libusb-1.0.20/include/libusb-1.0 
)

file(GLOB SOURCES src/*.c src/**/*.c sound_cortex.c)

add_library(kss STATIC ${SOURCES})
file(GLOB LIBUSB modules/libusb-1.0.20/MS32/static/libusb-1.0.lib)
target_link_libraries(kss emu2212 emu2413 emu8950 emu76489 kmz80 ${LIBUSB} legacy_stdio_definitions)

add_executable(kss2wav EXCLUDE_FROM_ALL src/kss2wav.c)
target_link_libraries(kss2wav kss)

add_executable(kssplay EXCLUDE_FROM_ALL kssplay_i2c.c)
target_link_libraries(kssplay kss)
