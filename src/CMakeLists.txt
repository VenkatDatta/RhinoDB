# Minimum CMake version
cmake_minimum_required(VERSION 3.15)

# Protobuf
set(protobuf_MODULE_COMPATIBLE TRUE)
find_package(Protobuf CONFIG REQUIRED)
message(STATUS "Using protobuf ${protobuf_VERSION}")

# Protobuf-compiler
set(_PROTOBUF_PROTOC $<TARGET_FILE:protobuf::protoc>)

# gRPC
find_package(gRPC CONFIG REQUIRED)
message(STATUS "Using gRPC ${gRPC_VERSION}")
set(_GRPC_GRPCPP gRPC::grpc++)
set(_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:gRPC::grpc_cpp_plugin>)

# Set LevelDB CMake module
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMakeScripts)



#   Google-gflags
find_package(Gflags REQUIRED)
include_directories(${GFLAGS_INCLUDE_DIRS})

#	Google-glog
find_package(Glog REQUIRED)
include_directories(${GLOG_INCLUDE_DIRS})

# Proto file
get_filename_component(hw_proto network/service.proto ABSOLUTE)
get_filename_component(hw_proto_path "${hw_proto}" PATH)

# Generated sources
set(hw_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/service.pb.cc")
set(hw_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/service.pb.h")
set(hw_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/service.grpc.pb.cc")
set(hw_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/service.grpc.pb.h")
add_custom_command(
      OUTPUT "${hw_proto_srcs}" "${hw_proto_hdrs}" "${hw_grpc_srcs}" "${hw_grpc_hdrs}"
      COMMAND ${_PROTOBUF_PROTOC}
      ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${hw_proto_path}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${hw_proto}"
      DEPENDS "${hw_proto}")

# Storage Proto file
get_filename_component(st_proto engine/storage.proto ABSOLUTE)
get_filename_component(st_proto_path "${st_proto}" PATH)

# Generated sources
set(st_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/storage.pb.cc")
set(st_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/storage.pb.h")
add_custom_command(
      OUTPUT "${st_proto_srcs}" "${st_proto_hdrs}"
      COMMAND ${_PROTOBUF_PROTOC}
      ARGS 
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${st_proto_path}"
        "${st_proto}"
      DEPENDS "${st_proto}")

#    LevelDB
find_package(LevelDB REQUIRED)
include_directories(${LEVELDB_INCLUDE})
if(LEVELDB_FOUND)
    find_package(Snappy REQUIRED)
    include_directories(${SNAPPY_INCLUDE_DIR})
    include_directories(${GFLAGS_INCLUDE_DIRS})
    set(LEVELDB_LIBS
        ${LEVELDB_LIBS}
        ${SNAPPY_LIBS}
    )
endif()

file(GLOB_RECURSE sources ${PROJECT_SOURCE_DIR}/src/*.cc ${PROJECT_SOURCE_DIR}/src/*.h ${PROJECT_SOURCE_DIR}/src/*.hpp)

# Include generated *.pb.h files
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

include_directories(include)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# Targets (client|server)
add_executable(client network/client.cpp
    ${hw_proto_srcs}
    ${hw_grpc_srcs} ${st_proto_srcs} ${sources})
target_link_libraries(client
    ${_REFLECTION}
    ${GFLAGS_LIBRARIES}
    ${GLOG_LIBRARIES}
    ${_GRPC_GRPCPP}
    ${LEVELDB_LIBS}
    ${_PROTOBUF_LIBPROTOBUF})

add_executable(server network/server.cpp
    ${hw_proto_srcs}
    ${hw_grpc_srcs} ${st_proto_srcs} ${sources})
  target_link_libraries(server
    ${_REFLECTION}
    ${GFLAGS_LIBRARIES}
    ${GLOG_LIBRARIES}
    ${_GRPC_GRPCPP}
    ${LEVELDB_LIBS}
    ${_PROTOBUF_LIBPROTOBUF})