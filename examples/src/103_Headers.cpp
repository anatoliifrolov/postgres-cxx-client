/// ### What to include
///
/// The library provides all-in-one header file ```#include <postgres/Postgres.h>```,
/// one with forward declarations ```#include <postgres/Fwd.h>```,
/// and also each class that is a part of a public API has its own header.
/// It is strongly discouraged to declare any of the library types in your project code.
/// So include ```Fwd.h``` when you just need a declaration, say in a function signature.
/// If compilation time is not a big concern use ```Postgres.h```.
/// Otherwise include only needed files from ```postgres``` directory.
