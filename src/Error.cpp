#include <postgres/Error.h>

namespace postgres {

Error::Error(std::string const& msg)
    : std::runtime_error(msg) {
}

}  // namespace postgres
