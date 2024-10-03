#include <grpcpp/grpcpp.h>
#include <grpcpp/server_builder.h>
#include "generated/simple.grpc.pb.h"
#include <iostream>
#include <memory>
#include <thread>
#include <queue>

// Class that handles one request
class AsyncServiceImpl final {
public:
    AsyncServiceImpl() : cq_(nullptr), service_(), server_(nullptr) {}

    void Run() {
        grpc::ServerBuilder builder;
        builder.AddListeningPort("0.0.0.0:50051", grpc::InsecureServerCredentials());
        builder.RegisterService(&service_);
        cq_ = builder.AddCompletionQueue();
        server_ = builder.BuildAndStart();
        std::cout << "Server listening on port 50051" << std::endl;

        HandleRpcs();
    }

private:
    // Queue to store requests that will be handled later
    std::queue<std::pair<simple::Request, grpc::ServerAsyncResponseWriter<simple::Response>>> deferred_requests_;

    // Start the loop to handle RPCs
    void HandleRpcs() {
        new CallData(&service_, cq_.get(), deferred_requests_);

        void* tag;
        bool ok;
        while (true) {
            cq_->Next(&tag, &ok);
             std::cout << "Next called, ok is: " << ok << "tag is: " << tag << std::endl; 
            if (ok) {
                static_cast<CallData*>(tag)->Proceed();
            }
        }
    }

    class CallData {
    public:
        CallData(simple::SimpleService::AsyncService* service,
                 grpc::ServerCompletionQueue* cq,
                 std::queue<std::pair<simple::Request, grpc::ServerAsyncResponseWriter<simple::Response>>>& deferred_requests)
            : service_(service), cq_(cq), responder_(&ctx_), status_(CREATE), deferred_requests_(deferred_requests) {
            Proceed();
        }

        void Proceed() {
            if (status_ == CREATE) {
                status_ = PROCESS;
                service_->RequestSimpleRequest(&ctx_, &request_, &responder_, cq_, cq_, this);
            } else if (status_ == PROCESS) {
                // Push the request aside for later handling
                deferred_requests_.emplace(request_, std::move(responder_));
                std::cout << "Request received and deferred: " << request_.message() << std::endl;

                // Spawn a new CallData instance to process the next request
                new CallData(service_, cq_, deferred_requests_);

                status_ = FINISH;
            } else {
                delete this;
            }
        }

    private:
        simple::SimpleService::AsyncService* service_;
        grpc::ServerCompletionQueue* cq_;
        grpc::ServerContext ctx_;

        simple::Request request_;
        grpc::ServerAsyncResponseWriter<simple::Response> responder_;

        enum CallStatus { CREATE, PROCESS, FINISH };
        CallStatus status_;

        // Queue to defer the requests
        std::queue<std::pair<simple::Request, grpc::ServerAsyncResponseWriter<simple::Response>>>& deferred_requests_;
    };

    std::unique_ptr<grpc::ServerCompletionQueue> cq_;
    simple::SimpleService::AsyncService service_;
    std::unique_ptr<grpc::Server> server_;
};

int main() {
    AsyncServiceImpl server;
    server.Run();
    return 0;
}

