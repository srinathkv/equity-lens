# Advanced C++11–26 Examples: Concepts and Tradeoffs

This guide explains the design choices behind the six offline chapters invoked by `EquityLens.exe learn`. The demo uses a fixed stock-price sample; it does not call Alpha Vantage, open SQLite, or change production data. The [feature catalog](feature-catalog.md) maps the supplied standards matrix to the examples, and each standard's [lesson](README.md#lesson-order) provides additional background and exercises.

The chapters demonstrate the concepts selected for this project, not every overload or wording detail in ISO C++. Code examples that depend on compiler or standard-library support check feature-test macros and explain a fallback. A `/std:c++latest` build enables MSVC's latest working-draft mode; it does not mean every C++26 facility is implemented.

## Shared data model

[`LearningDemoSupport.h`](../../EquityLens/LearningDemoSupport.h) defines a small aggregate with an owned `std::string` symbol, open and close prices, and volume. The sample array has static lifetime, which makes it safe for chapter examples to create non-owning views and worker tasks referring to it. The fixed dataset also makes algorithm results repeatable. Real application data needs validation, persistence, provider error handling, and symbol/date identity; these examples intentionally keep that infrastructure out of the language lessons.

## C++11: explicit ownership and concurrency

See [`LearningDemoCxx11.h`](../../EquityLens/LearningDemoCxx11.h) and [C++11: Foundations](cpp11.md).

### Types, initialization, and algorithms

`auto` deduces an expression's type; it does not make the object dynamically typed. `decltype` can name an expression's type when a declaration needs to preserve it. Range-based `for` visits the fixed sample without manual iterator management. `nullptr` is a pointer-specific null value, unlike integer `0`, and `enum class` prevents direction labels such as `up` from leaking into the surrounding scope or converting implicitly to integers.

A typed lambda passed to `std::count_if` expresses the selection rule next to the algorithm. This avoids a hand-written loop when the operation is simply a predicate count. The same data can also be accumulated with a range-for when a mutable local total makes the computation clearer.

### RAII, moves, and smart-pointer roles

The `unique_ptr` examples give one object one owner. Moving the pointer transfers that responsibility; the source becomes empty and the destination destroys the object automatically. The custom-deleter example models a resource whose cleanup is not plain `delete`. In production, custom deleters are useful for C handles such as WinHTTP handles, file handles, or SQLite statements.

`shared_ptr` is appropriate only when multiple components truly share a lifetime. Copying it increments shared ownership; it does not make concurrent mutation of the pointed-to `PriceSample` safe. A `weak_ptr` can observe a shared object without extending its lifetime. Prefer `unique_ptr` by default because shared ownership introduces lifetime coupling and reference-count overhead.

### Worker completion and data synchronization

The worker thread calculates a volume total and up-day count. A `promise` publishes a result to its `future`; `future::get()` waits for completion and can rethrow a worker exception. The caller joins the thread before reading ordinary state written by the worker. The recursive `sumPack` template also demonstrates how C++11 expands parameter packs before fold expressions existed.

The example uses an atomic for one independent numeric value and a mutex plus `lock_guard` for a compound update. Relaxed atomic ordering is sufficient for the isolated counter because thread completion is separately synchronized by `future::get()` and `join()`; it would not publish unrelated memory. A mutex is the appropriate choice when several fields must change together while preserving an invariant. Avoid unsynchronized reads and writes to ordinary objects across threads.

A second `std::async` task demonstrates that a future can own completion for an asynchronous operation without the caller manually managing a thread. `std::chrono::milliseconds` represents a duration as a type instead of an unlabelled integer. The `constexpr` percentage helper, `static_assert`, type trait, and `noexcept` query show compile-time checks; none validate external input at runtime.

## C++14: generic programming and compile-time calculation

See [`LearningDemoCxx14.h`](../../EquityLens/LearningDemoCxx14.h) and [C++14: Generic programming](cpp14.md).

`std::make_unique` constructs an object directly into unique ownership, avoiding the gap between a raw `new` expression and smart-pointer construction. A generic lambda accepts different sample-like types without repeating an explicit parameter type. The init-capture `[total = 0.0]` creates state owned by the closure; `mutable` permits that captured state to change between calls.

`decltype(auto)` preserves the exact value category of the returned expression. The forwarding helper uses `std::forward<T>` so an lvalue argument remains an lvalue reference and an rvalue can remain an rvalue. This is powerful for generic wrappers, but returning a reference is safe only if the referenced object outlives the caller; forwarding a temporary and retaining the returned reference would dangle.

The chapter transforms the price array into daily changes with `std::transform` and a generic projection. C++14 relaxed `constexpr` permits local variables and loops, so the integer sum can be evaluated during translation and checked with `static_assert`. Variable templates provide a compile-time property such as `isArithmeticValue<T>` without wrapping it in a class template. `index_sequence` represents a pack of compile-time indices and is commonly used to expand tuple operations.

Digit separators and binary literals improve readability of constants. `std::exchange` returns the old capacity value while replacing it. Chrono literals such as `500ms` give a duration an explicit unit. These facilities reduce boilerplate, but the type and unit still need to match the real domain.

## C++17: vocabulary types and algorithm composition

See [`LearningDemoCxx17.h`](../../EquityLens/LearningDemoCxx17.h) and [C++17: Vocabulary types](cpp17.md).

Structured bindings name components of a pair, tuple, or aggregate. Class template argument deduction (CTAD) infers the element type of the example vector from its initializers. Inline variables permit a shared header definition without multiple-definition errors. The returned sample illustrates guaranteed copy elision for a same-type prvalue in the applicable initialization context.

`optional<double>` represents a value that might be absent; `value_or` chooses a fallback without inventing a sentinel price. `variant<PriceSample, string>` represents one of a closed set of alternatives. `visit` handles each alternative with `if constexpr`, making the visitor exhaustive at compile time. `any` instead stores a value of an arbitrary type and requires a matching `any_cast`; a mismatched cast can throw `bad_any_cast`.

`string_view` is a non-owning view of characters. It is safe here because the source string in the static sample array outlives the view. Do not return or store a view into a temporary or a string that may reallocate. `filesystem::path` composes and normalizes path values; lexical path composition does not itself create a file or prove that a path exists.

`from_chars` parses from a bounded character range without locale-dependent stream formatting. Its returned error code and end pointer must both be checked to reject malformed or trailing text. `byte` represents raw byte-like data and does not implicitly behave like an integer; `to_integer` makes a numeric interpretation explicit. Fold expressions apply an operation across a parameter pack.

The chapter uses `count_if` and `transform_reduce` with `std::execution::par`. An execution policy allows the implementation to use parallel execution but does not promise a particular number of threads or that a small range will run faster. Parallel reductions should use operations whose result is valid under regrouping; integer volume addition is a better deterministic teaching example than floating-point addition. Do not introduce unsynchronized side effects into a parallel algorithm callback.

## C++20: constraints, views, representation, and stoppable work

See [`LearningDemoCxx20.h`](../../EquityLens/LearningDemoCxx20.h) and [C++20: Constraints and ranges](cpp20.md).

### Concepts and ranges

The `constrainedMean` template accepts an input range whose values can convert to `double`. A concept/requires clause rejects unsuitable types during overload resolution with a compile-time diagnostic; it does not validate untrusted numeric values. The pipeline filters up days and transforms each sample into a gain. Views are lazy: composing them describes work, while iteration in the mean calculation performs it.

`ranges::sort` sorts a copy of the samples using a projection to `PriceSample::close`. A projection avoids writing a comparator that repeats the key-selection logic. The original sample data stays unchanged. `span` and `subspan` provide bounded, non-owning views into contiguous data; they do not own or extend the lifetime of the array.

### Initialization and compile-time choices

The designated initializer makes the `PriceSample` fields explicit and must follow declaration order. The defaulted spaceship operator synthesizes comparisons from its members. Because `double` can be unordered for NaN, the generated ordering is not a total order for all possible values; financial data should validate finite prices before ordering.

`bit_cast` copies an object's representation between equal-sized trivially copyable types. It is not a numeric conversion: the displayed integer is the IEEE floating-point bit pattern, and representation assumptions should be stated when moving across platforms. `consteval` requires a call to be evaluated at compile time; `constinit` requires static initialization but does not make the variable immutable. The chrono calendar object checks whether a date is valid.

### Cancellation and time zones

The `jthread` worker publishes a start signal, then cooperatively checks its `stop_token`. The caller requests stop and joins; cancellation is a request that the worker must observe, not forced thread termination. `jthread` joins automatically at destruction, which improves ownership safety over a bare joinable thread. The atomic is used for the count, while the promise/future establishes the startup handshake.

The UTC lookup is guarded and caught because a standard-library chrono API does not guarantee that a usable time-zone database is installed or loadable on every target. Calendar arithmetic and time-zone database support are related but distinct concerns.

Modules require module interface units and build-system configuration; adding the keyword to an ordinary `.cpp` does not create a modular build. Coroutines are language syntax and compiler machinery for suspend/resume control flow, not a ready-made HTTP client, scheduler, or task type. The C++23 generator later in the guide provides a standard-library coroutine abstraction where available.

## C++23: explicit errors, multidimensional views, and lazy pipelines

See [`LearningDemoCxx23.h`](../../EquityLens/LearningDemoCxx23.h) and [C++23: Library expansion](cpp23.md).

`expected<double, string>` carries either a parsed close or an error. The parser checks the `from_chars` status, full input consumption, and finiteness before returning a value. This keeps expected bad input in the result type instead of using exceptions for ordinary parse failure. The monadic `transform` example projects a successful result while propagating an error unchanged; the example is guarded by `__cpp_lib_expected >= 202211L` because monadic operations can lag behind basic `expected` support.

`ranges::fold_left` combines transformed daily changes. `byteswap` reverses byte order; applying it twice demonstrates reversibility, not a general conversion between host and network encodings. Endianness and serialized field sizes still matter in an actual file or wire format.

`if consteval` selects a branch based on whether the call is being evaluated at compile time. The separate compile-time and runtime values make the two paths observable. Deducing `this` makes the explicit object parameter available to a member function, which can simplify overload families and fluent APIs. The multidimensional subscript operator accepts row and column directly; bounds checking remains the program's responsibility.

`mdspan` is a non-owning multidimensional view over the dense array. Extents and layout describe how indices map to the backing storage; `mdspan` neither allocates nor owns that storage. Keep the array alive for every access. `std::generator` lazily yields values from a coroutine, and `fold_left` consumes that input range without first building a second vector. The source sample has static lifetime, so the generator's references remain valid during iteration.

When `__cpp_lib_move_only_function` is available, the C++23 chapter type-erases a callable that owns a `std::unique_ptr`. The wrapper can be moved but not copied, matching the resource semantics of its target; its captured output stream remains a reference and must outlive invocation.

`flat_map` offers an ordered associative interface backed by flat storage; insertion and iterator invalidation characteristics differ from node-based maps. `std::print` supplies format-checked output, but application output should still avoid mixing unrelated streams when redirecting output. `mdspan`, `generator`, `flat_map`, and `print` have independent feature-test macros; one being present does not imply the others are available.

## C++26: bounded storage and evolving facilities

See [`LearningDemoCxx26.h`](../../EquityLens/LearningDemoCxx26.h) and [C++26: New facilities and toolchain support](cpp26.md).

When `__cpp_lib_inplace_vector >= 202406L` is advertised, `inplace_vector<PriceSample, 3>` stores a runtime-sized batch with a compile-time maximum capacity and inline element storage. This can avoid a separate allocation for the elements and makes the bound explicit. It is not a drop-in replacement for `vector`: the capacity cannot grow, a full container cannot accept another element, and reserving a large maximum increases the containing object's size. The example aggregates volume over the bounded batch.

The zero-initialized array illustrates the safe practice of initializing storage before reading it. C++26's erroneous/indeterminate-value rules do not mean default-initialized scalar objects are automatically zero. Never demonstrate an invalid read to explain a safety rule.

Reflection, contracts, pack indexing, structured-binding packs, sender/receiver execution, `hive`, and expanded range/container facilities are support-sensitive or compile-time/design topics. Sender/receiver execution is distinct from C++17 parallel algorithm policies: it composes asynchronous operations and completion channels, but does not provide an HTTP client or event loop. Consult the compiler and standard-library feature status before using any of these facilities in production. C++26 erroneous/indeterminate-value rules do not automatically initialize scalar objects or make arbitrary uninitialized reads safe.

## How to use this guide

1. Run `EquityLens.exe learn` and follow the matching source header for each chapter.
2. Read the linked per-standard lesson for the language/library background and exercises.
3. Check feature-test macros before interpreting a skipped example as a compiler error; the fallback may simply mean that the standard library has not implemented that facility.
4. For non-owning views, identify the exact object that owns the backing storage and prove it outlives every access.
5. For concurrency examples, identify which operation publishes data, which operation waits, and whether an atomic or mutex protects the invariant.
6. Add an example only when it improves understanding; for production code, prefer a simpler supported facility when advanced syntax does not improve correctness or design.
