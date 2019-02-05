#pragma once

#include <utility>
#include <postgres/Oid.h>

namespace postgres {

template <typename T>
struct OidBinding {
    T&& val_;
    const Oid type_;
};

template <typename T>
OidBinding<T> bindOid(T&& param, const Oid type) {
    return OidBinding<T>{std::forward<T>(param), type};
}

}  // namespace postgres
