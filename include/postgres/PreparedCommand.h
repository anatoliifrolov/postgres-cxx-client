#pragma once

#include <utility>
#include <postgres/Command.h>

namespace postgres {

class PreparedCommand : public Command {
public:
    template <typename... Ts>
    explicit PreparedCommand(Ts&& ... args)
        : Command{std::forward<Ts>(args)...} {
    }

    PreparedCommand(const PreparedCommand& other) = delete;
    PreparedCommand(PreparedCommand&& other) = delete;
    PreparedCommand& operator=(const PreparedCommand& other) = delete;
    PreparedCommand& operator=(PreparedCommand&& other) = delete;
    ~PreparedCommand();
};

}  // namespace postgres
