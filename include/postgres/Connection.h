#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>
#include <libpq-fe.h>
#include <postgres/Command.h>
#include <postgres/Result.h>
#include <postgres/Row.h>
#include <postgres/Statement.h>
#include <postgres/Transaction.h>

namespace postgres {

class Config;
class Consumer;
class PreparedCommand;
class Receiver;
struct PrepareData;

class Connection {
public:
    static PGPing ping();
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
    Status create() {
        return exec(Statement<T>::create());
    }

    template <typename T>
    Status drop() {
        return exec(Statement<T>::drop());
    }

    template <typename T>
    Status insert(T const& val) {
        return exec(Command{Statement<T>::insert(), val});
    }

    template <typename Iter>
    Status insert(Iter const it, Iter const end) {
        return exec(Command{RangeStatement::insert(it, end), std::make_pair(it, end)});
    }

    template <typename T>
    Status update(T const& val) {
        return exec(Command{Statement<T>::update(), val});
    }

    template <typename T>
    Result select(std::vector<T>& out) {
        auto res = exec(Statement<T>::select());
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
        auto res = exec("BEGIN", std::forward<Ts>(args)...);
        if (!res.isOk()) {
            exec("ROLLBACK");
            return res;
        }

        auto status = exec("COMMIT");
        if (!status.isOk()) {
            return status;
        }

        return res;
    }

    Result exec(PrepareData const& prep);
    Result exec(Command const& cmd);
    Result exec(PreparedCommand const& cmd);
    Status execRaw(std::string_view stmt);

    Receiver send(PrepareData const& prep);
    Receiver send(Command const& cmd);
    Receiver send(PreparedCommand const& cmd);
    Consumer sendRaw(std::string_view stmt);

    Receiver iter(Command const& cmd);
    Receiver iter(PreparedCommand const& cmd);

    Transaction begin();

    void check();
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

    template <typename F>
    std::string doEsc(std::string const& in, F f);

    std::shared_ptr<PGconn> handle_;
};

}  // namespace postgres
