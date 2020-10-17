cmake_minimum_required(VERSION 3.7)
project(urchinEngineTest)

if (CMAKE_BUILD_TYPE MATCHES "Release")
    add_definitions(-DNDEBUG) #disable assert
endif()

if(USE_O3 MATCHES "true")
    set(CMAKE_CXX_FLAGS "-O3")
elseif(USE_O2 MATCHES "true")
    set(CMAKE_CXX_FLAGS "-O2")
elseif(USE_O1 MATCHES "true")
    set(CMAKE_CXX_FLAGS "-O1")
elseif(USE_O0 MATCHES "true")
    set(CMAKE_CXX_FLAGS "-O0")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_CXX_STANDARD 17)

add_definitions(-ffast-math -Wall -Wextra -Wpedantic -Werror)
include_directories(src)

#Urchin engine
include_directories(../urchinEngine/common/src ../urchinEngine/3dEngine/src ../urchinEngine/physicsEngine/src ../urchinEngine/soundEngine/src ../urchinEngine/AIEngine/src ../urchinEngine/mapHandler/src)
if (CMAKE_BUILD_TYPE MATCHES "Debug")
    link_directories(../urchinEngine/.build/debug/common ../urchinEngine/.build/debug/3dEngine ../urchinEngine/.build/debug/physicsEngine ../urchinEngine/.build/debug/soundEngine ../urchinEngine/.build/debug/AIEngine ../urchinEngine/.build/debug/mapHandler)
else()
    link_directories(../urchinEngine/.build/release/common ../urchinEngine/.build/release/3dEngine ../urchinEngine/.build/release/physicsEngine ../urchinEngine/.build/release/soundEngine ../urchinEngine/.build/release/AIEngine ../urchinEngine/.build/release/mapHandler)
endif()
add_subdirectory(../urchinEngine urchinEngine)

#Copy shaders in project
if (WIN32)
    file(REMOVE_RECURSE ${CMAKE_CURRENT_SOURCE_DIR}\\resources\\shaders\\)
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}\\..\\urchinEngine\\3dEngine\\resources\\shaders DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}\\resources\\)
    add_custom_target(syncShaders cd) #ignore
else()
    add_custom_target(syncShaders rsync -r ${CMAKE_CURRENT_SOURCE_DIR}/../urchinEngine/3dEngine/resources/shaders/* ${CMAKE_CURRENT_SOURCE_DIR}/resources/shaders/)
endif()

#GLFW
if (WIN32)
    SET(GLFW_LIB glfw3 gdi32 winmm)
else()
    SET(GLFW_LIB glfw)
endif()

#Executable
file(GLOB_RECURSE SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")
if (WIN32)
    add_executable(urchinEngineTest WIN32 ${SOURCE_FILES})
else()
    add_executable(urchinEngineTest ${SOURCE_FILES})
endif()
add_dependencies(urchinEngineTest syncShaders)
target_link_libraries(urchinEngineTest ${GLFW_LIB} urchinCommon urchin3dEngine urchinPhysicsEngine urchinSoundEngine urchinAIEngine urchinMapHandler)