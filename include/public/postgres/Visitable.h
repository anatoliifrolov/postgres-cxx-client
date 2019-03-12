#pragma once

#define _POSTGRES_CXX_ACCEPT_DEF(arg) visitor.template accept<decltype(arg)>(#arg);
#define _POSTGRES_CXX_ACCEPT_FLD(arg) visitor.accept(#arg, arg);

#define _POSTGRES_CXX_VISIT1(accept, arg)      accept(arg)
#define _POSTGRES_CXX_VISIT2(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT1(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT3(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT2(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT4(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT3(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT5(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT4(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT6(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT5(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT7(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT6(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT8(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT7(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT9(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT8(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT10(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT9(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT11(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT10(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT12(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT11(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT13(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT12(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT14(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT13(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT15(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT14(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT16(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT15(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT17(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT16(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT18(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT17(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT19(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT18(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT20(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT19(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT21(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT20(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT22(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT21(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT23(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT22(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT24(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT23(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT25(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT24(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT26(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT25(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT27(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT26(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT28(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT27(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT29(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT28(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT30(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT29(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT31(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT30(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT32(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT31(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT33(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT32(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT34(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT33(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT35(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT34(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT36(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT35(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT37(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT36(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT38(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT37(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT39(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT38(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT40(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT39(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT41(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT40(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT42(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT41(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT43(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT42(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT44(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT43(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT45(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT44(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT46(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT45(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT47(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT46(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT48(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT47(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT49(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT48(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT50(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT49(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT51(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT50(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT52(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT51(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT53(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT52(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT54(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT53(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT55(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT54(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT56(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT55(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT57(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT56(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT58(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT57(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT59(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT58(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT60(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT59(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT61(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT60(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT62(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT61(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT63(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT62(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT64(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT63(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT65(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT64(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT66(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT65(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT67(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT66(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT68(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT67(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT69(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT68(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT70(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT69(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT71(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT70(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT72(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT71(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT73(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT72(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT74(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT73(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT75(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT74(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT76(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT75(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT77(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT76(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT78(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT77(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT79(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT78(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT80(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT79(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT81(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT80(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT82(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT81(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT83(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT82(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT84(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT83(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT85(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT84(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT86(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT85(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT87(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT86(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT88(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT87(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT89(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT88(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT90(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT89(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT91(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT90(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT92(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT91(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT93(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT92(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT94(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT93(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT95(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT94(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT96(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT95(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT97(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT96(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT98(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT97(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT99(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT98(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT100(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT99(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT101(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT100(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT102(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT101(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT103(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT102(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT104(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT103(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT105(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT104(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT106(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT105(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT107(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT106(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT108(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT107(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT109(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT108(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT110(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT109(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT111(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT110(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT112(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT111(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT113(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT112(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT114(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT113(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT115(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT114(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT116(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT115(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT117(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT116(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT118(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT117(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT119(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT118(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT120(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT119(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT121(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT120(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT122(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT121(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT123(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT122(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT124(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT123(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT125(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT124(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT126(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT125(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT127(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT126(accept, __VA_ARGS__)
#define _POSTGRES_CXX_VISIT128(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT127(accept, __VA_ARGS__)

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
    _65, \
    _66, \
    _67, \
    _68, \
    _69, \
    _70, \
    _71, \
    _72, \
    _73, \
    _74, \
    _75, \
    _76, \
    _77, \
    _78, \
    _79, \
    _80, \
    _81, \
    _82, \
    _83, \
    _84, \
    _85, \
    _86, \
    _87, \
    _88, \
    _89, \
    _90, \
    _91, \
    _92, \
    _93, \
    _94, \
    _95, \
    _96, \
    _97, \
    _98, \
    _99, \
    _100, \
    _101, \
    _102, \
    _103, \
    _104, \
    _105, \
    _106, \
    _107, \
    _108, \
    _109, \
    _110, \
    _111, \
    _112, \
    _113, \
    _114, \
    _115, \
    _116, \
    _117, \
    _118, \
    _119, \
    _120, \
    _121, \
    _122, \
    _123, \
    _124, \
    _125, \
    _126, \
    _127, \
    _128, \
    N, ...) N

#define _POSTGRES_CXX_VISIT(accept, ...) _POSTGRES_CXX_OVERLOAD( \
    __VA_ARGS__, \
    _POSTGRES_CXX_VISIT128, \
    _POSTGRES_CXX_VISIT127, \
    _POSTGRES_CXX_VISIT126, \
    _POSTGRES_CXX_VISIT125, \
    _POSTGRES_CXX_VISIT124, \
    _POSTGRES_CXX_VISIT123, \
    _POSTGRES_CXX_VISIT122, \
    _POSTGRES_CXX_VISIT121, \
    _POSTGRES_CXX_VISIT120, \
    _POSTGRES_CXX_VISIT119, \
    _POSTGRES_CXX_VISIT118, \
    _POSTGRES_CXX_VISIT117, \
    _POSTGRES_CXX_VISIT116, \
    _POSTGRES_CXX_VISIT115, \
    _POSTGRES_CXX_VISIT114, \
    _POSTGRES_CXX_VISIT113, \
    _POSTGRES_CXX_VISIT112, \
    _POSTGRES_CXX_VISIT111, \
    _POSTGRES_CXX_VISIT110, \
    _POSTGRES_CXX_VISIT109, \
    _POSTGRES_CXX_VISIT108, \
    _POSTGRES_CXX_VISIT107, \
    _POSTGRES_CXX_VISIT106, \
    _POSTGRES_CXX_VISIT105, \
    _POSTGRES_CXX_VISIT104, \
    _POSTGRES_CXX_VISIT103, \
    _POSTGRES_CXX_VISIT102, \
    _POSTGRES_CXX_VISIT101, \
    _POSTGRES_CXX_VISIT100, \
    _POSTGRES_CXX_VISIT99, \
    _POSTGRES_CXX_VISIT98, \
    _POSTGRES_CXX_VISIT97, \
    _POSTGRES_CXX_VISIT96, \
    _POSTGRES_CXX_VISIT95, \
    _POSTGRES_CXX_VISIT94, \
    _POSTGRES_CXX_VISIT93, \
    _POSTGRES_CXX_VISIT92, \
    _POSTGRES_CXX_VISIT91, \
    _POSTGRES_CXX_VISIT90, \
    _POSTGRES_CXX_VISIT89, \
    _POSTGRES_CXX_VISIT88, \
    _POSTGRES_CXX_VISIT87, \
    _POSTGRES_CXX_VISIT86, \
    _POSTGRES_CXX_VISIT85, \
    _POSTGRES_CXX_VISIT84, \
    _POSTGRES_CXX_VISIT83, \
    _POSTGRES_CXX_VISIT82, \
    _POSTGRES_CXX_VISIT81, \
    _POSTGRES_CXX_VISIT80, \
    _POSTGRES_CXX_VISIT79, \
    _POSTGRES_CXX_VISIT78, \
    _POSTGRES_CXX_VISIT77, \
    _POSTGRES_CXX_VISIT76, \
    _POSTGRES_CXX_VISIT75, \
    _POSTGRES_CXX_VISIT74, \
    _POSTGRES_CXX_VISIT73, \
    _POSTGRES_CXX_VISIT72, \
    _POSTGRES_CXX_VISIT71, \
    _POSTGRES_CXX_VISIT70, \
    _POSTGRES_CXX_VISIT69, \
    _POSTGRES_CXX_VISIT68, \
    _POSTGRES_CXX_VISIT67, \
    _POSTGRES_CXX_VISIT66, \
    _POSTGRES_CXX_VISIT65, \
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
    _POSTGRES_CXX_VISIT1)(accept, __VA_ARGS__)

#define POSTGRES_CXX_TABLE(name, ...) \
    static auto constexpr _POSTGRES_CXX_VISITABLE = true; \
    static auto constexpr _POSTGRES_CXX_TABLE_NAME = name; \
    template <typename V> \
    static void visitPostgresDefinition(V& visitor) { \
        _POSTGRES_CXX_VISIT(_POSTGRES_CXX_ACCEPT_DEF, __VA_ARGS__) \
    } \
    template <typename V> \
    void visitPostgresFields(V& visitor) const { \
        _POSTGRES_CXX_VISIT(_POSTGRES_CXX_ACCEPT_FLD, __VA_ARGS__) \
    } \
    template <typename V> \
    void visitPostgresFields(V& visitor) { \
        _POSTGRES_CXX_VISIT(_POSTGRES_CXX_ACCEPT_FLD, __VA_ARGS__) \
    }

