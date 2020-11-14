#ifndef __LOCK_MANAGER_H__
#define __LOCK_MANAGER_H__

#include <tr1/unordered_map>
#include <deque>
#include <map>
#include <vector>

#include "utils/common.h"
#include "txn/transaction.h"

using std::map;
using std::deque;
using std::vector;
using std::tr1::unordered_map;

class Transaction;

enum LockMode {
    UNLOCKED = 0,
    SHARED = 1, 
    EXCLUSIVE = 2,
};

class LockManager {
    
    public:
        virtual ~LockManager();

        virtual bool ReadLock(Transaction* txn, const Key& key) = 0;

        virtual bool WriteLock(Transaction* txn, const Key& key) = 0;

        virtual void Release(Transaction* txn, const Key& key) = 0;

        virtual LockMode Status(const Key& key, vector<Transaction*>* owners) = 0;

    protected:
        struct LockRequest {
            LockRequest(LockMode m, Transaction* t) : txn_(t), mode_(m) {}
            Transaction* txn_;
            LockMode mode_;
        };

        unordered_map<Key, deque<LockRequest>*> lock_table_;

        deque<Transaction*>* ready_txns_;

        unordered_map<Transaction*, int> txn_waits_;

        deque<LockRequest>* _getLockQueue(const Key& key);
    
};

class LockManagerA : public LockManager {
 public:
  explicit LockManagerA(deque<Transaction*>* ready_txns);
  inline virtual ~LockManagerA() {}

  virtual bool ReadLock(Transaction* txn, const Key& key);
  virtual bool WriteLock(Transaction* txn, const Key& key);
  virtual void Release(Transaction* txn, const Key& key);
  virtual LockMode Status(const Key& key, vector<Transaction*>* owners);
};

// Version of the LockManager implementing both shared and exclusive locks.
class LockManagerB : public LockManager {
 public:
  explicit LockManagerB(deque<Transaction*>* ready_txns);
  inline virtual ~LockManagerB() {}

  virtual bool ReadLock(Transaction* txn, const Key& key);
  virtual bool WriteLock(Transaction* txn, const Key& key);
  virtual void Release(Transaction* txn, const Key& key);
  virtual LockMode Status(const Key& key, vector<Transaction*>* owners);
 private:
  /**
   * Returns `false` if no exclusive lock is in the lock queue for `key`.
   */

  /**
   * Maintains counts of the number of exclusive locks in lock_table_
   * for each key.
   */
  unordered_map<Key, uint64_t> _numExclusiveWaiting;

  bool _addLock(LockMode mode, Transaction* txn, const Key& key);
  bool _noExclusiveWaiting(const Key& key);
};
#endif
