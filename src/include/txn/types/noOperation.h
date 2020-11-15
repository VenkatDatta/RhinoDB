#ifndef __NO_OPERATION_TYPES_H__
#define __NO_OPERATION_TYPES_H__

#include <map>
#include <set>
#include <string>

#include <txn/transaction.h>

// Immediately Commits

class NoOperation : public Transaction {
    public:
    NoOperation() {}

    virtual void Run() { COMMIT; }
};

#endif