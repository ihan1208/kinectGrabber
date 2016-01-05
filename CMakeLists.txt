# The name of my project is "Kinect2 Grabber". CMakeLists files in this project can 
# refer to the root source directory of the project as ${PEOPLETRAINER_SOURCE_DIR} and 
# to the root binary directory of the project as ${PEOPLETRAINER_BINARY_DIR}. 
cmake_minimum_required (VERSION 2.8) 

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

# The version number.
# must go before the project command
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Configs" FORCE)
if(DEFINED CMAKE_BUILD_TYPE)
  set_property( CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS ${CMAKE_CONFIGURATION_TYPES} )
endif()
set (Kinect Grabber_VERSION_MAJOR 0)
set (Kinect Grabber_VERSION_MINOR 1)
set(CMAKE_BUILD_TYPE Release)
if(NOT CMAKE_BUILD_TYPE) 
    set(CMAKE_BUILD_TYPE Release)
endif(NOT CMAKE_BUILD_TYPE)

project ("Kinect Grabber")

find_package(OpenCV REQUIRED)
find_package(KinectSDK REQUIRED)

include_directories( ${OpenCV_INCLUDE_DIRS} )
include_directories( ${KinectSDK_INCLUDE_DIR} )
include_directories( "include")

add_executable(KinectGrabber src/KinectManager.cpp src/Main.cpp) 
target_link_libraries(KinectGrabber ${OpenCV_LIBS})
target_link_libraries(KinectGrabber ${KinectSDK_LIBRARIES})

install(TARGETS KinectGrabber DESTINATION bin)
