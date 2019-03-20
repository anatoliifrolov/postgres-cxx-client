#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>
#include <postgres/internal/Bytes.h>
#include <postgres/internal/Classifier.h>
#include <postgres/Oid.h>
#include <postgres/Time.h>

namespace postgres {

class Command {
public:
    template <typename Stmt, typename... Args>
    /*explicit*/ Command(Stmt&& stmt, Args&& ... args) {
        auto constexpr NARGS = sizeof... (Args);
        types_.reserve(NARGS);
        values_.reserve(NARGS);
        lengths_.reserve(NARGS);
        formats_.reserve(NARGS);
        setStatement(std::forward<Stmt>(stmt));
        unwind(std::forward<Args>(args)...);
    }

    Command(Command const& other) = delete;
    Command& operator=(Command const& other) = delete;
    Command(Command&& other) noexcept;
    Command& operator=(Command&& other) noexcept;
    ~Command() noexcept;

    // Dynamic arguments addition.
    template <typename T>
    Command& operator<<(T&& arg) {
        add(std::forward<T>(arg));
        return *this;
    }

    // Visitor interface.
    template <typename T>
    void accept(char const*, T& arg) {
        add(std::forward<T>(arg));
    };

    // libpq interface adapters.
    char const* statement() const;
    int count() const;
    Oid const* types() const;
    char const* const* values() const;
    int const* lengths() const;
    int const* formats() const;

private:
    template <typename T, typename... Ts>
    void unwind(T&& arg, Ts&& ... args) {
        add(std::forward<T>(arg));
        unwind(std::forward<Ts>(args)...);
    };

    void unwind() const {
    }

    template <typename Iter>
    void add(std::pair<Iter, Iter> const rng) {
        for (auto it = rng.first; it != rng.second; ++it) {
            add(*it);
        }
    };

    // Use mutable reference to disallow temporaries.
    template <typename T>
    std::enable_if_t<internal::isVisitable<T>()> add(T& arg) {
        arg.visitPostgresFields(*this);
    }

    template <typename T>
    void add(OidBinding<T> arg) {
        add(std::move(arg.value));
        types_.back() = arg.type;
    }

    template <typename T>
    void add(std::optional<T>& arg) {
        arg.has_value() ? add(arg.value()) : add(nullptr);
    }

    template <typename T>
    void add(T const* const arg) {
        arg ? add(*arg) : add(nullptr);
    }

    template <typename T>
    std::enable_if_t<std::is_arithmetic_v<T>> add(T arg) {
        auto constexpr LEN = sizeof(arg);
        static_assert(LEN <= 8, "Unexpected arithmetic argument type length");

        auto constexpr ID = []() -> Oid {
            if (std::is_same_v<T, bool>) {
                return BOOLOID;
            }
            if (std::is_integral_v<T>) {
                return ((Oid[]) {INT2OID, INT4OID, INT8OID})[LEN / 4];
            }
            if (std::is_floating_point_v<T>) {
                return ((Oid[]) {UNKNOWNOID, FLOAT4OID, FLOAT8OID})[LEN / 4];
            }
            return UNKNOWNOID;
        }();
        static_assert(ID != UNKNOWNOID, "Unexpected arithmetic argument type");

        arg = internal::orderBytes(arg);
        setMeta(ID, LEN, 1);
        storeData(&arg, LEN);
    };

    void add(std::nullptr_t);
    void add(std::chrono::system_clock::time_point t);
    void add(Time const& t);
    void add(std::string const& s);
    void add(std::string_view s);
    void add(char const* s);
    void addText(char const* s, size_t len);
    void setMeta(Oid id, int len, int fmt);
    void storeData(void const* arg, size_t len);

    void setStatement(std::string stmt);
    void setStatement(std::string_view stmt);
    void setStatement(char const* stmt);

    char const* stmt_ = nullptr;
    std::string              stmt_buf_;
    std::vector<Oid>         types_;
    std::vector<char const*> values_;
    std::vector<int>         lengths_;
    std::vector<int>         formats_;
    std::vector<char>        buf_;
};

}  // namespace postgres
