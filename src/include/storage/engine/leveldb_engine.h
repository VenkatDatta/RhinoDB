#ifndef  RHINO_SRC_LEVELDB_ENGINE_H_
#define  RHINO_SRC_LEVELDB_ENGINE_H_
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <unordered_map>
#include <sstream>
#include <functional>
#include "storage/engine/base_kv_engine.h" 
#include "leveldb/db.h"
#include "gflags/gflags.h"
#include <glog/logging.h>
#include <unordered_map>
#include <memory>
#include <vector>
#include <string>
DECLARE_string(db_dir);


namespace RHINO {
template<typename T, typename... Args>
void args_str(std::ostringstream& oss, T&& t, Args&&... args) {
    oss << " " << std::forward<T>(t);
    args_str(oss, std::forward<Args>(args)...);
}

template<typename T>
void args_str(std::ostringstream& oss, T&& t) {
    oss << " " << std::forward<T>(t);
}

template<typename T, typename... Args>
static int leveldb_work(std::function<T(Args...)> f, std::string op, Args&&... args) {
      std::ostringstream oss;
      T ret = f(std::forward<Args>(args)...);
      // args_str(oss, args...);
      if (!ret.ok()) {
          LOG(ERROR) << op << " failed. Args " << oss.str(); 
          return 1;
      }
      return 0;

}

using std::unique_ptr;
class LevelDBEngine : public BaseKVEngine {
    public:
        using LevelOpRet = std::function<leveldb::Status(const leveldb::WriteOptions,
                           const std::string&, const std::string&)>;
        LevelDBEngine() {
        }
        bool init() {
            leveldb::Options options;
            options.create_if_missing = true;
            leveldb::DB* db;
            leveldb::Status status = leveldb::DB::Open(options, "./db", &db);
            if (!status.ok()) {
                LOG(ERROR) << "open leveldb [" << "./db" << "] failed.";
                return false;
            }
            LOG(INFO) << "open leveldb [" << "./db" << "] complete.";
            _db.reset(db);
            return true;
        }
        int insert(const std::string& key, const std::string& value) {
            LevelOpRet f = std::bind(&leveldb::DB::Put, (leveldb::DB*)_db.get(),
                               std::placeholders::_1, std::placeholders::_2,
                               std::placeholders::_3); 
            LOG(INFO) << "kv insert [" << key << "] [" << value << "]";
            return leveldb_work(f, std::string("Put"), leveldb::WriteOptions(), key, value);
        }
        int insert(const std::pair<std::string, std::string>& kv) {
            return insert(kv.first, kv.second);
        }
        bool has_key(const std::string& key) {
            std::string value;
            leveldb::Status status = _db->Get(leveldb::ReadOptions(), key, &value);
            if (status.ok()) {
                return true;
            }
            if (status.IsNotFound()) {
                return false;
            }
            LOG(ERROR) << "some error when has_key [" << key << "]";
            return false;
        }
        std::string operator[] (const std::string& key) {
            std::string value;
            leveldb::Status status = _db->Get(leveldb::ReadOptions(), key, &value);
            if (!status.ok()) {
                LOG(ERROR) << "some error when Get [" << key << "]";
                return "";
            }
            return value;
        }
    private:
        std::unique_ptr<leveldb::DB> _db;
};
}  // namespace TINYKV
#endif