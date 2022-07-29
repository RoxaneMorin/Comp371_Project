cmake_minimum_required(VERSION 3.11)

project(Final_Project)

include(ExternalProject)

# Set install directory
set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/dist CACHE PATH ${CMAKE_SOURCE_DIR}/dist FORCE)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")

if(MSVC OR XCODE)
    set(CMAKE_CONFIGURATION_TYPES "Debug" CACHE STRING "Debug" FORCE)
endif()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

find_package(OpenGL REQUIRED COMPONENTS OpenGL)

include(BuildGLEW)
include(BuildGLFW)
include(BuildGLM)

# assignment02
set(EXEC project)

set(ASSETS assets)

file(GLOB SRC src/*.cpp)

add_executable(${EXEC} ${SRC})

target_include_directories(${EXEC} PRIVATE include)

target_link_libraries(${EXEC} OpenGL::GL glew_s glfw glm)

list(APPEND BIN ${EXEC})
# end assignment02

# install files to install location
install(TARGETS ${BIN} DESTINATION ${CMAKE_INSTALL_PREFIX})
install(DIRECTORY ${ASSETS} DESTINATION ${CMAKE_INSTALL_PREFIX})

#set_property(TARGET ${BIN} PROPERTY CXX_STANDARD 20)
#set_property(TARGET ${BIN} PROPERTY C_STANDARD 17)