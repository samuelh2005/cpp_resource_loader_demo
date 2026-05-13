#include <iostream>

#include "Registry.hpp"
#include "ResourceKey.hpp"

struct Block {
    std::string name;
    int hardness;
};

int main() {
    Registry<Block> blocks;

    ResourceKey stone{"minecraft", "stone"};
    ResourceKey dirt{"minecraft", "dirt"};

    blocks.registerItem(stone, {"Stone", 3});
    blocks.registerItem(dirt, {"Dirt", 1});

    if (auto* b = blocks.get(stone)) {
        std::cout << b->name << "\n";
    }
}