#pragma once

#include <optional>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>
#include <postgres/internal/Bytes.h>
#include <postgres/internal/Classifier.h>
#include <postgres/Oid.h>
#include <postgres/Timestamp.h>

namespace postgres {

class Command {
public:
    template <typename Stmt, typename... Args>
    explicit Command(Stmt&& stmt, Args&& ... args)
        : stmt_{std::forward<Stmt>(stmt)} {
        unwind(std::forward<Args>(args)...);
    }

    Command(Command const& other) = delete;
    Command& operator=(Command const& other) = delete;
    Command(Command&& other) noexcept;
    Command& operator=(Command&& other) noexcept;
    ~Command();

    // Dynamic arguments addition.
    template <typename Arg>
    Command& operator<<(Arg&& arg) {
        add(std::forward<Arg>(arg));
        return *this;
    }

    // Visitor interface.
    template <typename Arg>
    void accept(char const*, char const*, Arg& arg) {
        add(std::forward<Arg>(arg));
    };

    // libpq interface adapters.
    char const* statement() const;
    int count() const;
    Oid const* types() const;
    char const* const* values() const;
    int const* lengths() const;
    int const* formats() const;

private:
    template <typename Arg, typename... Args>
    void unwind(Arg&& arg, Args&& ... args) {
        add(std::forward<Arg>(arg));
        unwind(std::forward<Args>(args)...);
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
    template <typename Arg>
    std::enable_if_t<internal::isVisitable<Arg>()> add(Arg& arg) {
        arg.visitPostgresFields(*this);
    }

    template <typename Arg>
    void add(internal::OidBinding<Arg> arg) {
        add(std::forward<Arg>(arg.val));
        types_.back() = arg.type;
    }

    template <typename Arg>
    void add(std::optional<Arg>& arg) {
        arg.has_value() ? add(arg.value()) : add(nullptr);
    }

    template <typename Arg>
    void add(Arg const* const arg) {
        arg ? add(*arg) : add(nullptr);
    }

    template <typename Arg>
    std::enable_if_t<std::is_arithmetic_v<Arg>> add(Arg arg) {
        auto constexpr LEN = sizeof(arg);
        static_assert(LEN <= 8, "Unexpected arithmetic argument type length");

        auto constexpr ID = []() -> Oid {
            if (std::is_same_v<Arg, bool>) {
                return BOOLOID;
            }
            if (std::is_integral_v<Arg>) {
                return ((Oid[]) {INT2OID, INT4OID, INT8OID})[LEN / 4];
            }
            if (std::is_floating_point_v<Arg>) {
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
    void add(std::chrono::system_clock::time_point arg);
    void add(Timestamp const& arg);
    void add(std::string const& arg);
    void add(std::string&& arg);
    void add(char const* arg);
    void addText(char const* arg, size_t len);
    void setMeta(Oid id, int len, int fmt);
    void storeData(void const* arg, size_t len);

    std::string              stmt_;
    std::vector<Oid>         types_;
    std::vector<char const*> values_;
    std::vector<int>         lengths_;
    std::vector<int>         formats_;
    std::vector<char>        buffer_;
};

}  // namespace postgres
