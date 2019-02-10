#pragma once

#include <libpq-fe.h>

namespace postgres {
namespace internal {

template <typename T>
struct OidBinding {
    T&& val;
    Oid const type;
};

}  // namespace internal
}  // namespace postgres
