#include <postgres/Command.h>

namespace postgres {

Command::Command(Command&& other) = default;

Command& Command::operator=(Command&& other) = default;

Command::~Command() = default;

char const* Command::statement() const {
    return stmt_.c_str();
}

int Command::nParams() const {
    return values_.size();
}

Oid const* Command::paramTypes() const {
    return types_.empty() ? nullptr : types_.data();
}

char const* const* Command::paramValues() const {
    return values_.empty() ? nullptr : values_.data();
}

int const* Command::paramLengths() const {
    return lenghts_.empty() ? nullptr : lenghts_.data();
}

int const* Command::paramFormats() const {
    return formats_.empty() ? nullptr : formats_.data();
}

int Command::resultFormat() const {
    return 1;
}

void Command::add(std::nullptr_t) {
    setMeta(0, 0, 0);
    values_.push_back(nullptr);
}

void Command::add(std::chrono::system_clock::time_point const param) {
    add(makeTimestamp(param));
}

void Command::add(Timestamp const& param) {
    if (param.hasTimezone()) {
        auto const formatted = param.format();
        addText(formatted.c_str(), formatted.size() + 1);
        types_.back() = TIMESTAMPTZOID;
    } else {
        add(param.postgresTime());
        types_.back() = TIMESTAMPOID;
    }
}

void Command::add(std::string const& param) {
    add(param.c_str());
}

void Command::add(std::string&& param) {
    addText(param.c_str(), param.size() + 1);
}

void Command::add(char const* const param) {
    setMeta(0, 0, 0);
    values_.push_back(param);
}

void Command::addText(char const* const param, int const size) {
    setMeta(0, size, 0);
    storeData(param, size);
}

void Command::add(bool const param) {
    addBinary(param, BOOLOID);
}

void Command::setMeta(Oid const id, int const len, int const fmt) {
    types_.push_back(id);
    lenghts_.push_back(len);
    formats_.push_back(fmt);
}

void Command::storeData(void const* const arg, int const len) {
    auto       storage = buffer_.data();
    auto const old_len = buffer_.size();
    auto const new_len = old_len + len;
    if (buffer_.capacity() < new_len) {
        buffer_.reserve(buffer_.empty() ? 256 : buffer_.capacity() * 2);
    }
    buffer_.resize(new_len);
    if (buffer_.data() != storage) {
        storage = buffer_.data();
        for (auto i = 0u; i < values_.size(); ++i) {
            if (values_[i] && lenghts_[i]) {
                values_[i] = storage;
                storage += lenghts_[i];
            }
        }
    } else {
        storage += old_len;
    }
    memcpy(storage, arg, len);
    values_.push_back(storage);
}

}  // namespace postgres
