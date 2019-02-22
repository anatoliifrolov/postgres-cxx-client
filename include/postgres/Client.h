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
struct PreparingStatement;

class Client {
public:
    static PGPing ping();
    static PGPing ping(Config const& cfg);
    static PGPing ping(std::string const& uri);

    explicit Client();
    explicit Client(Config const& cfg);
    explicit Client(std::string const& uri);
    Client(Client const& other) = delete;
    Client& operator=(Client const& other) = delete;
    Client(Client&& other) noexcept;
    Client& operator=(Client&& other) noexcept;
    ~Client() noexcept;

    template <typename T>
    Result create() {
        return exec(Statement<T>::create());
    }

    template <typename T>
    Result insert(T const& val) {
        return exec(Command{Statement<T>::insert(), val});
    }

    template <typename Iter>
    Result insert(Iter const it, Iter const end) {
        return exec(Command{RangeStatement::insert(it, end), std::make_pair(it, end)});
    }

    template <typename T>
    Result update(T const& val) {
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
        auto tx = begin();
        return tx.complete(exec(std::forward<Ts>(args)...));
    }

    Result exec(PreparingStatement const& stmt);
    Result exec(Command const& cmd);
    Result exec(PreparedCommand const& cmd);
    Status execRaw(std::string_view stmt);

    Receiver send(PreparingStatement const& stmt);
    Receiver send(Command const& cmd);
    Receiver send(PreparedCommand const& cmd);
    Consumer sendRaw(std::string_view stmt);

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

    std::string postEsc(char* escaped);

    std::shared_ptr<PGconn> conn_;
};

}  // namespace postgres
