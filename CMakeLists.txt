cmake_minimum_required(VERSION 3.17)
project(InitialContract)

if (NOT BEAM_SHADER_SDK)
    set(BEAM_SHADER_SDK $ENV{BEAM_SHADER_SDK})
endif()

if (NOT BEAM_SHADER_SDK)
    message(FATAL_ERROR "BEAM_SHADER_SDK is not defined")
endif()

if (NOT WASI_SDK_PREFIX)
    set(WASI_SDK_PREFIX $ENV{WASI_SDK_PREFIX})
endif()

if (NOT WASI_SDK_PREFIX)
    message(FATAL_ERROR "WASI_SDK_PREFIX is not defined")
endif()

set(CMAKE_MODULE_PATH "${BEAM_SHADER_SDK}/beam/cmake" ${CMAKE_MODULE_PATH})
set(CMAKE_MODULE_PATH "${BEAM_SHADER_SDK}/cmake" ${CMAKE_MODULE_PATH})
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${BEAM_SHADER_SDK})
set(BEAM_PROJECT_SOURCE_DIR ${BEAM_SHADER_SDK}/beam CACHE INTERNAL "")
set(BEAM_PROJECT_BINARY_DIR ${PROJECT_BINARY_DIR}/beam CACHE INTERNAL "")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${BEAM_SHADER_SDK}/beam/bvm)
message(STATUS "CMAKE_SYSTEM_NAME=${CMAKE_SYSTEM_NAME}")
include(ShaderTools)
include_directories(3rdparty/yas/include)
include_directories(shaders/Genetic)

if(CMAKE_SYSTEM_NAME MATCHES "WASI")
    #include(GenerateSID)

    include(ShaderLib)

    set(CMAKE_EXECUTABLE_SUFFIX .wasm)

    string (REPLACE ";" " " BEAM_SHADER_CLANG_OPTIONS_STR "${BEAM_SHADER_CLANG_OPTIONS}")

    add_subdirectory(shaders)
else()

    if (MSVC)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:__cplusplus")
    endif()
    set(beam_SOURCE_DIR ${BEAM_SHADER_SDK}/beam)
    include(AddTest)
    include(Bvm)
    include(AddShader)
    find_package(Boost 1.72 REQUIRED COMPONENTS system filesystem)
    find_package(OpenSSL)
    add_subdirectory(unittests)
endif()



#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -std=c++17 -Wreturn-type -flto -fno-rtti -Wno-inline-new-delete -fno-exceptions -nostartfiles -Wno-invalid-noreturn -Wl -I 3rdparty\yas\include,--no-entry,--allow-undefined,--export=Ctor,--export=Dtor,--export=Method_0,--export=Method_1,--export=Method_2,--export=Method_3")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wreturn-type -flto -fno-rtti -nostartfiles -Wl -I 3rdparty\yas\include,--no-entry,--allow-undefined,--export=Method_0,--export=Method_1") #-nostdlib ")
