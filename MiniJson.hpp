#pragma once

#include <cctype>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <limits>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

namespace rl {

class MiniJson {
public:
    enum class Type { Null, Bool, Number, String, Object, Array };

    using Object = std::unordered_map<std::string, MiniJson>;
    using Array = std::vector<MiniJson>;

    MiniJson() : type_(Type::Null), value_(std::monostate{}) {}
    explicit MiniJson(std::nullptr_t) : MiniJson() {}
    explicit MiniJson(bool v) : type_(Type::Bool), value_(v) {}
    explicit MiniJson(double v) : type_(Type::Number), value_(v) {}
    explicit MiniJson(std::string v) : type_(Type::String), value_(std::move(v)) {}
    explicit MiniJson(Object v) : type_(Type::Object), value_(std::make_shared<Object>(std::move(v))) {}
    explicit MiniJson(Array v) : type_(Type::Array), value_(std::make_shared<Array>(std::move(v))) {}

    [[nodiscard]] Type type() const { return type_; }
    [[nodiscard]] bool isNull() const { return type_ == Type::Null; }
    [[nodiscard]] bool isBool() const { return type_ == Type::Bool; }
    [[nodiscard]] bool isNumber() const { return type_ == Type::Number; }
    [[nodiscard]] bool isString() const { return type_ == Type::String; }
    [[nodiscard]] bool isObject() const { return type_ == Type::Object; }
    [[nodiscard]] bool isArray() const { return type_ == Type::Array; }

    [[nodiscard]] bool asBool() const {
        if (!isBool()) throw std::runtime_error("MiniJson: not a bool");
        return std::get<bool>(value_);
    }

    [[nodiscard]] double asNumber() const {
        if (!isNumber()) throw std::runtime_error("MiniJson: not a number");
        return std::get<double>(value_);
    }

    [[nodiscard]] int asInt() const {
        const double d = asNumber();
        if (d < static_cast<double>(std::numeric_limits<int>::min()) ||
            d > static_cast<double>(std::numeric_limits<int>::max())) {
            throw std::runtime_error("MiniJson: number out of int range");
        }
        return static_cast<int>(d);
    }

    [[nodiscard]] const std::string& asString() const {
        if (!isString()) throw std::runtime_error("MiniJson: not a string");
        return std::get<std::string>(value_);
    }

    [[nodiscard]] const Object& asObject() const {
        if (!isObject()) throw std::runtime_error("MiniJson: not an object");
        return *std::get<std::shared_ptr<Object>>(value_);
    }

    [[nodiscard]] const Array& asArray() const {
        if (!isArray()) throw std::runtime_error("MiniJson: not an array");
        return *std::get<std::shared_ptr<Array>>(value_);
    }

    [[nodiscard]] bool contains(std::string_view key) const {
        if (!isObject()) return false;
        const auto& obj = asObject();
        return obj.find(std::string(key)) != obj.end();
    }

    [[nodiscard]] const MiniJson& at(std::string_view key) const {
        const auto& obj = asObject();
        auto it = obj.find(std::string(key));
        if (it == obj.end()) throw std::runtime_error("MiniJson: missing key '" + std::string(key) + "'");
        return it->second;
    }

    [[nodiscard]] std::string valueString(std::string_view key, std::string defaultValue = {}) const {
        if (!contains(key)) return defaultValue;
        return at(key).asString();
    }

    [[nodiscard]] int valueInt(std::string_view key, int defaultValue = 0) const {
        if (!contains(key)) return defaultValue;
        return at(key).asInt();
    }

    [[nodiscard]] std::vector<std::string> valueStringArray(std::string_view key,
                                                            std::vector<std::string> defaultValue = {}) const {
        if (!contains(key)) return defaultValue;
        const auto& arr = at(key).asArray();
        std::vector<std::string> out;
        out.reserve(arr.size());
        for (const auto& item : arr) {
            out.push_back(item.asString());
        }
        return out;
    }

    static MiniJson parse(std::string_view text) {
        Parser p(text);
        MiniJson v = p.parseValue();
        p.skipWs();
        if (!p.eof()) {
            throw std::runtime_error("MiniJson: trailing characters after JSON value");
        }
        return v;
    }

private:
    using Storage = std::variant<std::monostate, bool, double, std::string,
                                 std::shared_ptr<Object>, std::shared_ptr<Array>>;

    class Parser {
    public:
        explicit Parser(std::string_view text) : text_(text) {}

        MiniJson parseValue() {
            skipWs();
            if (eof()) throw std::runtime_error("MiniJson: unexpected end of input");
            char c = peek();
            switch (c) {
                case 'n': return parseNull();
                case 't': return parseTrue();
                case 'f': return parseFalse();
                case '"': return MiniJson(parseString());
                case '{': return parseObject();
                case '[': return parseArray();
                default:
                    if (c == '-' || std::isdigit(static_cast<unsigned char>(c))) {
                        return MiniJson(parseNumber());
                    }
                    throw std::runtime_error(std::string("MiniJson: unexpected character '") + c + "'");
            }
        }

        void skipWs() {
            while (!eof() && std::isspace(static_cast<unsigned char>(peek()))) {
                ++pos_;
            }
        }

        bool eof() const { return pos_ >= text_.size(); }

    private:
        MiniJson parseNull() {
            expectLiteral("null");
            return MiniJson();
        }

        MiniJson parseTrue() {
            expectLiteral("true");
            return MiniJson(true);
        }

        MiniJson parseFalse() {
            expectLiteral("false");
            return MiniJson(false);
        }

        MiniJson parseObject() {
            expect('{');
            Object obj;
            skipWs();
            if (match('}')) {
                return MiniJson(std::move(obj));
            }
            while (true) {
                skipWs();
                const std::string key = parseString();
                skipWs();
                expect(':');
                skipWs();
                obj.emplace(key, parseValue());
                skipWs();
                if (match('}')) break;
                expect(',');
            }
            return MiniJson(std::move(obj));
        }

        MiniJson parseArray() {
            expect('[');
            Array arr;
            skipWs();
            if (match(']')) {
                return MiniJson(std::move(arr));
            }
            while (true) {
                skipWs();
                arr.push_back(parseValue());
                skipWs();
                if (match(']')) break;
                expect(',');
            }
            return MiniJson(std::move(arr));
        }

        std::string parseString() {
            expect('"');
            std::string out;
            while (!eof()) {
                char c = get();
                if (c == '"') {
                    return out;
                }
                if (c == '\\') {
                    if (eof()) throw std::runtime_error("MiniJson: invalid escape");
                    char esc = get();
                    switch (esc) {
                        case '"': out.push_back('"'); break;
                        case '\\': out.push_back('\\'); break;
                        case '/': out.push_back('/'); break;
                        case 'b': out.push_back('\b'); break;
                        case 'f': out.push_back('\f'); break;
                        case 'n': out.push_back('\n'); break;
                        case 'r': out.push_back('\r'); break;
                        case 't': out.push_back('\t'); break;
                        default:
                            throw std::runtime_error("MiniJson: unsupported escape sequence");
                    }
                } else {
                    out.push_back(c);
                }
            }
            throw std::runtime_error("MiniJson: unterminated string");
        }

        double parseNumber() {
            const char* begin = text_.data() + pos_;
            char* end = nullptr;
            double value = std::strtod(begin, &end);
            if (end == begin) {
                throw std::runtime_error("MiniJson: invalid number");
            }
            pos_ = static_cast<std::size_t>(end - text_.data());
            return value;
        }

        void expectLiteral(const char* lit) {
            while (*lit) {
                if (eof() || get() != *lit++) {
                    throw std::runtime_error("MiniJson: invalid literal");
                }
            }
        }

        bool match(char c) {
            if (!eof() && peek() == c) {
                ++pos_;
                return true;
            }
            return false;
        }

        void expect(char c) {
            if (eof() || get() != c) {
                throw std::runtime_error(std::string("MiniJson: expected '") + c + "'");
            }
        }

        char get() {
            if (eof()) throw std::runtime_error("MiniJson: unexpected end of input");
            return text_[pos_++];
        }

        char peek() const {
            if (eof()) throw std::runtime_error("MiniJson: unexpected end of input");
            return text_[pos_];
        }

        std::string_view text_;
        std::size_t pos_ = 0;
    };

    Type type_;
    Storage value_;
};

inline MiniJson readMiniJsonFile(const std::filesystem::path& file) {
    std::ifstream in(file);
    if (!in) {
        throw std::runtime_error("Failed to open JSON file: " + file.string());
    }
    std::string text((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    return MiniJson::parse(text);
}

} // namespace rl
