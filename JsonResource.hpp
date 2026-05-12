#pragma once

#include "MiniJson.hpp"
#include "ResourceTypes.hpp"

#include <filesystem>
#include <memory>
#include <stdexcept>
#include <type_traits>

namespace rl {

class JsonResource {
public:
    virtual ~JsonResource() = default;

    virtual void loadFromJson(const ResourceKey& key,
                              const MiniJson& json,
                              const std::filesystem::path& sourceFile) = 0;
};

MiniJson readJsonFile(const std::filesystem::path& file);

template <typename T>
std::shared_ptr<T> loadJsonResource(const ResourceKey& key,
                                    const std::filesystem::path& file) {
    static_assert(std::is_base_of_v<JsonResource, T>,
                  "T must derive from rl::JsonResource");

    auto json = readJsonFile(file);
    auto obj = std::make_shared<T>();
    obj->loadFromJson(key, json, file);
    return obj;
}

} // namespace rl
