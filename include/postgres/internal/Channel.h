#pragma once

#include <postgres/Job.h>

namespace postgres {
namespace internal {

class Channel {
public:
    explicit Channel();
    Channel(Channel const& other) = delete;
    Channel& operator=(Channel const& other) = delete;
    Channel(Channel&& other) noexcept;
    Channel& operator=(Channel&& other) noexcept;
    ~Channel() noexcept;

    bool send(Job job);
    bool send(QueryJob job);
    Job receive();
    QueryJob receiveQuery();
};

}  // namespace internal
}  // namespace postgres
