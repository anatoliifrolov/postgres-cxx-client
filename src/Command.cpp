#include <postgres/Command.h>

namespace postgres {

Command::Command(Command&& other) = default;

Command& Command::operator=(Command&& other) = default;

Command::~Command() = default;

const char* Command::statement() const {
    return statement_.c_str();
}

int Command::nParams() const {
    return values_.size();
}

const Oid* Command::paramTypes() const {
    return types_.empty() ? nullptr : types_.data();
}

const char* const* Command::paramValues() const {
    return values_.empty() ? nullptr : values_.data();
}

const int* Command::paramLengths() const {
    return lenghts_.empty() ? nullptr : lenghts_.data();
}

const int* Command::paramFormats() const {
    return formats_.empty() ? nullptr : formats_.data();
}

int Command::resultFormat() const {
    return 1;
}

void Command::add(std::nullptr_t) {
    setMeta(0, 0, 0);
    values_.push_back(nullptr);
}

void Command::add(const std::chrono::system_clock::time_point param) {
    add(makeTimestamp(param));
}

void Command::add(const Timestamp& param) {
    if (param.hasTimezone()) {
        const auto formatted = param.format();
        addText(formatted.c_str(), formatted.size() + 1);
        types_.back() = TIMESTAMPTZOID;
    } else {
        add(param.postgresTime());
        types_.back() = TIMESTAMPOID;
    }
}

void Command::add(const std::string& param) {
    add(param.c_str());
}

void Command::add(std::string&& param) {
    addText(param.c_str(), param.size() + 1);
}

void Command::add(const char* const param) {
    setMeta(0, 0, 0);
    values_.push_back(param);
}

void Command::addText(const char* const param, const int size) {
    setMeta(0, size, 0);
    storeData(param, size);
}

void Command::add(const bool param) {
    addBinary(param, BOOLOID);
}

void Command::setMeta(const Oid type, const int size, const int format) {
    types_.push_back(type);
    lenghts_.push_back(size);
    formats_.push_back(format);
}

void Command::storeData(const void* const param, const int size) {
    auto storage = buffer_.data();
    const auto old_size = buffer_.size();
    const auto new_size = old_size + size;
    if (buffer_.capacity() < new_size) {
        buffer_.reserve(buffer_.empty() ? 256 : buffer_.capacity() * 2);
    }
    buffer_.resize(new_size);
    if (buffer_.data() != storage) {
        storage = buffer_.data();
        for (auto i = 0u; i < values_.size(); ++i) {
            if (values_[i] && lenghts_[i]) {
                values_[i] = storage;
                storage += lenghts_[i];
            }
        }
    } else {
        storage += old_size;
    }
    memcpy(storage, param, size);
    values_.push_back(storage);
}

}  // namespace postgres
