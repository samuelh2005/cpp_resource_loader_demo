#include "ResourceLoader.hpp"

#include <algorithm>
#include <iostream>
#include <optional>

namespace fs = std::filesystem;

namespace rl {

namespace {

fs::path stripOneExtension(fs::path p) {
    p.replace_extension();
    return p;
}

std::string joinPathComponents(const fs::path& p) {
    std::string out;
    for (const auto& part : p) {
        const std::string s = part.string();
        if (s.empty() || s == ".") {
            continue;
        }
        if (!out.empty()) {
            out.push_back('/');
        }
        out += s;
    }
    return out;
}

} // namespace

ResourceLoader::ResourceLoader(ResourceManager& manager)
    : manager_(manager) {}

bool ResourceLoader::isDatapackRoot(const fs::path& packRoot) {
    return fs::exists(packRoot / "data") && fs::is_directory(packRoot / "data");
}

std::optional<ParsedDatapackPath> ResourceLoader::parseFile(const fs::path& packRoot,
                                                            const fs::path& file) {
    const fs::path dataRoot = packRoot / "data";
    if (!fs::exists(dataRoot) || !fs::is_directory(dataRoot)) {
        return std::nullopt;
    }

    std::error_code ec;
    fs::path rel = fs::relative(file, dataRoot, ec);
    if (ec || rel.empty()) {
        return std::nullopt;
    }

    std::vector<fs::path> parts;
    for (const auto& part : rel) {
        const std::string s = part.string();
        if (!s.empty() && s != ".") {
            parts.emplace_back(s);
        }
    }

    if (parts.size() < 3) {
        return std::nullopt;
    }

    ParsedDatapackPath out;
    out.key.namespace_ = parts[0].string();
    out.registry = parts[1].string();

    fs::path resourcePath;
    for (std::size_t i = 2; i < parts.size(); ++i) {
        resourcePath /= parts[i];
    }
    out.key.path = stripOneExtension(resourcePath).generic_string();
    out.file = file;

    if (out.key.namespace_.empty() || out.registry.empty() || out.key.path.empty()) {
        return std::nullopt;
    }

    return out;
}

std::vector<ParsedDatapackPath> ResourceLoader::scanDataDirectory(const fs::path& dataRoot) {
    std::vector<ParsedDatapackPath> discovered;
    if (!fs::exists(dataRoot) || !fs::is_directory(dataRoot)) {
        return discovered;
    }

    const fs::path packRoot = dataRoot.parent_path();
    for (const auto& entry : fs::recursive_directory_iterator(dataRoot)) {
        if (!entry.is_regular_file()) {
            continue;
        }
        auto parsed = parseFile(packRoot, entry.path());
        if (parsed) {
            discovered.push_back(std::move(*parsed));
        }
    }
    return discovered;
}

std::vector<ParsedDatapackPath> ResourceLoader::scan(const fs::path& packRoot) {
    return scanDataDirectory(packRoot / "data");
}

void ResourceLoader::loadPack(const fs::path& packRoot) {
    const fs::path dataRoot = packRoot / "data";
    for (const auto& discovered : scanDataDirectory(dataRoot)) {
        const bool dispatched = manager_.dispatch(discovered.registry, discovered.key, discovered.file);
        if (!dispatched) {
            std::cerr << "[ResourceLoader] No registry named '" << discovered.registry
                      << "' for file " << discovered.file << '\n';
        }
    }
}

} // namespace rl
