#include <cassert>
#include <iostream>
#include "dss/storage.hpp"

using namespace dss;

void test_kv_operations() {
    StorageEngine engine;
    assert(!engine.get("k1").has_value());

    engine.put("k1", "v1");
    assert(engine.get("k1").value() == "v1");
    assert(engine.key_count() == 1);

    engine.put("k1", "v2");
    assert(engine.get("k1").value() == "v2");

    assert(engine.remove("k1"));
    assert(!engine.get("k1").has_value());
    assert(engine.key_count() == 0);
}

void test_wal_and_commit() {
    StorageEngine engine;
    LogIndex idx1 = engine.append_log(1, CommandType::Put, "key1", "val1");
    LogIndex idx2 = engine.append_log(1, CommandType::Put, "key2", "val2");

    assert(idx1 == 1);
    assert(idx2 == 2);
    assert(engine.last_log_index() == 2);
    assert(engine.last_log_term() == 1);

    // Before apply, KV store is empty
    assert(!engine.get("key1").has_value());

    // Apply commit up to index 2
    engine.apply_committed(2);
    assert(engine.get("key1").value() == "val1");
    assert(engine.get("key2").value() == "val2");
    assert(engine.commit_index() == 2);
}

int main() {
    std::cout << "Running StorageEngine tests...\n";
    test_kv_operations();
    test_wal_and_commit();
    std::cout << "StorageEngine tests PASSED!\n";
    return 0;
}
