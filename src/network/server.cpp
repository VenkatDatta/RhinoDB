#include <stdio.h>
#include <stdlib.h>
#include <unordered_map>
#include "storage/old.h" 
#include "grpcpp/grpcpp.h"
#include "kvservice.grpc.pb.h"
#include <glog/logging.h>
#include "storage/engine/leveldb_engine.h"
#include "storage/engine/plainkv_engine.h"
using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using RHINO::ReqKey;
using RHINO::ReqKeyValue;
using RHINO::RespValue;

DEFINE_string(db_dir, "./db", "local db dir");
DEFINE_string(db_type, "plain", "local db engine type");

// Logic and data behind the server's behavior.
template <class KVEngine>
class KVServiceImpl final : public RHINO::KVService::Service {
public:
  KVServiceImpl() {
      _kv_engine = new KVEngine();
  }
  virtual ~KVServiceImpl() {}
  int init() {
      if (_kv_engine->init() != 0) {
         return -1;
      }
      return 0;
  }
  Status Get(ServerContext* context, const ReqKey* request,
                  RespValue* reply) override {
    if (request->key_size() == 0) {
        LOG(ERROR) << "Reqid=" << request->qid() << " miss request key.";
        return Status::OK;
    }
    LOG(INFO) << "Reqid=" << request->qid() << " key_size=" << request->key_size()
              << " Sample key=" << request->key(0);
    reply->clear_value();
    for (int i = 0; i < request->key_size(); ++i) {
        std::string value;
        if (_kv_engine->get(request->key(i), &value) == 0 && value.length() > 0) {
            reply->add_value(value);
        }
    }
    return Status::OK;
  }
  Status Insert(ServerContext* context, const ReqKeyValue* req_key_value,
                 RespValue* relay) override {
      int key_size = req_key_value->key_size();
      int value_size = req_key_value->value_size();
      if (key_size != value_size) {
          LOG(WARNING) << "Insert cancelled for key value size not match [" << key_size << "/" << value_size << "]";
          return Status::CANCELLED;
      }
      LOG(INFO) << "Reqid=" << req_key_value->qid() << " key_size=" << req_key_value->key_size()
                << " Sample key=" << req_key_value->key(0);
      for (int i = 0; i < key_size; ++i) {
          _kv_engine->insert(req_key_value->key(i), req_key_value->value(i));
      }
      return Status::OK;
  }
private:
  KVEngine* _kv_engine;
};

#define SERVICE_IMPL_PTR(ENGINE_TYPE) \
  do { \
      auto service_impl_ptr = new(std::nothrow)KVServiceImpl<RHINO::Rhino<ENGINE_TYPE>>(); \
      if (!service_impl_ptr->init()) { \
          LOG(WARNING) << "service_impl_ptr init failed."; \
          return -1; \
      } \
      service.reset(service_impl_ptr); \
  } while(0);

int RunServer() {
  std::string server_address("0.0.0.0:50051");
  KVServiceImpl<RHINO::Rhino<RHINO::LevelDBEngine>> *service =  new(std::nothrow)KVServiceImpl<RHINO::Rhino<RHINO::LevelDBEngine>>();

  if (!service->init()) {
    LOG(WARNING) << "KVServiceImpl init failed.";
    return -1;
  }

  ServerBuilder builder;
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(service);
  std::unique_ptr<Server> server(builder.BuildAndStart());
  LOG(INFO) << "Server listening on " << server_address;
  server->Wait();
  return 0;
}

int main(int argc, char** argv) {
  RunServer();
  return 0;
}