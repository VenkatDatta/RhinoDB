#ifndef __PUT_OPERATION_TYPES_H__
#define __PUT_OPERATION_TYPES_H__

#include <map>
#include <set>
#include <string>

#include <txn/transaction.h>

// Immediately Commits

class PutOperation : public Transaction {
    public:
    PutOperation(const map<Key, Value>& m) : m_(m) {
        for (map<Key, Value>::iterator it = m_.begin(); it != m_.end(); ++it)
            writeset_.insert(it->first);
    }

    PutOperation* clone() const {
        PutOperation* clone = new PutOperation(map<Key, Value>(m_));
        this->copyTransactionInternals(clone);
        return clone;
    }

    virtual void Run() {
        for (map<Key, Value>::iterator it = m_.begin(); it != m_.end(); ++it)
            write(it->first, it->second);
        COMMIT;
    }

    private:
    map<Key, Value> m_;
};

#endif