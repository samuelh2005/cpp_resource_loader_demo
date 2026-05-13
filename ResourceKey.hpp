#pragma once

#include <string>
#include <stdexcept>
#include <functional>


struct ResourceKey {
    std::string ns;
    std::string path;

    ResourceKey() = default;

    ResourceKey(std::string ns_, std::string path_);

    static ResourceKey parse(const std::string& full);

    std::string toString() const;

    bool operator==(const ResourceKey& other) const;
};

namespace std {
    template <>
    struct hash<ResourceKey> {
        size_t operator()(const ResourceKey& k) const noexcept;
    };
}