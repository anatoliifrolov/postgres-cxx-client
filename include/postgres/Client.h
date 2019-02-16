#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <type_traits>
#include <postgres/Result.h>
#include <postgres/Transaction.h>
#include <postgres/PrepareData.h>
#include <postgres/Connection.h>
#include <postgres/Statement.h>

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

    Result prepare(const PrepareData& statement);
    Result tryPrepare(const PrepareData& statement);

    Transaction begin();

    template <typename... Ts>
    Result execute(const Ts& ... statements) {
        return validate(tryExecute(statements...));
    };

    template <typename... Ts>
    std::enable_if_t<(sizeof... (Ts) > 1), Result> tryExecute(const Ts& ... statements) {
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
    std::enable_if_t<internal::isVisitable<T>(), Result> tryInsert(const T& val) {
        return tryExecute(Command{Statement<T>::insert(), val});
    }

    template <typename Iterator>
    std::enable_if_t<internal::isVisitable<typename Iterator::value_type>(), Result>
    tryInsert(const Iterator it, const Iterator end) {
        return tryExecute(Command{RangeStatement::insert(it, end), std::make_pair(it, end)});
    }

    // Update helpers.
    template <typename T>
    Result update(const T& val) {
        return validate(tryUpdate(val));
    }

    template <typename T>
    std::enable_if_t<internal::isVisitable<T>(), Result> tryUpdate(const T& val) {
        return tryExecute(Command{Statement<T>::update(), val});
    }

    // Select helpers.
    template <typename T>
    Result select(std::vector<T>& vals) {
        return validate(trySelect(vals));
    }

    template <typename T>
    std::enable_if_t<internal::isVisitable<T>(), Result> trySelect(std::vector<T>& vals) {
        auto res = tryExecute(Statement<T>::select());
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
//    template <typename... Ts>
//    Connection& send(Ts&& ... args) {
//        _POSTGRES_CXX_ASSERT(conn_.send(std::forward<Ts>(args)...), conn_.error());
//        return conn_;
//    }
//
//    template <typename... Ts>
//    Connection* trySend(Ts&& ... args) {
//        return conn_.send(std::forward<Ts>(args)...) ? &conn_ : nullptr;
//    }

    void reconnect();
    bool tryReconnect();
    Connection& connection();

private:
    template <typename T, typename... Ts>
    std::enable_if_t<(sizeof... (Ts) > 0), Result>
    doTryExecute(const T& statement, const Ts& ... statements) {
        auto res = doTryExecute(statement);
        if (res) {
            return doTryExecute(statements...);
        }
        return res;
    };

    Result doTryExecute(std::string&& stmt);
    Result doTryExecute(std::string const& stmt);
    Result doTryExecute(std::string_view stmt);
    Result doTryExecute(char const* stmt);
    Result doTryExecute(PreparedCommand const& cmd);
    Result doTryExecute(Command const& cmd);
    Result completeTransaction(Result res);
    Result validate(Result res);

    Connection conn_;
};

}  // namespace postgres
