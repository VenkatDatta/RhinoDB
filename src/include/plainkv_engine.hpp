#ifndef  RHINO_SRC_PLAIN_KV_ENGINE_H_
#define  RHINO_SRC_PLAIN_KV_ENGINE_H_
#include <stdio.h>
#include <string>
#include <unordered_map>
#include "base_kv_engine.hpp" 
namespace RHINO {
class PlainEngine : public BaseKVEngine {
    public:
        bool init() {
            return true;
        }
        int insert(const std::string& key, const std::string& value) {
            _map.insert(std::make_pair(key, value));
            return 0;
        }
        int insert(const std::pair<std::string, std::string>& kv) {
            _map.insert(kv);
            return 0;
        }
        bool has_key(const std::string& key) {
            if (_map.find(key) != _map.end()) {
                return true;
            }
            return false;
        } 
        std::string operator[] (const std::string& key) {
            return _map[key];
        } 
    private:
        std::unordered_map<std::string, std::string> _map;

};
}  // namespace RHINO
#endif
