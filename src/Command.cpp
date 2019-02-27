#include <postgres/Command.h>
#include <cstring>

namespace postgres {

Command::Command(Command&& other) noexcept = default;

Command& Command::operator=(Command&& other) noexcept = default;

Command::~Command() noexcept = default;

char const* Command::statement() const {
    return stmt_;
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

void Command::add(std::chrono::system_clock::time_point const t) {
    add(Time{t});
}

void Command::add(Time const& t) {
    if (t.hasZone()) {
        add(t.toString());
        types_.back() = TIMESTAMPTZOID;
        return;
    }

    add(t.toPostgres());
    types_.back() = TIMESTAMPOID;
}

void Command::add(std::string&& s) {
    addText(s.data(), s.size() + 1);
}

void Command::add(std::string const& s) {
    add(s.data());
}

void Command::add(std::string_view const s) {
    add(s.data());
}

void Command::add(char const* const s) {
    setMeta(0, 0, 0);
    values_.push_back(s);
}

void Command::addText(char const* const s, size_t const len) {
    setMeta(0, static_cast<int>(len), 0);
    storeData(s, len);
}

void Command::setMeta(Oid const id, int const len, int const fmt) {
    types_.push_back(id);
    lengths_.push_back(len);
    formats_.push_back(fmt);
}

void Command::storeData(void const* const arg, size_t const len) {
    auto       storage = buf_.data();
    auto const old_len = buf_.size();
    auto const new_len = old_len + len;
    buf_.resize(new_len);
    if (buf_.data() != storage) {
        storage = buf_.data();
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

void Command::setStatement(std::string&& stmt) {
    stmt_buf_ = std::move(stmt);
    stmt_ = stmt_buf_.data();
}

void Command::setStatement(std::string const& stmt) {
    stmt_ = stmt.data();
}

void Command::setStatement(std::string_view const stmt) {
    stmt_ = stmt.data();
}

void Command::setStatement(char const* const stmt) {
    stmt_ = stmt;
}

}  // namespace postgres
