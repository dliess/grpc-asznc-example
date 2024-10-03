cmake_minimum_required(VERSION 3.15)
project(AsyncGRPCServerClient CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(GRPC_INSTALL_DIR "${CMAKE_SOURCE_DIR}/grpc_install")
list(APPEND CMAKE_PREFIX_PATH "${GRPC_INSTALL_DIR}")
message(STATUS "CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}")

# finding absl is needed here because otherwise it will find incomplete lib in system
find_package(absl 20240116 CONFIG REQUIRED)
# finding exact version of protobuf is needed here because otherwise it will find incomplete lib in system
find_package(Protobuf 27.2.0 CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)

# TODO:Proto file generation
# set(PROTO_SRC simple.proto)
# protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_SRC})
# grpc_generate_cpp(GRPC_SRCS GRPC_HDRS ${PROTO_SRC})

set(PROTO_SRCS "generated/simple.pb.cc")
set(PROTO_HDRS "generated/simple.pb.h")
set(GRPC_SRCS "generated/simple.grpc.pb.cc")
set(GRPC_HDRS "generated/simple.grpc.pb.h")

# Server executable
add_executable(async_server async_server.cpp ${PROTO_SRCS} ${GRPC_SRCS})
target_link_libraries(async_server 
   PRIVATE 
      gRPC::grpc++ 
      protobuf::libprotobuf
)

# Client executable
add_executable(sync_client sync_client.cpp ${PROTO_SRCS} ${GRPC_SRCS})
target_link_libraries(sync_client 
   PRIVATE 
      gRPC::grpc++
      protobuf::libprotobuf
)
