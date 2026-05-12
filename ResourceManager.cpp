#include "ResourceManager.hpp"

namespace rl {

bool ResourceManager::hasRegistry(std::string_view name) const {
    return registries_.find(std::string(name)) != registries_.end();
}

bool ResourceManager::dispatch(std::string_view registryName,
                               const ResourceKey& key,
                               const std::filesystem::path& file) {
    auto it = registries_.find(std::string(registryName));
    if (it == registries_.end()) {
        return false;
    }
    it->second->load(key, file);
    return true;
}

} // namespace rl
