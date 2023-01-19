# CMakeLists files in this project can
# refer to the root source directory of the project as ${HELLO_SOURCE_DIR} and
# to the root binary directory of the project as ${HELLO_BINARY_DIR}.
cmake_minimum_required (VERSION 3.1)
project (nanokontrol2_driver)

add_compile_options(-D__LINUX_ALSA__ -lasound -lpthread)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable (
	bk_nanokontrol 
	src/main.cpp
	rtmidi/RtMidi.cpp
	)

find_package(ALSA REQUIRED)    

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)                                               
include_directories(${ALSA_INCLUDE_DIRS})                  

set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.45.0 COMPONENTS filesystem regex)

target_link_libraries (bk_nanokontrol ${ALSA_LIBRARIES} Threads::Threads ${Boost_LIBRARIES}) 

include_directories(${Boost_INCLUDE_DIRS})
target_include_directories(bk_nanokontrol PRIVATE rtmidi)