#include <assert.h>
#include <errno.h>
#include <chrono>
#include <iostream>
#include <fstream>
#include <memory>
#include <random>
#include <string>
#include <thread>

#include <grpc/grpc.h>
#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>
#include <glog/logging.h>
#include "kvservice.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReader;
using grpc::ClientReaderWriter;
using grpc::ClientWriter;
using grpc::Status;
using RHINO::KVService;

class KVClient {
 public:
  KVClient(std::shared_ptr<Channel> channel)
      : stub_(KVService::NewStub(channel)),
        qid_(0) {
  }

  int get(const std::string& key, std::string* value) {
    ClientContext context;
    RHINO::RawGetRequest req_key;
    req_key.set_qid(qid_++);
    req_key.add_key(key);
    RHINO::RawGetResponse rsp;
    Status status = stub_->RawGet(&context, req_key, &rsp);
    if (!status.ok()) {
        LOG(ERROR) << "kv get [" << key << "] failed."; 
        return -1;
    }
    *value = rsp.value(0);
    return 0;
  }


  int txnGet(const std::string& key, std::string* value) {
    ClientContext context;
    RHINO::GetRequest req_key;
    req_key.set_version(0);
    req_key.set_key(key);
    RHINO::GetResponse rsp;
    Status status = stub_->Get(&context, req_key, &rsp);
    if (!status.ok()) {
        LOG(ERROR) << "kv get [" << key << "] failed."; 
        return -1;
    }
    *value = rsp.value(0);
    return 0;
  }

  int insert(const std::string& key, const std::string& value) {
    ClientContext context;
    RHINO::RawPutRequest req_key_value;
    req_key_value.set_qid(qid_++);
    req_key_value.add_key(key);
    req_key_value.add_value(value);
    RHINO::RawPutResponse rsp_value;
    Status status = stub_->RawPut(&context, req_key_value, &rsp_value);
    if (!status.ok()) {
        LOG(ERROR) << "kv insert [" << key << "] [" << value << "] failed.";
        return -1;
    }
    return 0;
  }

  int txn_put(const std::string& key, const std::string& value) {
    ClientContext context;
    RHINO::PutRequest req_key_value;

    req_key_value.set_key(key);
    req_key_value.set_value(value);
    RHINO::PutResponse rsp_value;
    Status status = stub_->Put(&context, req_key_value, &rsp_value);
    if (!status.ok()) {
        LOG(ERROR) << "kv insert [" << key << "] [" << value << "] failed.";
        return -1;
    }
    return 0;
  }

 private:
  std::unique_ptr<KVService::Stub> stub_;
  int64_t qid_;
};

static const char* KEY_LIST_FILE = "out_key.txt";
static const char* VALUE_LIST_FILE = "out_value.txt";

int load_key_value_list(std::vector<std::string>* keys, std::vector<std::string>* values) {
    std::ifstream key_list_fd;
    key_list_fd.open(KEY_LIST_FILE);
    if (not key_list_fd.is_open()) {
        LOG(ERROR) << "open key_list_file " << KEY_LIST_FILE << " failed";
        return -1;
    }

    std::ifstream value_list_fd;
    value_list_fd.open(VALUE_LIST_FILE, std::ifstream::in);
    if (not value_list_fd.is_open()) {
        LOG(ERROR) << "open value_list_file " << VALUE_LIST_FILE << " failed";
        return -1;
    }
    while (key_list_fd.good()) {
        std::string tmp_key;
        std::getline(key_list_fd, tmp_key);
        if (tmp_key.length() > 0) {
            (*keys).push_back(tmp_key);
        }
    }
    while (value_list_fd.good()) {
        std::string tmp_value;
        std::getline(value_list_fd, tmp_value);
        if (tmp_value.length() > 0) {
            (*values).push_back(tmp_value);
        }
    }
    assert(keys->size() == values->size());
    LOG(INFO) << "Loaded " << keys->size() << " keys & " << values->size() << " values";
    return 0;
}

int main(int argc, char** argv) {
  KVClient kv_client(
      grpc::CreateChannel("localhost:50051",
                          grpc::InsecureChannelCredentials()));
  std::vector<std::string> keys;
  std::vector<std::string> values;
  if (load_key_value_list(&keys, &values) != 0) {
      LOG(ERROR) << "Load key_value_list failed.";
      return -1;
  }
  // single thread insert
  for (int i = 0; i < keys.size(); ++i) {
    kv_client.insert(keys[i], values[i]);
  }

  return 0;
}

/* vim: set expandtab ts=2 sw=2 sts=2 tw=80: */
