fn main() {
    let mut args = std::env::args();
    args.next();
    let n: u32 = match args.next() {
        Some(n) => n.parse().expect("Bad input"),
        None => panic!("Not enough arguments"),
    };

    if n < 1 {
        panic!("Too small number of fields");
    }

    println!("{}", PRAGMA);
    println!("\n{}", ACCEPT);
    println!("\n{}", VISIT1);
    let range = 1..(n + 1);
    for i in range.clone().skip(1) {
        println!("\
#define _POSTGRES_CXX_VISIT{}(accept, arg, ...) accept(arg) _POSTGRES_CXX_VISIT{}(accept, __VA_ARGS__)", i, i - 1);
    }
    println!("\n{}", OVERLOAD_HEAD);
    for i in range.clone() {
        println!("    _{}, \\", i);
    }
    println!("{}", OVERLOAD_TAIL);
    println!("\n{}", VISIT_HEAD);
    for i in range.clone().skip(1).rev() {
        println!("    _POSTGRES_CXX_VISIT{}, \\", i);
    }
    println!("{}", VISIT_TAIL);
    println!("\n{}\n", TABLE);
}

const PRAGMA: &str = "#pragma once";

const ACCEPT: &str = "\
#define _POSTGRES_CXX_ACCEPT_DEF(arg) visitor.template accept<decltype(arg)>(#arg);
#define _POSTGRES_CXX_ACCEPT_FLD(arg) visitor.accept(#arg, arg);";

const VISIT1: &str = "\
#define _POSTGRES_CXX_VISIT1(accept, arg)      accept(arg)";

const OVERLOAD_HEAD: &str = "#define _POSTGRES_CXX_OVERLOAD(\\";

const OVERLOAD_TAIL: &str = "    N, ...) N";

const VISIT_HEAD: &str = "\
#define _POSTGRES_CXX_VISIT(accept, ...) _POSTGRES_CXX_OVERLOAD( \\
    __VA_ARGS__, \\";

const VISIT_TAIL: &str = "    _POSTGRES_CXX_VISIT1)(accept, __VA_ARGS__)";

const TABLE: &str = "\
#define POSTGRES_CXX_TABLE(name, ...) \\
    static auto constexpr _POSTGRES_CXX_VISITABLE = true; \\
    static auto constexpr _POSTGRES_CXX_TABLE_NAME = name; \\
    template <typename V> \\
    static void visitPostgresDefinition(V& visitor) { \\
        _POSTGRES_CXX_VISIT(_POSTGRES_CXX_ACCEPT_DEF, __VA_ARGS__) \\
    } \\
    template <typename V> \\
    void visitPostgresFields(V& visitor) const { \\
        _POSTGRES_CXX_VISIT(_POSTGRES_CXX_ACCEPT_FLD, __VA_ARGS__) \\
    } \\
    template <typename V> \\
    void visitPostgresFields(V& visitor) { \\
        _POSTGRES_CXX_VISIT(_POSTGRES_CXX_ACCEPT_FLD, __VA_ARGS__) \\
    }";
