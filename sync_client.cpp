#include <grpcpp/grpcpp.h>
#include "generated/simple.grpc.pb.h"
#include <iostream>

int main() {
    auto channel = grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials());
    std::unique_ptr<simple::SimpleService::Stub> stub = simple::SimpleService::NewStub(channel);

    // Prepare request
    simple::Request request;
    request.set_message("Hello from the client!");

    // Prepare response and context
    simple::Response response;
    grpc::ClientContext context;

    // Perform the RPC call
    grpc::Status status = stub->SimpleRequest(&context, request, &response);

    if (status.ok()) {
        std::cout << "Server replied: " << response.reply() << std::endl;
    } else {
        std::cerr << "RPC failed" << std::endl;
    }

    return 0;
}

