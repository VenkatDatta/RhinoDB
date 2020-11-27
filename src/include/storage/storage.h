#ifndef  RHINO_SRC_DB_ENGINE_H_
#define  RHINO_SRC_DB_ENGINE_H_

#include<string>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <tr1/unordered_map>
#include <deque>
#include <map>

#include "utils/common.h"
#include "txn/transaction.h"
#include "utils/mutex.h"
#include "storage.pb.h"
#include "pbjson/pbjson.hpp"


namespace RHINO {

struct Version {
  Value value_;      // The value of this version
  int max_read_id_;  // Largest timestamp of a transaction that read the version
  int version_id_;   // Timestamp of the transaction that created(wrote) the version
};
using std::tr1::unordered_map;
using std::deque;
using std::map;

template <class T>
class Storage
{
private:
    typedef T DbEngine;
    DbEngine *_engine;
    unordered_map<Key, deque<Version*>*> mvcc_data_;

    unordered_map<Key, Mutex*> mutexs_;
public:
    explicit Storage();

    virtual ~Storage() = default;
    
    virtual bool init();

    virtual int rawPut(const std::string& key, const std::string& value);

    
    virtual int rawGet(const std::string& key, std::string* value);
    
    virtual int insert(const Key& key, const Value& value, int txn_unique_id = 0);
    
    virtual int get(const Key& key, Value* value, int txn_unique_id = 0);

    virtual void lock(Key key);

    virtual void unLock(Key key);

    virtual double timestamp(Key key) {return 0;}

    virtual bool checkWrite(Key key, int txn_unique_id);
};
}

#endif