project(sort_HK_dataset C CXX)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_package(OpenCV 3 REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

set(EXTERNAL_LIBRARIES
	${OpenCV_LIBS}
	${Boost_LIBRARIES}

)

add_executable(sort_Dataset sort.cpp)
target_link_libraries(sort_Dataset ${EXTERNAL_LIBRARIES})
