cmake_minimum_required(VERSION 2.8)
project(ros_wrapper)

set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_CXX_FLAGS "-std=c++14")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -Wall -g")

find_package (catkin REQUIRED COMPONENTS
   roscpp
   rospy
   std_msgs
   cv_bridge
   image_transport
   tf
   sensor_msgs
   message_generation
)

find_package(OpenCV REQUIRED)
MESSAGE("OPENCV VERSION:" ${OpenCV_VERSION})

find_package(Eigen3 REQUIRED)
MESSAGE("Eigen3 VERSION:" ${Eigen3_VERSION})
find_package(Pangolin REQUIRED)
find_package(Sophus REQUIRED)

catkin_package (
   CATKIN_DEPENDS roscpp rospy std_msgs cv_bridge image_transport tf sensor_msgs message_runtime
   LIBRARIES {PROJECT_NAME}
)

include_directories(
   ${PROJECT_SOURCE_DIR}/include
   ${EIGEN3_INCLUDE_DIR}
   ${catkin_INCLUDE_DIRS}
   ${Pangolin_INCLUDE_DIRS}
   ${Sophus_INCLUDE_DIRS}
)

# include_directories(${PROJECT_SOURCE_DIR}/../ov_core/src)
# MESSAGE("include_directories:")

# Build ROS nodes
set(LIBS
   ${EIGEN3_LIBS}
   ${catkin_LIBRARIES}
   ${Pangolin_LIBRARIES}
   ${OpenCV_LIBS}
   ${Sophus_LIBS}
)

add_executable (${PROJECT_NAME}_stereo_inertial
   src/write_data.cc
   src/common.cc
   src/txt_writer.cc
   src/Sync.cc
)
target_link_libraries(${PROJECT_NAME}_stereo_inertial
   ${LIBS} #ov_core_lib
)
