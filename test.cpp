#include <maccount/maccount.hpp>

int main(void) {
    ma::Instance instance;

    int* memory = new int();
    ma::track(memory, "int");

    ma::untrack(memory);
    delete memory;
}