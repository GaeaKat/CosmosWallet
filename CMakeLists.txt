  
cmake_minimum_required(VERSION 3.1...3.17)
project(CosmosWallet VERSION 1.0
        DESCRIPTION "Api for stuff"
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
# Set cmake as import path for Find*.cmake files
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

### Require out-of-source builds
file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" LOC_PATH)
if(EXISTS "${LOC_PATH}")
    message(FATAL_ERROR "You cannot build in a source directory (or any directory with a CMakeLists.txt file). Please make a build subdirectory. Feel free to remove CMakeCache.txt and CMakeFiles.")
endif()

## Check if GTests is installed. If not, install it

option(PACKAGE_TESTS "Build the tests" ON)
if(NOT TARGET gtest_main AND PACKAGE_TESTS)
    # Download and unpack googletest at configure time
    configure_file(cmake/gtests.txt.in googletest-download/CMakeLists.txt)
    execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
            RESULT_VARIABLE result
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
    if(result)
        message(FATAL_ERROR "CMake step for googletest failed: ${result}")
    endif()
    execute_process(COMMAND ${CMAKE_COMMAND} --build .
            RESULT_VARIABLE result
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
    if(result)
        message(FATAL_ERROR "Build step for googletest failed: ${result}")
    endif()

    # Prevent overriding the parent project's compiler/linker
    # settings on Windows
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

    # Add googletest directly to our build. This defines
    # the gtest and gtest_main targets.
    add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
            ${CMAKE_CURRENT_BINARY_DIR}/googletest-build)
    include(GoogleTest)

endif()
if(NOT TARGET gigamonkey)
    configure_file(cmake/gigamonkey.txt.in gigamonkey-download/CMakeLists.txt)
    execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
            RESULT_VARIABLE result
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/gigamonkey-download )
    if(result)
        message(FATAL_ERROR "CMake step for gigamonkey failed: ${result}")
    endif()
    execute_process(COMMAND ${CMAKE_COMMAND} --build .
            RESULT_VARIABLE result
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/gigamonkey-download )
    if(result)
        message(FATAL_ERROR "Build step for gigamonkey failed: ${result}")
    endif()

    # Add googletest directly to our build. This defines
    # the gtest and gtest_main targets.
    add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/gigamonkey-src
            ${CMAKE_CURRENT_BINARY_DIR}/gigamonkey-build)
endif()
find_package(yaml-cpp)

find_package(Getdns REQUIRED)
if(GETDNS_INCLUDE_DIR)
    message(STATUS "GetDNS INCLUDE Dirs: ${GETDNS_INCLUDE_DIR}")
    include_directories(${GETDNS_INCLUDE_DIR})
endif()

## Enable testing
include(CTest)
#mongodb cxx wrapper
find_package(libmongocxx REQUIRED)
find_package(libbsoncxx REQUIRED)
include_directories(${LIBMONGOCXX_LIBRARIES})
include_directories(${LIBBSONCXX_LIBRARIES})
include_directories(${LIBMONGOCXX_INCLUDE_DIRS})
include_directories(${LIBBSONCXX_INCLUDE_DIRS})
if(PACKAGE_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()
add_executable(CosmosWallet main.cpp 
        src/matterpool_timechain.cpp
        src/MatterpoolAPI.cpp
        src/hd_wallet.cpp
        src/types.cpp
        src/mongodb_db.cpp
        src/utils/paymail/capability.cpp
        src/utils/paymail/paymail.cpp)
target_include_directories(CosmosWallet PUBLIC include)
target_link_libraries(CosmosWallet gigamonkey yaml-cpp ${GETDNS_LIBRARIES} ${LIBMONGOCXX_LIBRARIES} ${LIBBSONCXX_LIBRARIES})
