#ifndef  RHINO_SRC_DB_ENGINE_H_
#define  RHINO_SRC_DB_ENGINE_H_

#include<string.h>

namespace RHINO {

template <class T>
class Rhino
{
private:
    typedef T DbEngine;
    DbEngine *_engine;
public:
    Rhino() {
        _engine = new(std::nothrow) DbEngine();

        if (!_engine) {
            std::__throw_runtime_error("creation of DbEngine failed");
        }
    }
    ~Rhino() = default;
    
    bool init() {
        return _engine->init();
    }
    
    int insert(const std::string& key, const std::string& value) {
        _engine->insert(std::make_pair(key, value));
        return 0;
    }
    
    int get(const std::string& key, std::string* value) {
        if (!_engine->has_key(key)) {
            fprintf(stderr, "get %s failed.\n", key.c_str());
            return -1;
        }
        *value = (*_engine)[key];
        return 0;
    }
};
}

#endif