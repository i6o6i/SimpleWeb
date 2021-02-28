cmake_minimum_required(VERSION 3.0)
project (SimpleWeb)

include_directories( src/ )
FILE(GLOB_RECURSE RUN_SRC src/*.cpp)
FILE(GLOB_RECURSE RUN_HEADER src/*.hpp src/*.tlp)
set(RUN_SRCS ${RUN_SRC} ${RUN_HEADER})

#ADD_DEFINITIONS(-DDEBUG)
#set(CMAKE_BUILD_TYPE Debug)
if( CMAKE_BUILD_TYPE MATCHES Debug )
    ADD_DEFINITIONS(-DDEBUG)
ENDIF( CMAKE_BUILD_TYPE MATCHES Debug )

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

file(GLOB_RECURSE TEST_SOURCE test/sw.cpp )
foreach( testsourcefile ${TEST_SOURCE} )
    string( REGEX REPLACE ".*/\(.*\).cpp" "\\1" testname ${testsourcefile} )
    add_executable( ${testname} ${testsourcefile} ${RUN_SRCS})
    target_link_libraries( ${testname} PRIVATE Threads::Threads)
endforeach( testsourcefile ${TEST_SOURCE} )



