#include "ExampleConcrete.hpp"
#include "JsonResource.hpp"
#include "Registry.hpp"
#include "ResourceLoader.hpp"
#include "ResourceManager.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>

namespace fs = std::filesystem;

int main() {
    const fs::path packRoot = fs::current_path() / "demo_pack";

    rl::ResourceManager manager;

    auto buildings = std::make_unique<rl::Registry<rl::ExampleConcrete>>(
        "buildings",
        [](const rl::ResourceKey& key, const fs::path& file) {
            return rl::loadJsonResource<rl::ExampleConcrete>(key, file);
        }
    );

    manager.addRegistry(std::move(buildings));

    rl::ResourceLoader loader(manager);
    loader.loadPack(packRoot);

    auto* registry = manager.getRegistry<rl::ExampleConcrete>("buildings");
    if (!registry) {
        std::cerr << "Registry missing\n";
        return 1;
    }

    const rl::ResourceKey apartmentKey{"city", "apartment"};
    const rl::ResourceKey supermarketKey{"city", "supermarket"};

    const auto* apartment = registry->get(apartmentKey);
    const auto* supermarket = registry->get(supermarketKey);

    if (apartment) {
        std::cout << apartment->debugString() << '\n';
    }
    if (supermarket) {
        std::cout << supermarket->debugString() << '\n';
    }

    std::cout << "Loaded " << registry->keys().size() << " building resources.\n";
    return 0;
}
