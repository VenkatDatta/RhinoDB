#ifndef __TRANSACTION_H__
#define __TRANSACTION_H__

#include <map>
#include <set>
#include <vector>
#include "utils/common.h"

using std::map;
using std::set;
using std::vector;

enum TransactionStatus {
    INCOMPLETE = 0, //Not Executed
    COMPLETED_COMMIT = 1, // Executed with commit status
    COMPLETED_ABORT = 2, // Executed with abort status
    COMMITTED = 3, // Committed
    ABORTED = 4, // Aborted
};

class Transaction {
    public:
        Transaction() : status_(INCOMPLETE) {}

        virtual ~Transaction() {}

        virtual Transaction * clone() const = 0;

        virtual void Run() = 0;

        TransactionStatus getStatus() {
            return status_;
        }

        void checkReadWriteSets();

                // Set of all keys that may need to be read in order to execute the
        // transaction.
        set<Key> readset_;

        // Set of all keys that may be updated when executing the transaction.
        set<Key> writeset_;

        // Results of reads performed by the transaction.
        map<Key, Value> reads_;

        // Key, Value pairs WRITTEN by the transaction.
        map<Key, Value> writes_;

        // Transaction's current execution status.
        TransactionStatus status_;

        // Unique, monotonically increasing transaction ID, assigned by TxnProcessor.
        uint64 unique_id_;

        // Start time (used for OCC).
        double occ_start_time_; 

    protected:
        void copyTransactionInternals(Transaction * txn) const;

        friend class TranasctionProcessor;

        bool read(const Key& key, Value* value);
        
        void write(const Key& key, const Value& value);

        #define COMMIT \
            do { \
                status_ = COMPLETED_COMMIT; \
                return; \
            } while (0)

        #define ABORT \
            do { \
                status_ = COMPLETED_ABORT; \
                return; \
            } while (0)

   
};

#endif