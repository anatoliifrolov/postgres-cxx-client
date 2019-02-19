#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <type_traits>
#include <postgres/internal/Classifier.h>
#include <postgres/Result.h>
#include <postgres/PrepareData.h>
#include <postgres/Connection.h>
#include <postgres/Statement.h>
#include <postgres/Transaction.h>

namespace postgres {

class Config;
class Command;
class PreparedCommand;

class Client {
public:
    explicit Client();
    explicit Client(Config const& config);
    Client(Client const&) = delete;
    Client(Client&&);
    Client& operator=(Client const&) = delete;
    Client& operator=(Client&&);
    ~Client();

    template <typename... Ts>
    std::enable_if_t<(1 < sizeof... (Ts)), Result> transact(Ts&& ... args) {
        auto tx = conn_.begin();
        return tx.complete(exec(std::forward<Ts>(args)...));
    }

    Connection& connection();

private:
    template <typename T, typename... Ts>
    std::enable_if_t<(0 < sizeof... (Ts)), Result> exec(T&& arg, Ts&& ... args) {
        auto res = exec(std::forward<T>(arg));
        if (!res.isOk()) {
            return res;
        }
        return exec(std::forward<Ts>(args)...);
    };

    Result exec(PreparedCommand const& cmd);
    Result exec(Command const& cmd);

    Connection conn_;
};

}  // namespace postgres
