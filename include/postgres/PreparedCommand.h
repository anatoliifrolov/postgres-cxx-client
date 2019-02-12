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

    PreparedCommand(PreparedCommand const& other) = delete;
    PreparedCommand& operator=(PreparedCommand const& other) = delete;
    PreparedCommand(PreparedCommand&& other) noexcept;
    PreparedCommand& operator=(PreparedCommand&& other) noexcept;
    ~PreparedCommand();
};

}  // namespace postgres
