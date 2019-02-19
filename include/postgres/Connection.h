#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>
#include <libpq-fe.h>
#include <postgres/internal/Classifier.h>
#include <postgres/Command.h>
#include <postgres/Result.h>
#include <postgres/Statement.h>
#include <postgres/Transaction.h>

namespace postgres {

class Config;
class PreparedCommand;
class Receiver;
struct PrepareData;

class Connection {
public:
    static PGPing ping(Config const& cfg);
    static PGPing ping(std::string const& uri);

    explicit Connection();
    explicit Connection(Config const& cfg);
    explicit Connection(std::string const& uri);
    Connection(Connection const& other) = delete;
    Connection& operator=(Connection const& other) = delete;
    Connection(Connection&& other) noexcept;
    Connection& operator=(Connection&& other) noexcept;
    ~Connection() noexcept;

    template <typename T>
    std::enable_if_t<internal::isVisitable<T>(), Result> insert(T const& val) {
        return exec(Command{Statement<T>::insert(), val});
    }

    template <typename Iter>
    std::enable_if_t<internal::isVisitable<typename Iter::value_type>(), Result>
    insert(Iter const it, Iter const end) {
        return exec(Command{RangeStatement::insert(it, end), std::make_pair(it, end)});
    }

    template <typename T>
    std::enable_if_t<internal::isVisitable<T>(), Result> update(T const& val) {
        return exec(Command{Statement<T>::update(), val});
    }

    template <typename T>
    std::enable_if_t<internal::isVisitable<T>(), Result> select(std::vector<T>& out) {
        auto res = exec(Command{Statement<T>::select()});
        if (!res.isOk()) {
            return res;
        }

        out.reserve(out.size() + res.size());
        for (auto row : res) {
            out.emplace_back();
            row >> out.back();
        }
        return res;
    }

    template <typename... Ts>
    std::enable_if_t<(1 < sizeof... (Ts)), Result> transact(Ts&& ... args) {
        auto tx = begin();
        return tx.complete(exec(std::forward<Ts>(args)...));
    }

    Result exec(PrepareData const& data);
    Result exec(Command const& cmd);
    Result exec(PreparedCommand const& cmd);
    Result execRaw(std::string_view stmt);

    Receiver send(PrepareData const& data);
    Receiver send(Command const& cmd);
    Receiver send(PreparedCommand const& cmd);
    Receiver sendRaw(std::string_view stmt);

    Receiver iter(Command const& cmd);
    Receiver iter(PreparedCommand const& cmd);

    Transaction begin();

    bool reset();
    bool isOk();
    std::string message();

    std::string esc(std::string const& in);
    std::string escId(std::string const& in);

    PGconn* native() const;

private:
    template <typename T, typename... Ts>
    std::enable_if_t<(0 < sizeof... (Ts)), Result> exec(T&& arg, Ts&& ... args) {
        auto res = exec(std::forward<T>(arg));
        if (!res.isOk()) {
            return res;
        }
        return exec(std::forward<Ts>(args)...);
    };

    std::shared_ptr<PGconn> conn_;
};

}  // namespace postgres
