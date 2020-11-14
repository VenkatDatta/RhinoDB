#ifndef __RMW_OPERATION_TYPES_H__
#define __RMW_OPERATION_TYPES_H__

#include <map>
#include <set>
#include <string>

#include <txn/transaction.h>

// Read-Modify-Write Transaction

class ReadModifyWrite : public Transaction {
    public:
    explicit ReadModifyWrite(double time = 0) : time_(time) {}

    ReadModifyWrite(const set<Key>& readset, const set<Key>& writeset, double time = 0)
        : time_(time) {
        readset_ = readset;
        writeset_ = writeset;
    }

    ReadModifyWrite(int dbsize, int readsetsize, int writesetsize, double time = 0)
    : time_(time) {
        DCHECK(dbsize >= readsetsize + writesetsize);

        for (int i = 0; i < readsetsize; i++) {
            Key key;

            do {
                key = rand() % dbsize;

            } while (readset_.count(key));

            readset_.insert(key);
        }

        for (int i = 0; i < writesetsize; i++) {
            Key key;
            do {
                key = random() % dbsize;
            } while (readset_.count(key) || writeset_.count(key));

            writeset_.insert(key);
        }
    }

    ReadModifyWrite* clone() const {             // Virtual constructor (copying)
        ReadModifyWrite* clone = new ReadModifyWrite(time_);
        this->copyTransactionInternals(clone);
        return clone;
    }

    virtual void Run() {
        Value result;
        // Read everything in readset.
        for (set<Key>::iterator it = readset_.begin(); it != readset_.end(); ++it)
        read(*it, &result);

        // Increment length of everything in writeset.
        for (set<Key>::iterator it = writeset_.begin(); it != writeset_.end();
            ++it) {
        result = 0;
        read(*it, &result);
        write(*it, result + 1);
        }

        // Run while loop to simulate the txn logic(duration is time_).
        double begin = GetTime();
        while (GetTime() - begin < time_) {
        for (int i = 0;i < 1000; i++) {
            int x = 100;
            x = x + 2;
            x = x*x;
        }
        }
        
        COMMIT;
    }

    private:
    double time_;
};

#endif