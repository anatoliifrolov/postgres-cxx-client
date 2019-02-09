#pragma once

#include <string>
#include <vector>
#include <type_traits>
#include <postgres/Result.h>
#include <postgres/Transaction.h>
#include <postgres/PrepareData.h>
#include <postgres/Connection.h>
#include <postgres/Generators.h>

namespace postgres {

class Config;
class Command;
class PreparedCommand;

class Client {
public:
    explicit Client();
    explicit Client(const Config& config);
    Client(const Client&) = delete;
    Client(Client&&);
    Client& operator=(const Client&) = delete;
    Client& operator=(Client&&);
    ~Client();

    Result setSchema(const std::string& schema, const bool cache = true);
    Result trySetSchema(const std::string& schema, const bool cache = true);
    Result prepare(const PrepareData& statement, const bool cache = true);
    Result tryPrepare(const PrepareData& statement, const bool cache = true);

    Transaction begin();

    template <typename... Ts>
    Result execute(const Ts&... statements) {
        return validate(tryExecute(statements...));
    };

    template <typename... Ts>
    std::enable_if_t<(sizeof... (Ts) > 1), Result>
    tryExecute(const Ts&... statements) {
        return completeTransaction(doTryExecute("BEGIN", statements...));
    }

    template <typename T>
    Result tryExecute(const T& statement) {
        return doTryExecute(statement);
    }

    // Insert helpers.
    template <typename T>
    Result insert(const T& val) {
        return validate(tryInsert(val));
    }

    template <typename Iterator>
    Result insert(const Iterator it, const Iterator end) {
        return validate(tryInsert(it, end));
    }

    template <typename T>
    std::enable_if_t<internal::isVisitable<T>(), Result>
    tryInsert(const T& val) {
        return tryExecute(Command{insertStatement<T>(), val});
    }

    template <typename Iterator>
    std::enable_if_t<internal::isVisitable<typename Iterator::value_type>(), Result>
    tryInsert(const Iterator it, const Iterator end) {
        return tryExecute(Command{multiInsertStatement(it, end), it, end});
    }

    // Insert helpers handling conflicts.
    template <typename T>
    Result insertWeak(const T& val) {
        return validate(tryInsertWeak(val));
    }

    template <typename Iterator>
    Result insertWeak(const Iterator it, const Iterator end) {
        return validate(tryInsertWeak(it, end));
    }

    template <typename T>
    std::enable_if_t<internal::isVisitable<T>(), Result>
    tryInsertWeak(const T& val) {
        return tryExecute(Command{insertStatementWeak<T>(), val});
    }

    template <typename Iterator>
    std::enable_if_t<internal::isVisitable<typename Iterator::value_type>(), Result>
    tryInsertWeak(const Iterator it, const Iterator end) {
        return tryExecute(Command{multiInsertStatementWeak(it, end), it, end});
    }

    // Update helpers.
    template <typename T>
    Result update(const T& val) {
        return validate(tryUpdate(val));
    }

    template <typename T>
    std::enable_if_t<internal::isVisitable<T>(), Result>
    tryUpdate(const T& val) {
        return tryExecute(Command{updateStatement<T>(), val});
    }

    // Select helpers.
    template <typename T>
    Result select(std::vector<T>& vals) {
        return validate(trySelect(vals));
    }

    template <typename T>
    std::enable_if_t<internal::isVisitable<T>(), Result>
    trySelect(std::vector<T>& vals) {
        auto res = tryExecute(selectStatement<T>());
        if (!res) {
            return res;
        }
        vals.reserve(res.size());
        for (auto tuple : res) {
            vals.emplace_back();
            tuple >> vals.back();
        }
        return res;
    }

    // Async interface.
    template <typename... Ts>
    Connection& send(Ts&&... args) {
        POSTGRES_CXX_ASSERT(connection_.send(std::forward<Ts>(args)...), connection_.errorMessage());
        return connection_;
    }

    template <typename... Ts>
    Connection* trySend(Ts&&... args) {
        return connection_.send(std::forward<Ts>(args)...) ? &connection_ : nullptr;
    }

    void reconnect();
    bool tryReconnect();
    Connection& connection();

private:
    template <typename T, typename... Ts>
    std::enable_if_t<(sizeof... (Ts) > 0), Result>
    doTryExecute(const T& statement, const Ts&... statements) {
        auto res = doTryExecute(statement);
        if (res) {
            return doTryExecute(statements...);
        }
        return res;
    };

    template <typename T, typename = decltype (T::operator std::string)>
    Result doTryExecute(const T& statement) {
        return doTryExecute(statement.operator std::string());
    }

    template <typename T>
    Result doTryExecute(const T& statement) {
        return connection_.execute(statement);
    }

    Result completeTransaction(Result res);
    Result validate(Result res);

    Connection connection_;
    std::string schema_;
    std::vector<PrepareData> prepared_;
};

}  // namespace postgres
