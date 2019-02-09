#pragma once

#include <utility>
#include <type_traits>
#include <string>
#include <vector>
#include <cstddef>
#include <cstring>
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
    template <typename Statement, typename... Params>
    explicit Command(Statement&& statement, Params&&... params)
        : statement_{std::forward<Statement>(statement)}
    {
        unwind(std::forward<Params>(params)...);
    }

    template <typename Statement, typename Iterator, typename = typename Iterator::iterator_category>
    explicit Command(Statement&& statement, Iterator it, const Iterator end)
        : statement_{std::forward<Statement>(statement)}
    {
        add(std::make_pair(it, end));
    }

    Command(const Command& other) = delete;
    Command(Command&& other);
    Command& operator=(const Command& other) = delete;
    Command& operator=(Command&& other);
    ~Command();

    // libpq interface adapters.
    const char* statement() const;
    int nParams() const;
    const Oid* paramTypes() const;
    const char* const* paramValues() const;
    const int* paramLengths() const;
    const int* paramFormats() const;
    int resultFormat() const;

    // Visitor interface.
    template <typename T>
    void accept(const char* const table, const char* const name, T& val) {
        add(val);
    };

    // Dynamic parameter addition.
    template <typename Param>
    Command& operator<<(Param&& param) {
        add(std::forward<Param>(param));
        return *this;
    }

private:
    template <typename Param, typename... Params>
    void unwind(Param&& param, Params&&... params) {
        add(std::forward<Param>(param));
        unwind(std::forward<Params>(params)...);
    };

    void unwind() const {}

    // Handle containers.
    template <typename Param, typename = typename Param::iterator>
    void add(const Param& params) {
        add(std::make_pair(params.begin(), params.end()));
    };

    template <typename Iterator, typename = typename Iterator::iterator_category>
    void add(std::pair<Iterator, Iterator> range) {
        for (; range.first != range.second; ++range.first) {
            add(*range.first);
        }
    };

    // Handle visitable structures.
    template <typename Param>
    std::enable_if_t<internal::isVisitable<Param>()>
    add(Param& param) {
        param.visitPostgresFields(*this);
    }

    // Handle special types.
    template <typename Param>
    void add(OidBinding<Param> param) {
        add(std::forward<Param>(param.val_));
        types_.back() = param.type_;
    }

    // Handle optional types.
    template <typename Param>
    std::enable_if_t<internal::isOptional<Param>()>
    add(Param& param) {
        param.has_value() ? add(param.value()) : add(nullptr);
    }

    template <typename Param>
    void add(const Param* const param) {
        param ? add(*param) : add(nullptr);
    }

    void add(std::nullptr_t);

    // Handle timestamps.
    void add(const std::chrono::system_clock::time_point param);
    void add(const Timestamp& param);

    // Handle strings.
    void add(const std::string& param);
    void add(std::string&& param);
    void add(const char* const param);
    void addText(const char* const param, const int size);

    // Handle arithmetic types.
    template <typename Param>
    std::enable_if_t<std::is_arithmetic<Param>::value> add(const Param param) {
        if constexpr (std::is_integral<Param>::value) {
            switch (sizeof (Param)) {
                case 1: return addBinary(param, BYTEAOID);
                case 2: return addBinary(param, INT2OID);
                case 4: return addBinary(param, INT4OID);
                case 8: return addBinary(param, INT8OID);
            }
        }

        if constexpr (std::is_floating_point<Param>::value) {
            switch (sizeof (Param)) {
                case 4: return addBinary(param, FLOAT4OID);
                case 8: return addBinary(param, FLOAT8OID);
            }
        }

        _POSTGRES_CXX_FAIL("Unexpected arithmetic argument type");
    };

    template <typename Param>
    void addBinary(Param param, const int type) {
        static auto constexpr size = sizeof (Param);
        param = internal::orderBytes(param);
        setMeta(type, size, 1);
        storeData(&param, size);
    }

    void add(const bool param);

    // Misc.
    void setMeta(const Oid type, const int size, const int format);
    void storeData(const void* const param, const int size);

    std::string statement_;
    std::vector<Oid> types_;
    std::vector<const char*> values_;
    std::vector<int> lenghts_;
    std::vector<int> formats_;
    std::vector<char> buffer_;
};

}  // namespace postgres
