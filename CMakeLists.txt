cmake_minimum_required(VERSION 3.10)

# Set the project name
project(Steganography)

# Find OpenCV package
find_package(OpenCV REQUIRED)

# Include directories
include_directories(${OpenCV_INCLUDE_DIRS})

# Set the source files
set(SOURCES
    main.cpp
)

# Create executable
add_executable(steganography ${SOURCES})

# Link OpenCV libraries
target_link_libraries(steganography ${OpenCV_LIBS})
