#include "storage/engine/plainkv_engine.h"

namespace RHINO {
    bool PlainEngine::init() {
        return true;
    }
    int PlainEngine::insert(const std::string& key, const std::string& value) {
        _map.insert(std::make_pair(key, value));
        return 0;
    }
    int PlainEngine::insert(const std::pair<std::string, std::string>& kv) {
        _map.insert(kv);
        return 0;
    }
    bool PlainEngine::has_key(const std::string& key) {
        if (_map.find(key) != _map.end()) {
            return true;
        }
        return false;
    } 
    std::string PlainEngine::operator[] (const std::string& key) {
        return _map[key];
    } 
}