#pragma once

#define _POSTGRES_CXX_ACCEPT_DEF(T, arg) visitor.accept(#T, #arg);
#define _POSTGRES_CXX_ACCEPT_FLD(T, arg) visitor.accept(#T, #arg, arg);

#define _POSTGRES_CXX_VISIT1(accept, T, arg)      accept(T, arg)
#define _POSTGRES_CXX_VISIT2(accept, T, arg, ...) accept(T, arg) _POSTGRES_CXX_VISIT1(accept, T, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT3(accept, T, arg, ...) accept(T, arg) _POSTGRES_CXX_VISIT2(accept, T, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT4(accept, T, arg, ...) accept(T, arg) _POSTGRES_CXX_VISIT3(accept, T, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT5(accept, T, arg, ...) accept(T, arg) _POSTGRES_CXX_VISIT4(accept, T, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT6(accept, T, arg, ...) accept(T, arg) _POSTGRES_CXX_VISIT5(accept, T, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT7(accept, T, arg, ...) accept(T, arg) _POSTGRES_CXX_VISIT6(accept, T, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT8(accept, T, arg, ...) accept(T, arg) _POSTGRES_CXX_VISIT7(accept, T, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT9(accept, T, arg, ...) accept(T, arg) _POSTGRES_CXX_VISIT8(accept, T, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT10(accept, T, arg, ...) accept(T, arg) _POSTGRES_CXX_VISIT9(accept, T, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT11(accept, T, arg, ...) accept(T, arg) _POSTGRES_CXX_VISIT10(accept, T, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT12(accept, T, arg, ...) accept(T, arg) _POSTGRES_CXX_VISIT11(accept, T, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT13(accept, T, arg, ...) accept(T, arg) _POSTGRES_CXX_VISIT12(accept, T, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT14(accept, T, arg, ...) accept(T, arg) _POSTGRES_CXX_VISIT13(accept, T, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT15(accept, T, arg, ...) accept(T, arg) _POSTGRES_CXX_VISIT14(accept, T, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT16(accept, T, arg, ...) accept(T, arg) _POSTGRES_CXX_VISIT15(accept, T, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT17(accept, T, arg, ...) accept(T, arg) _POSTGRES_CXX_VISIT16(accept, T, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT18(accept, T, arg, ...) accept(T, arg) _POSTGRES_CXX_VISIT17(accept, T, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT19(accept, T, arg, ...) accept(T, arg) _POSTGRES_CXX_VISIT18(accept, T, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT20(accept, T, arg, ...) accept(T, arg) _POSTGRES_CXX_VISIT19(accept, T, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT21(accept, T, arg, ...) accept(T, arg) _POSTGRES_CXX_VISIT20(accept, T, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT22(accept, T, arg, ...) accept(T, arg) _POSTGRES_CXX_VISIT21(accept, T, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT23(accept, T, arg, ...) accept(T, arg) _POSTGRES_CXX_VISIT22(accept, T, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT24(accept, T, arg, ...) accept(T, arg) _POSTGRES_CXX_VISIT23(accept, T, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT25(accept, T, arg, ...) accept(T, arg) _POSTGRES_CXX_VISIT24(accept, T, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT26(accept, T, arg, ...) accept(T, arg) _POSTGRES_CXX_VISIT25(accept, T, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT27(accept, T, arg, ...) accept(T, arg) _POSTGRES_CXX_VISIT26(accept, T, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT28(accept, T, arg, ...) accept(T, arg) _POSTGRES_CXX_VISIT27(accept, T, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT29(accept, T, arg, ...) accept(T, arg) _POSTGRES_CXX_VISIT28(accept, T, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT30(accept, T, arg, ...) accept(T, arg) _POSTGRES_CXX_VISIT29(accept, T, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT31(accept, T, arg, ...) accept(T, arg) _POSTGRES_CXX_VISIT30(accept, T, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT32(accept, T, arg, ...) accept(T, arg) _POSTGRES_CXX_VISIT31(accept, T, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT33(accept, T, arg, ...) accept(T, arg) _POSTGRES_CXX_VISIT32(accept, T, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT34(accept, T, arg, ...) accept(T, arg) _POSTGRES_CXX_VISIT33(accept, T, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT35(accept, T, arg, ...) accept(T, arg) _POSTGRES_CXX_VISIT34(accept, T, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT36(accept, T, arg, ...) accept(T, arg) _POSTGRES_CXX_VISIT35(accept, T, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT37(accept, T, arg, ...) accept(T, arg) _POSTGRES_CXX_VISIT36(accept, T, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT38(accept, T, arg, ...) accept(T, arg) _POSTGRES_CXX_VISIT37(accept, T, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT39(accept, T, arg, ...) accept(T, arg) _POSTGRES_CXX_VISIT38(accept, T, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT40(accept, T, arg, ...) accept(T, arg) _POSTGRES_CXX_VISIT39(accept, T, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT41(accept, T, arg, ...) accept(T, arg) _POSTGRES_CXX_VISIT40(accept, T, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT42(accept, T, arg, ...) accept(T, arg) _POSTGRES_CXX_VISIT41(accept, T, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT43(accept, T, arg, ...) accept(T, arg) _POSTGRES_CXX_VISIT42(accept, T, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT44(accept, T, arg, ...) accept(T, arg) _POSTGRES_CXX_VISIT43(accept, T, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT45(accept, T, arg, ...) accept(T, arg) _POSTGRES_CXX_VISIT44(accept, T, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT46(accept, T, arg, ...) accept(T, arg) _POSTGRES_CXX_VISIT45(accept, T, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT47(accept, T, arg, ...) accept(T, arg) _POSTGRES_CXX_VISIT46(accept, T, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT48(accept, T, arg, ...) accept(T, arg) _POSTGRES_CXX_VISIT47(accept, T, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT49(accept, T, arg, ...) accept(T, arg) _POSTGRES_CXX_VISIT48(accept, T, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT50(accept, T, arg, ...) accept(T, arg) _POSTGRES_CXX_VISIT49(accept, T, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT51(accept, T, arg, ...) accept(T, arg) _POSTGRES_CXX_VISIT50(accept, T, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT52(accept, T, arg, ...) accept(T, arg) _POSTGRES_CXX_VISIT51(accept, T, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT53(accept, T, arg, ...) accept(T, arg) _POSTGRES_CXX_VISIT52(accept, T, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT54(accept, T, arg, ...) accept(T, arg) _POSTGRES_CXX_VISIT53(accept, T, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT55(accept, T, arg, ...) accept(T, arg) _POSTGRES_CXX_VISIT54(accept, T, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT56(accept, T, arg, ...) accept(T, arg) _POSTGRES_CXX_VISIT55(accept, T, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT57(accept, T, arg, ...) accept(T, arg) _POSTGRES_CXX_VISIT56(accept, T, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT58(accept, T, arg, ...) accept(T, arg) _POSTGRES_CXX_VISIT57(accept, T, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT59(accept, T, arg, ...) accept(T, arg) _POSTGRES_CXX_VISIT58(accept, T, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT60(accept, T, arg, ...) accept(T, arg) _POSTGRES_CXX_VISIT59(accept, T, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT61(accept, T, arg, ...) accept(T, arg) _POSTGRES_CXX_VISIT60(accept, T, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT62(accept, T, arg, ...) accept(T, arg) _POSTGRES_CXX_VISIT61(accept, T, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT63(accept, T, arg, ...) accept(T, arg) _POSTGRES_CXX_VISIT62(accept, T, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT64(accept, T, arg, ...) accept(T, arg) _POSTGRES_CXX_VISIT63(accept, T, __VA_ARGS__)

#define _POSTGRES_CXX_OVERLOAD(\
    _1, \
    _2, \
    _3, \
    _4, \
    _5, \
    _6, \
    _7, \
    _8, \
    _9, \
    _10, \
    _11, \
    _12, \
    _13, \
    _14, \
    _15, \
    _16, \
    _17, \
    _18, \
    _19, \
    _20, \
    _21, \
    _22, \
    _23, \
    _24, \
    _25, \
    _26, \
    _27, \
    _28, \
    _29, \
    _30, \
    _31, \
    _32, \
    _33, \
    _34, \
    _35, \
    _36, \
    _37, \
    _38, \
    _39, \
    _40, \
    _41, \
    _42, \
    _43, \
    _44, \
    _45, \
    _46, \
    _47, \
    _48, \
    _49, \
    _50, \
    _51, \
    _52, \
    _53, \
    _54, \
    _55, \
    _56, \
    _57, \
    _58, \
    _59, \
    _60, \
    _61, \
    _62, \
    _63, \
    _64, \
    N, ...) N

#define _POSTGRES_CXX_VISIT(accept, T, ...) _POSTGRES_CXX_OVERLOAD( \
    __VA_ARGS__, \
    _POSTGRES_CXX_VISIT64, \
    _POSTGRES_CXX_VISIT63, \
    _POSTGRES_CXX_VISIT62, \
    _POSTGRES_CXX_VISIT61, \
    _POSTGRES_CXX_VISIT60, \
    _POSTGRES_CXX_VISIT59, \
    _POSTGRES_CXX_VISIT58, \
    _POSTGRES_CXX_VISIT57, \
    _POSTGRES_CXX_VISIT56, \
    _POSTGRES_CXX_VISIT55, \
    _POSTGRES_CXX_VISIT54, \
    _POSTGRES_CXX_VISIT53, \
    _POSTGRES_CXX_VISIT52, \
    _POSTGRES_CXX_VISIT51, \
    _POSTGRES_CXX_VISIT50, \
    _POSTGRES_CXX_VISIT49, \
    _POSTGRES_CXX_VISIT48, \
    _POSTGRES_CXX_VISIT47, \
    _POSTGRES_CXX_VISIT46, \
    _POSTGRES_CXX_VISIT45, \
    _POSTGRES_CXX_VISIT44, \
    _POSTGRES_CXX_VISIT43, \
    _POSTGRES_CXX_VISIT42, \
    _POSTGRES_CXX_VISIT41, \
    _POSTGRES_CXX_VISIT40, \
    _POSTGRES_CXX_VISIT39, \
    _POSTGRES_CXX_VISIT38, \
    _POSTGRES_CXX_VISIT37, \
    _POSTGRES_CXX_VISIT36, \
    _POSTGRES_CXX_VISIT35, \
    _POSTGRES_CXX_VISIT34, \
    _POSTGRES_CXX_VISIT33, \
    _POSTGRES_CXX_VISIT32, \
    _POSTGRES_CXX_VISIT31, \
    _POSTGRES_CXX_VISIT30, \
    _POSTGRES_CXX_VISIT29, \
    _POSTGRES_CXX_VISIT28, \
    _POSTGRES_CXX_VISIT27, \
    _POSTGRES_CXX_VISIT26, \
    _POSTGRES_CXX_VISIT25, \
    _POSTGRES_CXX_VISIT24, \
    _POSTGRES_CXX_VISIT23, \
    _POSTGRES_CXX_VISIT22, \
    _POSTGRES_CXX_VISIT21, \
    _POSTGRES_CXX_VISIT20, \
    _POSTGRES_CXX_VISIT19, \
    _POSTGRES_CXX_VISIT18, \
    _POSTGRES_CXX_VISIT17, \
    _POSTGRES_CXX_VISIT16, \
    _POSTGRES_CXX_VISIT15, \
    _POSTGRES_CXX_VISIT14, \
    _POSTGRES_CXX_VISIT13, \
    _POSTGRES_CXX_VISIT12, \
    _POSTGRES_CXX_VISIT11, \
    _POSTGRES_CXX_VISIT10, \
    _POSTGRES_CXX_VISIT9, \
    _POSTGRES_CXX_VISIT8, \
    _POSTGRES_CXX_VISIT7, \
    _POSTGRES_CXX_VISIT6, \
    _POSTGRES_CXX_VISIT5, \
    _POSTGRES_CXX_VISIT4, \
    _POSTGRES_CXX_VISIT3, \
    _POSTGRES_CXX_VISIT2, \
    _POSTGRES_CXX_VISIT1)(accept, T, __VA_ARGS__)

#define POSTGRES_CXX_TABLE(T, ...) \
    static auto constexpr _POSTGRES_CXX_VISITABLE = true; \
    static auto constexpr _POSTGRES_CXX_TABLE_NAME = #T; \
    template <typename V> \
    static void visitPostgresDefinition(V& visitor) { \
        _POSTGRES_CXX_VISIT(_POSTGRES_CXX_ACCEPT_DEF, T, __VA_ARGS__) \
    } \
    template <typename V> \
    void visitPostgresFields(V& visitor) const { \
        _POSTGRES_CXX_VISIT(_POSTGRES_CXX_ACCEPT_FLD, T, __VA_ARGS__) \
    } \
    template <typename V> \
    void visitPostgresFields(V& visitor) { \
        _POSTGRES_CXX_VISIT(_POSTGRES_CXX_ACCEPT_FLD, T, __VA_ARGS__) \
    }

