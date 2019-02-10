#include <postgres/Command.h>
#include <cstring>

namespace postgres {

Command::Command(Command&& other) noexcept = default;

Command& Command::operator=(Command&& other) noexcept = default;

Command::~Command() = default;

char const* Command::statement() const {
    return stmt_.c_str();
}

int Command::count() const {
    return static_cast<int>(values_.size());
}

Oid const* Command::types() const {
    return types_.data();
}

char const* const* Command::values() const {
    return values_.data();
}

int const* Command::lengths() const {
    return lengths_.data();
}

int const* Command::formats() const {
    return formats_.data();
}

void Command::add(std::nullptr_t) {
    setMeta(0, 0, 0);
    values_.push_back(nullptr);
}

void Command::add(std::chrono::system_clock::time_point const arg) {
    add(makeTimestamp(arg));
}

void Command::add(Timestamp const& arg) {
    if (arg.hasTimezone()) {
        add(arg.format());
        types_.back() = TIMESTAMPTZOID;
        return;
    }

    add(arg.postgresTime());
    types_.back() = TIMESTAMPOID;
}

void Command::add(std::string const& arg) {
    add(arg.c_str());
}

void Command::add(std::string&& arg) {
    addText(arg.c_str(), arg.size() + 1);
}

void Command::add(char const* const arg) {
    setMeta(0, 0, 0);
    values_.push_back(arg);
}

void Command::addText(char const* const arg, size_t const len) {
    setMeta(0, static_cast<int>(len), 0);
    storeData(arg, len);
}

void Command::setMeta(Oid const id, int const len, int const fmt) {
    types_.push_back(id);
    lengths_.push_back(len);
    formats_.push_back(fmt);
}

void Command::storeData(void const* const arg, size_t const len) {
    auto       storage = buffer_.data();
    auto const old_len = buffer_.size();
    auto const new_len = old_len + len;
    buffer_.resize(new_len);
    if (buffer_.data() != storage) {
        storage = buffer_.data();
        for (auto i = 0u; i < values_.size(); ++i) {
            if (values_[i] && lengths_[i]) {
                values_[i] = storage;
                storage += lengths_[i];
            }
        }
    } else {
        storage += old_len;
    }
    memcpy(storage, arg, len);
    values_.push_back(storage);
}

}  // namespace postgres
