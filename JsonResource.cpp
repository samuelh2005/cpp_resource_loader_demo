#include "JsonResource.hpp"

namespace rl {

MiniJson readJsonFile(const std::filesystem::path& file) {
    return readMiniJsonFile(file);
}

} // namespace rl
