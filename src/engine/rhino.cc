#include "engine/rhino.h"
#include "engine/types/leveldb_engine.h"

namespace RHINO {
    template<class T>
    Rhino<T>::Rhino()
    {
        _engine = new(std::nothrow) DbEngine();

        if (!_engine) {
            std::__throw_runtime_error("creation of DbEngine failed");
        }
    }
    template<class T>
    bool Rhino<T>::init() {
        return _engine->init();
    }
    
    template<class T>
    void Rhino<T>::lock(Key key) {
        mutexs_[key]->Lock();
    }
    
    template<class T>
    void Rhino<T>::unLock(Key key) {
        mutexs_[key]->Unlock();
    }

    template<class T>
    int Rhino<T>::insert(const Key& key, const Value& value, int txn_unique_id) {

        Version* new_update = new Version();
        new_update->value_ = value;
        new_update->version_id_ = txn_unique_id;
        new_update->max_read_id_ = 0;

        deque<Version*>* versions;

        if (mvcc_data_.count(key) > 0) {
            versions = mvcc_data_[key];
        } else {
            versions = new deque<Version*>();
            mvcc_data_[key] = versions;
        }

        versions->push_front(new_update);

        _engine->insert(std::make_pair(key, value));
        return 0;
    }
    
    template<class T>
    int Rhino<T>::get(const Key& key, Value* value, int txn_unique_id) {

        if (!mvcc_data_.count(key)) {
            return false;
        }

        deque<Version*> *data = mvcc_data_[key];

        if (data->empty()) {
            return false;
        } else {
            int maxlessthan = 0;
            deque<Version*> version_list = *data;
            for (deque<Version*>::iterator itr = version_list.begin(); itr != version_list.end(); itr++){
                Version* version = *itr;
                //get largest less than version
                if (version->version_id_ <= txn_unique_id && version->version_id_ > maxlessthan){
                    //set new version
                    *value = version->value_;
                    version->max_read_id_ = txn_unique_id;
                }  
            }
        }
        return true;
    }

    template<class T>
    bool Rhino<T>::checkWrite(Key key, int txn_unique_id) {
        // CPSC 638:
        //
        // Implement this method!
            
        // Hint: Before all writes are applied, we need to make sure that each write
        // can be safely applied based on MVCC timestamp ordering protocol. This method
        // only checks one key, so you should call this method for each key in the
        // write_set. Return true if this key passes the check, return false if not. 
        // Note that you don't have to call Lock(key) in this method, just
        // call Lock(key) before you call this method and call Unlock(key) afterward.
        
        //key not found
        if (!mvcc_data_.count(key)){
            return false;
        }

        deque<Version*> *data = mvcc_data_[key];
        
        if (data->empty()) {
            //no data
            return true;
        }
        else {
            deque<Version*> version_list = *data;
            //get latest version
            Version* version = version_list.back();

            //check if version valid
            if (version->max_read_id_ > txn_unique_id){
            return false;
            }
        }
        return true;
    }
template class Rhino<RHINO::LevelDBEngine>;
}
