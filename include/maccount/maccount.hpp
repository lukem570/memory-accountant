#include <string>
#include <cstdio>
#include <unordered_map>
#include <unordered_set>

namespace ma {

    struct TrackedMemoryRecord {
        void* ptr;
        std::string alias;
    };

    class Instance {
        public:
            Instance() { instance = this; }
            ~Instance() {
                for (auto& [ptr, record] : instance->memoryRecords) {
                    printf("Memory not cleaned up: [%x] %s\n", (size_t)ptr, record.alias.c_str());
                }
            }

            static void __track(void* ptr, const std::string& alias = "") {

                if (instance == nullptr) {
                    return;
                }
        
                if (instance->memoryRecords.find(ptr) != instance->memoryRecords.end()) {
                    printf("Record: '%s' already exists\n", alias.c_str());
                    return;
                }

                instance->memoryRecords[ptr] = TrackedMemoryRecord{.ptr = ptr, .alias = alias};
            }

            static void __untrack(void* ptr) {

                if (instance == nullptr) {
                    return;
                }

                if (instance->freedMemory.find(ptr) != instance->freedMemory.end()) {
                    printf("Attempted double free at: '%x'\n", (size_t)ptr);
                    return;
                }

                if (instance->memoryRecords.find(ptr) == instance->memoryRecords.end()) {
                    printf("Attempted to delete untracked pointer at: '%x'\n", (size_t)ptr);
                    return;
                }

                instance->memoryRecords.erase(ptr);
            }

        private:
            static Instance* instance;

            std::unordered_map<void*, TrackedMemoryRecord> memoryRecords;
            std::unordered_set<void*> freedMemory;
    };

    void track(void* ptr, const std::string& alias = "") {
        Instance::__track(ptr, alias);
    }

    void untrack(void* ptr) {
        Instance::__untrack(ptr);
    }
};