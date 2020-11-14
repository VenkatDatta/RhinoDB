#include "txn/transaction.h"

bool Transaction::read(const Key& key, Value * value) {
    if (readset_.count(key) == 0 && writeset_.count(key) == 0)
        DIE("INVALID READ: Key not present in Read/Write set")

    if (status_ != INCOMPLETE)
        return false;

    if (reads_.count(key)) {
        *value = reads_[key];
        return true;
    } else {
        return false;
    }
}

void Transaction::write(const Key& key, const Value& value) {

    if (writeset_.count(key) == 0)
        DIE("INVALID WRITE: key - " << key);

    if (status_ != INCOMPLETE)
        return;

    writes_[key] = value;

    reads_[key] = value;
}

void Transaction::checkReadWriteSets() {

    for (set<Key>::iterator it = writeset_.begin(); it != writeset_.end(); it++) {
        if (readset_.count(*it) > 0) {
            DIE("OVERLAPPING READ/WRITE Sets \n");
        }
    }
}

void Transaction::copyTransactionInternals(Transaction* txn) const {
    txn->readset_ = set<Key>(this->readset_);
    txn->writeset_ = set<Key>(this->writeset_);
    txn->reads_ = map<Key, Value>(this->reads_);
    txn->writes_ = map<Key, Value>(this->writes_);
    txn->status_ = this->status_;
    txn->unique_id_ = this->unique_id_;
    txn->occ_start_time_ = this->occ_start_time_;
}