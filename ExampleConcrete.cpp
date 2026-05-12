#include "ExampleConcrete.hpp"

#include <sstream>
#include <stdexcept>

namespace rl {

namespace {

template <typename T>
T requireField(const MiniJson& json, const char* field);

template <>
std::string requireField<std::string>(const MiniJson& json, const char* field) {
    if (!json.contains(field)) {
        throw std::runtime_error(std::string("Missing required field '") + field + "'");
    }
    return json.at(field).asString();
}

} // namespace

void ExampleConcrete::loadFromJson(const ResourceKey& key,
                                   const MiniJson& json,
                                   const std::filesystem::path& sourceFile) {
    key_ = key;
    sourceFile_ = sourceFile;

    displayName_ = requireField<std::string>(json, "display_name");
    floors_ = json.contains("floors") ? json.at("floors").asInt() : 0;
    tags_.clear();
    if (json.contains("tags")) {
        const auto& arr = json.at("tags").asArray();
        tags_.reserve(arr.size());
        for (const auto& item : arr) {
            tags_.push_back(item.asString());
        }
    }
}

std::string ExampleConcrete::debugString() const {
    std::ostringstream oss;
    oss << key_.toString() << " | name='" << displayName_ << "' | floors=" << floors_ << " | tags=[";
    for (std::size_t i = 0; i < tags_.size(); ++i) {
        if (i != 0) {
            oss << ", ";
        }
        oss << tags_[i];
    }
    oss << "] | file=" << sourceFile_.string();
    return oss.str();
}

} // namespace rl
