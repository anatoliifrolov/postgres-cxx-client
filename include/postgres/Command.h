#pragma once

#include <utility>
#include <type_traits>
#include <string>
#include <vector>
#include <cstddef>
#include <cstring>
#include <optional>
#include <libpq-fe.h>
#include <postgres/OidBinding.h>
#include <postgres/Oid.h>
#include <postgres/Timestamp.h>
#include <postgres/internal/Visitors.h>
#include <postgres/internal/Bytes.h>
#include <postgres/internal/Classifier.h>
#include <postgres/internal/Assert.h>

namespace postgres {

class Command {
public:
    template <typename Stmt, typename... Args>
    explicit Command(Stmt&& stmt, Args&& ... args)
        : stmt_{std::forward<Stmt>(stmt)} {
        unwind(std::forward<Args>(args)...);
    }

    template <typename Stmt, typename Iter, typename = typename Iter::iterator_category>
    explicit Command(Stmt&& stmt, Iter const it, Iter const end)
        : stmt_{std::forward<Stmt>(stmt)} {
        add(std::make_pair(it, end));
    }

    Command(Command const& other) = delete;
    Command& operator=(Command const& other) = delete;
    Command(Command&& other);
    Command& operator=(Command&& other);
    ~Command();

    // Dynamic arguments addition.
    template <typename Arg>
    Command& operator<<(Arg&& arg) {
        add(std::forward<Arg>(arg));
        return *this;
    }

    // Visitor interface.
    template <typename Arg>
    void accept(char const*, char const*, Arg&& arg) {
        add(std::forward<Arg>(arg));
    };

    // libpq interface adapters.
    char const* statement() const;
    int nParams() const;
    Oid const* paramTypes() const;
    char const* const* paramValues() const;
    int const* paramLengths() const;
    int const* paramFormats() const;
    int resultFormat() const;

private:
    template <typename Arg, typename... Args>
    void unwind(Arg&& arg, Args&& ... args) {
        add(std::forward<Arg>(arg));
        unwind(std::forward<Args>(args)...);
    };

    void unwind() const {
    }

    // Containers.
    template <typename Iter>
    void add(std::pair<Iter, Iter> range) {
        for (; range.first != range.second; ++range.first) {
            add(*range.first);
        }
    };

    // Visitable.
    template <typename Arg>
    std::enable_if_t<internal::isVisitable<Arg>()> add(Arg const& arg) {
        arg.visitPostgresFields(*this);
    }

    // Handle special types.
    template <typename Param>
    void add(OidBinding<Param> param) {
        add(std::forward<Param>(param.val_));
        types_.back() = param.type_;
    }

    // Handle optional types.
    template <typename Param>
    void add(std::optional<Param> const& param) {
        param.has_value() ? add(param.value()) : add(nullptr);
    }

    template <typename Param>
    void add(Param const* const param) {
        param ? add(*param) : add(nullptr);
    }

    void add(std::nullptr_t);

    // Handle timestamps.
    void add(std::chrono::system_clock::time_point const param);
    void add(Timestamp const& param);

    // Handle strings.
    void add(std::string const& param);
    void add(std::string&& param);
    void add(char const* const param);
    void addText(char const* const param, int const size);

    // Handle arithmetic types.
    template <typename Param>
    std::enable_if_t<std::is_arithmetic<Param>::value> add(Param const param) {
        if constexpr (std::is_integral<Param>::value) {
            switch (sizeof(Param)) {
                case 1: {
                    return addBinary(param, BYTEAOID);
                }
                case 2: {
                    return addBinary(param, INT2OID);
                }
                case 4: {
                    return addBinary(param, INT4OID);
                }
                case 8: {
                    return addBinary(param, INT8OID);
                }
            }
        }

        if constexpr (std::is_floating_point<Param>::value) {
            switch (sizeof(Param)) {
                case 4: {
                    return addBinary(param, FLOAT4OID);
                }
                case 8: {
                    return addBinary(param, FLOAT8OID);
                }
            }
        }

        _POSTGRES_CXX_FAIL("Unexpected arithmetic argument type");
    };

    template <typename Param>
    void addBinary(Param param, int const type) {
        static auto constexpr size = sizeof(Param);
        param = internal::orderBytes(param);
        setMeta(type, size, 1);
        storeData(&param, size);
    }

    void add(bool const param);

    void setMeta(Oid const id, int const len, int const fmt);
    void storeData(void const* const arg, int const len);

    std::string              stmt_;
    std::vector<Oid>         types_;
    std::vector<char const*> values_;
    std::vector<int>         lenghts_;
    std::vector<int>         formats_;
    std::vector<char>        buffer_;
};

}  // namespace postgres
