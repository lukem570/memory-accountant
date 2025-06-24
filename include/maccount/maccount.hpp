#ifndef MACCOUNT_EXPORT

#include <string>
#include <cstdio>
#include <unordered_map>
#include <unordered_set>

#ifdef _WIN32
    #ifdef BUILD_MACCOUNT
        #define MACCOUNT_EXPORT __declspec(dllexport)
    #else
        #define MACCOUNT_EXPORT __declspec(dllimport)
    #endif
#elif defined(__GNUC__) || defined(__clang__)
    #ifdef BUILD_MACCOUNT
        #define MACCOUNT_EXPORT __attribute__((visibility("default")))
    #else
        #define MACCOUNT_EXPORT
    #endif
#else
    #error "Unknown platform, please define export/import macros."
#endif

namespace ma {

    struct TrackedMemoryRecord {
        void* ptr;
        std::string alias;
    };

    class MACCOUNT_EXPORT Instance {
        public:
            Instance() { instance = this; }
            ~Instance() {
                if (instance == nullptr) {
                    return;
                }

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

            void disable() {
                instance = nullptr;
            }

        private:
            static Instance* instance;

            std::unordered_map<void*, TrackedMemoryRecord> memoryRecords;
            std::unordered_set<void*> freedMemory;
    };

    void MACCOUNT_EXPORT track(void* ptr, const std::string& alias = "") {
        Instance::__track(ptr, alias);
    }

    void MACCOUNT_EXPORT untrack(void* ptr) {
        Instance::__untrack(ptr);
    }
};

#endif