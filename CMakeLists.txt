cmake_minimum_required(VERSION 3.16)
project(urchinEngineTest)

if (CMAKE_BUILD_TYPE MATCHES "Debug")
    add_definitions(-DAPP_DEBUG)
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

if(USE_ASAN MATCHES "true")
    set(CMAKE_CXX_FLAGS -fsanitize=address)
    set(CMAKE_LINKER_FLAGS -fsanitize=address)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_CXX_STANDARD 20)

#Urchin engine
include_directories(../urchinEngine/common/src ../urchinEngine/3dEngine/src ../urchinEngine/physicsEngine/src ../urchinEngine/soundEngine/src ../urchinEngine/aiEngine/src ../urchinEngine/aggregation/src)
add_subdirectory(../urchinEngine urchinEngine)
list(TRANSFORM URCHIN_HEADERS PREPEND ../urchinEngine/)
if (WIN32)
    file(REMOVE_RECURSE ${CMAKE_CURRENT_SOURCE_DIR}\\resources\\shaders\\)
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}\\..\\urchinEngine\\3dEngine\\resources\\shaders\\spirv\\ DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/resources/shaders)
    add_custom_target(syncShaders ALL cd DEPENDS urchin3dEngine) #ignore
else()
    include_directories(BEFORE ${CMAKE_BINARY_DIR}/deps/curl/include)
    link_directories (BEFORE ${CMAKE_BINARY_DIR}/deps/curl/lib)
    add_custom_target(syncShaders ALL rsync -r ${CMAKE_CURRENT_SOURCE_DIR}/../urchinEngine/3dEngine/resources/shaders/spirv/ ${CMAKE_CURRENT_SOURCE_DIR}/resources/shaders/ DEPENDS urchin3dEngine)
endif()

#SDL
if (WIN32)
    SET(SDL_LIB libSDL2.a gdi32 winmm setupapi imm32 version)
else()
    SET(X11_LIB X11) #not static because X11 can use libraries not mentioned in its dependencies
    SET(SDL_LIB libSDL2.a ${X11_LIB})
endif()

if (NOT WIN32)
    set(LINK_OPT -rdynamic) #rdynamic: necessary for SignalHandler to work correctly
endif()

add_definitions(-fno-stack-protector -ffast-math -Wall -Wextra -Wpedantic -Wconversion -Werror)
include_directories(src)

file(GLOB_RECURSE SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")
if (WIN32)
    add_executable(urchinEngineTest WIN32 ${SOURCE_FILES})
else()
    add_executable(urchinEngineTest ${SOURCE_FILES})
endif()
add_dependencies(urchinEngineTest syncShaders)
target_link_libraries(urchinEngineTest -static-libstdc++ ${LINK_OPT} ${SDL_LIB} urchinCommon urchin3dEngine urchinPhysicsEngine urchinSoundEngine urchinNetworkEngine urchinAIEngine urchinAggregation)
target_precompile_headers(urchinEngineTest PRIVATE ${URCHIN_HEADERS})
