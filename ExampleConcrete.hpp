#pragma once

#include "JsonResource.hpp"

#include <filesystem>
#include <string>
#include <vector>

namespace rl {

class ExampleConcrete final : public JsonResource {
public:
    ExampleConcrete() = default;

    void loadFromJson(const ResourceKey& key,
                      const MiniJson& json,
                      const std::filesystem::path& sourceFile) override;

    [[nodiscard]] const ResourceKey& key() const { return key_; }
    [[nodiscard]] const std::string& displayName() const { return displayName_; }
    [[nodiscard]] int floors() const { return floors_; }
    [[nodiscard]] const std::vector<std::string>& tags() const { return tags_; }
    [[nodiscard]] const std::filesystem::path& sourceFile() const { return sourceFile_; }

    [[nodiscard]] std::string debugString() const;

private:
    ResourceKey key_;
    std::string displayName_;
    int floors_ = 0;
    std::vector<std::string> tags_;
    std::filesystem::path sourceFile_;
};

} // namespace rl
