#pragma once

#include "Registry.hpp"

#include <filesystem>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <stdexcept>

namespace rl {

class ResourceManager {
public:
    ResourceManager() = default;
    ~ResourceManager() = default;

    template <typename T>
    void addRegistry(std::unique_ptr<Registry<T>> registry) {
        if (!registry) {
            throw std::invalid_argument("Cannot add a null registry");
        }
        const std::string key = registry->name();
        registries_[key] = std::move(registry);
    }

    bool hasRegistry(std::string_view name) const;

    bool dispatch(std::string_view registryName,
                  const ResourceKey& key,
                  const std::filesystem::path& file);

    template <typename T>
    Registry<T>* getRegistry(std::string_view name) {
        auto it = registries_.find(std::string(name));
        if (it == registries_.end()) {
            return nullptr;
        }
        return dynamic_cast<Registry<T>*>(it->second.get());
    }

    template <typename T>
    const Registry<T>* getRegistry(std::string_view name) const {
        auto it = registries_.find(std::string(name));
        if (it == registries_.end()) {
            return nullptr;
        }
        return dynamic_cast<const Registry<T>*>(it->second.get());
    }

private:
    std::unordered_map<std::string, std::unique_ptr<IRegistry>> registries_;
};

} // namespace rl
