#ifndef  RHINO_SRC_PLAIN_KV_ENGINE_H_
#define  RHINO_SRC_PLAIN_KV_ENGINE_H_
#include <stdio.h>
#include <string>
#include <unordered_map>
#include "storage/engine/base_kv_engine.h" 
namespace RHINO {
class PlainEngine : public BaseKVEngine {
    public:
        bool init();
        
        int insert(const std::string& key, const std::string& value); 

        int insert(const std::pair<std::string, std::string>& kv); 

        bool has_key(const std::string& key);

        std::string operator[] (const std::string& key); 
    private:
        std::unordered_map<std::string, std::string> _map;

};
}  // namespace RHINO
#endif
