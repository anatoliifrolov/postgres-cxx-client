#include <postgres/internal/Channel.h>

namespace postgres {
namespace internal {

Channel::Channel() = default;

Channel::Channel(Channel&& other) noexcept = default;

Channel& Channel::operator=(Channel&& other) noexcept = default;

Channel::~Channel() noexcept = default;

}  // namespace internal
}  // namespace postgres
