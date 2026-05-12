#pragma once

#include "ResourceTypes.hpp"

#include <filesystem>
#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace rl {

class IRegistry {
public:
    virtual ~IRegistry() = default;

    virtual const std::string& name() const = 0;
    virtual void load(const ResourceKey& key, const std::filesystem::path& file) = 0;
    virtual bool contains(const ResourceKey& key) const = 0;
};

template <typename T>
class Registry final : public IRegistry {
public:
    using LoaderFn = std::function<std::shared_ptr<T>(const ResourceKey&, const std::filesystem::path&)>;

    Registry(std::string name, LoaderFn loader)
        : name_(std::move(name)), loader_(std::move(loader)) {
        if (name_.empty()) {
            throw std::invalid_argument("Registry name must not be empty");
        }
        if (!loader_) {
            throw std::invalid_argument("Registry loader must not be empty");
        }
    }

    const std::string& name() const override { return name_; }

    void load(const ResourceKey& key, const std::filesystem::path& file) override {
        auto loaded = loader_(key, file);
        if (!loaded) {
            throw std::runtime_error("Registry loader returned null for " + key.toString());
        }
        entries_[key.toString()] = std::move(loaded);
    }

    bool contains(const ResourceKey& key) const override {
        return entries_.find(key.toString()) != entries_.end();
    }

    T* get(const ResourceKey& key) {
        auto it = entries_.find(key.toString());
        return it == entries_.end() ? nullptr : it->second.get();
    }

    const T* get(const ResourceKey& key) const {
        auto it = entries_.find(key.toString());
        return it == entries_.end() ? nullptr : it->second.get();
    }

    std::vector<ResourceKey> keys() const {
        std::vector<ResourceKey> out;
        out.reserve(entries_.size());
        for (const auto& [k, _] : entries_) {
            const auto pos = k.find(':');
            if (pos == std::string::npos) {
                continue;
            }
            out.emplace_back(k.substr(0, pos), k.substr(pos + 1));
        }
        return out;
    }

private:
    std::string name_;
    LoaderFn loader_;
    std::unordered_map<std::string, std::shared_ptr<T>> entries_;
};

} // namespace rl
