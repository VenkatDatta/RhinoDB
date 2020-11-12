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
get_filename_component(hw_proto "service.proto" ABSOLUTE)
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


# Include generated *.pb.h files
include_directories("${CMAKE_CURRENT_BINARY_DIR}")
include_directories(include)

# Targets (client|server)
foreach(_target
  server client)
  add_executable(${_target} "${_target}.cc"
    ${hw_proto_srcs}
    ${hw_grpc_srcs})
  target_link_libraries(${_target}
    ${_REFLECTION}
    ${GFLAGS_LIBRARIES}
    ${GLOG_LIBRARIES}
    ${_GRPC_GRPCPP}
    ${LEVELDB_LIBS}
    ${_PROTOBUF_LIBPROTOBUF})
endforeach()
