# Modern C++ Interview Questions and Answers

These questions focus on reasoning about ownership, lifetime, errors, generic code, and concurrency rather than recalling syntax in isolation. Answers use the standard version where it matters; check compiler and library support before relying on newer facilities.

## Fundamentals and lifetime

### 1. What is RAII, and why is it useful?

**Answer:** Resource Acquisition Is Initialization ties a resource's lifetime to an object's lifetime. The object's destructor releases the resource on normal scope exit and during exception unwinding, which makes cleanup local and reliable. `std::unique_ptr`, lock guards, and file streams are common examples.

### 2. What is the difference between a pointer and a reference?

**Answer:** A pointer is an object that can be null, reassigned, and inspected before dereferencing. A reference is an alias that must be initialized to refer to an object and cannot be reseated. Neither extends the lifetime of the referred-to object.

### 3. When should you use `unique_ptr`, `shared_ptr`, or `weak_ptr`?

**Answer:** Use `unique_ptr` for one clear owner and transfer ownership with moves. Use `shared_ptr` only when independent components genuinely share lifetime responsibility. Use `weak_ptr` to observe a shared object without keeping it alive, commonly to break ownership cycles.

### 4. What does `std::move` do?

**Answer:** `std::move` casts its argument to an rvalue expression; it does not move data itself. A selected move-aware constructor or assignment operator may then transfer resources. The source remains valid but its value is type-specific and should not be assumed unchanged.

### 5. Why might a polymorphic base need a virtual destructor?

**Answer:** If a derived object can be destroyed through a base pointer, the base destructor must be virtual so the derived destructor runs. Otherwise deleting through that base pointer has undefined behavior. An interface often declares `virtual ~Base() = default;`.

### 6. What do the Rule of Zero, Three, and Five mean?

**Answer:** Prefer the Rule of Zero: compose members that manage their own resources and define no special member functions. If a class directly manages a resource, the Rule of Three highlights destructor, copy constructor, and copy assignment; C++11's Rule of Five also considers move construction and move assignment.

### 7. When can `std::vector` invalidate references or iterators?

**Answer:** Reallocation invalidates all iterators, pointers, and references to its elements. Insertions and erasures without reallocation can still invalidate positions at or after the changed point. Check the operation's invalidation guarantees before retaining aliases.

### 8. What is the lifetime risk of `std::string_view`?

**Answer:** `string_view` stores a non-owning pointer and length, not characters. Its source must remain alive and unmodified in ways that invalidate the view for every use; returning a view into a local string creates a dangling view.

## Templates and overloads

### 9. What is a forwarding reference?

**Answer:** In a deduced context such as `template<class T> void f(T&&)`, `T&&` is a forwarding reference. Reference collapsing preserves whether the caller supplied an lvalue or rvalue; use `std::forward<T>(value)` to preserve that category when forwarding. `T&&` with an explicitly fixed `T` is an ordinary rvalue reference.

### 10. How does overload resolution choose between a non-template and a function template?

**Answer:** The compiler first forms viable candidates and compares their conversion sequences. The best conversion sequence wins; when conversion ranks tie, a non-template is generally preferred over a function-template specialization. If equally good function-template candidates remain, partial ordering can select the more specialized parameter pattern.

### 11. Can a function template be partially specialized?

**Answer:** No. Class templates can be partially specialized, but function templates cannot. Define an overload with the desired parameter pattern instead; overload resolution and template partial ordering choose among those candidates.

### 12. What problem do concepts solve compared with unconstrained templates?

**Answer:** Concepts state requirements at the template boundary, improve diagnostics, and participate in overload ordering. They do not validate untrusted runtime values or prove that an algorithm's mathematical assumptions hold.

### 13. What is the difference between `constexpr`, `consteval`, and `constinit`?

**Answer:** `constexpr` permits constant evaluation when the context and inputs allow it. `consteval` requires an immediate compile-time invocation. `constinit` requires static initialization for a variable but does not make that variable `const`.

## Errors and library design

### 14. What are the basic, strong, and no-throw exception guarantees?

**Answer:** The basic guarantee preserves invariants and avoids leaks, but state may change. The strong guarantee leaves observable state unchanged if the operation fails. The no-throw guarantee promises that no exception escapes. Build updates in a temporary and commit with a non-throwing swap when practical.

### 15. When would you choose `optional`, `variant`, or `expected`?

**Answer:** `optional<T>` represents a value that may be absent. `variant<A, B, ...>` represents one value from a closed set of alternatives. `expected<T, E>` represents either a result or an error value, making failure details part of the return contract (standardized in C++23).

### 16. What is the difference between `std::map` and `std::unordered_map`?

**Answer:** `map` maintains key order and typically provides logarithmic lookup using a comparison relation. `unordered_map` uses hashing, has no iteration-order guarantee, and offers average constant-time lookup with linear worst cases. Choose based on ordering, key behavior, and workload rather than assuming one is universally faster.

### 17. What does a polymorphic memory resource change?

**Answer:** `std::pmr` lets allocator-aware objects select a runtime `memory_resource` while retaining the standard allocator-aware container interface. The container does not own its resource, so the resource must outlive the container and its allocations. A monotonic resource is useful for phase-scoped allocations, not a universal replacement for the default allocator.

### 18. Are ranges views owning containers?

**Answer:** Usually not. A view describes access or lazy work over a source range; it may borrow that source, so iteration must not outlive the source. Check the specific view's ownership properties instead of inferring ownership from its type name.

## Concurrency and newer facilities

### 19. What is a data race, and why is `volatile` not a fix?

**Answer:** Conflicting unsynchronized accesses to the same memory location, with at least one write, form a data race and cause undefined behavior. `volatile` affects certain observable accesses but does not provide atomicity or inter-thread synchronization; use a mutex or appropriate atomic operations.

### 20. How does release/acquire publish ordinary data?

**Answer:** A writer stores the payload and then performs a release store to an atomic flag. A reader that observes that store with an acquire load synchronizes with the release; preceding writes become visible to the reader. The payload itself need not be atomic if all accesses follow this ordering and no other data race exists.

### 21. What does `std::jthread` add over `std::thread`?

**Answer:** `jthread` requests cooperative stop and joins automatically during destruction. A stop request does not forcibly terminate work: the function must observe its `stop_token` and respond. The destructor can still wait indefinitely if the function ignores the request.

### 22. Do C++ coroutines automatically make work asynchronous?

**Answer:** No. Coroutines provide suspend/resume machinery. Asynchronous behavior requires an awaitable operation and an execution mechanism or scheduler; a synchronous generator can use coroutines without performing asynchronous I/O.

### 23. What do C++ modules change compared with headers?

**Answer:** Modules provide named interfaces and compiled module units, changing how declarations are exposed and how dependencies are built. They require build-system support for compiling interfaces before importers. They are not enabled merely by changing an ordinary source file's extension or language mode.

### 24. How do you check whether a recent library feature is available?

**Answer:** Check the selected compiler and standard-library documentation and, where provided, the facility's feature-test macro. A language-mode flag such as `/std:c++latest` does not guarantee that every library facility is implemented. Keep a behaviorally honest fallback or report that the example is skipped.

### 25. What should an answer to a C++ design question include besides the feature name?

**Answer:** State who owns each object, how long borrowed views remain valid, what happens on failure, which synchronization establishes visibility, and what tradeoff justifies the abstraction. A simpler correct design is often preferable to a newer facility without a concrete need.

## Object model and class design

### 26. What is the difference between a declaration and a definition?

**Answer:** A declaration introduces a name and its type; a definition provides the entity itself, such as a function body or object's storage. Some declarations are also definitions, while a program usually needs exactly one definition for each odr-used non-inline entity.

### 27. What is the One Definition Rule?

**Answer:** The ODR constrains how many definitions an entity may have and requires equivalent definitions where multiple translation units contain permitted definitions. Violations can be diagnosed or can make the program ill-formed with no diagnostic required.

### 28. What is linkage?

**Answer:** Linkage determines whether declarations in different scopes or translation units can denote the same entity. Namespaces, `static`, `extern`, and module attachment affect linkage; linkage is distinct from an object's storage duration.

### 29. Why can global initialization order be dangerous?

**Answer:** Dynamic initialization across translation units does not generally provide a useful global ordering. A global object that depends on another translation unit's dynamically initialized object can observe it before initialization; prefer function-local statics or explicit initialization dependencies.

### 30. What is a delegating constructor?

**Answer:** A constructor whose member-initializer list names another constructor of the same class. It centralizes initialization logic, and the target constructor completes before the delegating constructor body runs.

### 31. What does `explicit` prevent?

**Answer:** It prevents certain implicit conversions through a constructor or conversion function. Use it when conversion should require deliberate syntax; C++20 also permits conditional explicitness for templates.

### 32. What does `const` on a member function mean?

**Answer:** It makes the implicit object parameter refer to a const-qualified object and prevents modifying ordinary non-mutable data members through `this`. It is a compile-time interface promise, not thread synchronization.

### 33. What is a static data member?

**Answer:** It belongs to the class rather than to each object, so all instances refer to the same member. Since C++17, an `inline static` data member can be defined in a header without a separate out-of-class definition.

### 34. What do `override` and `final` communicate?

**Answer:** `override` asks the compiler to verify that a member overrides a virtual base member. `final` prevents a virtual function from being overridden further or a class from being derived from.

### 35. When does virtual dispatch occur?

**Answer:** A virtual call through a base reference or pointer dispatches to the final overrider for the object's dynamic type. Calls made from a constructor or destructor dispatch only within the class currently being constructed or destroyed, not to a not-yet-constructed or already-destroyed derived part.

### 36. What is object slicing?

**Answer:** Copying a derived object into a base object by value copies only the base subobject, discarding the derived state and dynamic behavior. Pass or own polymorphic objects through references or pointers when preserving dynamic type matters.

### 37. When is `dynamic_cast` appropriate?

**Answer:** It performs checked runtime conversion within a polymorphic hierarchy. Use it when a genuine runtime type query is part of the design; repeated downcasts can indicate that behavior belongs on a virtual interface or should use a closed-set type such as `variant`.

### 38. What is multiple inheritance?

**Answer:** A class can have more than one direct base class, combining interfaces or implementation subobjects. It can create ambiguity and layout complexity, so interface-focused use is often easier to reason about.

### 39. What problem does virtual inheritance solve?

**Answer:** In a diamond hierarchy, virtual inheritance allows multiple paths to share one common base subobject. The most-derived class initializes that virtual base, which adds construction and layout complexity and should be used only when shared identity is required.

### 40. How do `public`, `protected`, and `private` inheritance differ?

**Answer:** They change how the base class's public and protected members are exposed through the derived class. Public inheritance models substitutability; protected or private inheritance is an implementation relationship and is often better expressed with composition.

### 41. What is an aggregate, and what do designated initializers require?

**Answer:** An aggregate is a type eligible for aggregate initialization under standard rules, without requiring a user-provided constructor. C++20 designated initializers name aggregate members in declaration order; they are not general named-parameter syntax.

### 42. What is the difference between a class invariant and a precondition?

**Answer:** An invariant must hold for every externally observable valid object state. A precondition is a requirement on a particular operation's input or starting state; constructors and mutating operations should preserve invariants.

### 43. What does placement `new` do?

**Answer:** A placement-new expression constructs an object in supplied storage; it does not allocate that storage. The caller must satisfy alignment and size requirements and explicitly manage the object's lifetime and destruction.

### 44. Why does alignment matter?

**Answer:** Objects must be created at addresses meeting their alignment requirement. Misaligned storage can make object creation invalid or access undefined; use aligned allocation facilities rather than assuming byte buffers suit every type.

### 45. What are the risks of using a union?

**Answer:** A union stores overlapping members and normally has only one active member at a time. The program must track and manage the active member's lifetime; `std::variant` is usually safer when the alternatives form a known set.

### 46. Why prefer `enum class` over an unscoped enum?

**Answer:** A scoped enum keeps enumerators inside the type's scope and does not implicitly convert to integers. This reduces accidental name collisions and unintended arithmetic; convert explicitly when an integer representation is needed.

### 47. What is `std::byte` for?

**Answer:** `std::byte` represents raw object-representation storage without pretending to be a character or arithmetic integer. Use explicit conversions such as `std::to_integer` when interpreting it numerically.

### 48. Is `using` only an alias for `typedef`?

**Answer:** Both can introduce type aliases, but alias declarations with `using` are easier to read for templates. `using` also introduces namespace names or enumerators in other contexts, so its role depends on syntax.

### 49. What does `constexpr` on a constructor allow?

**Answer:** It allows suitable objects to be initialized during constant evaluation when their members and initialization satisfy the language rules. It does not force every constructed object to be a compile-time constant.

### 50. What is the difference between an object and a reference to it?

**Answer:** An object occupies storage and has a lifetime; a reference is an alias bound to an object and does not own separate storage for a referred-to value. A reference can dangle when its target's lifetime ends.

## Templates and overload selection

### 51. How does by-value template argument deduction treat top-level `const`?

**Answer:** For a parameter `T value`, top-level cv-qualification on the argument is generally dropped during deduction because the function receives a copy. A `const T&` parameter instead preserves relevant qualification in the deduced type.

### 52. What is a dependent name?

**Answer:** A name whose meaning depends on a template parameter. The compiler may need `typename` to identify a dependent qualified name as a type, or the `template` disambiguator before a dependent member template name.

### 53. What are default template arguments?

**Answer:** They supply template parameters when callers omit them, similar to default function arguments. Defaults can make APIs convenient, but must respect parameter ordering and avoid conflicting redefinitions across declarations.

### 54. What is a non-type template parameter?

**Answer:** It is a template parameter whose value, rather than type, is supplied as an argument, such as an array bound or compile-time integer. Newer standards allow additional structural types; admissible types depend on the language version.

### 55. What is explicit template specialization?

**Answer:** It provides a specific implementation for a particular set of template arguments. Specializations must be declared consistently wherever required before uses that would otherwise instantiate the primary template.

### 56. What is partial specialization?

**Answer:** It provides an implementation for a subset or pattern of class-template arguments, such as pointer types. Class templates support partial specialization; function templates use overloads instead.

### 57. What is SFINAE?

**Answer:** Substitution Failure Is Not An Error removes certain function-template candidates when substitution into their immediate context fails. It historically enables conditional overloads, though concepts generally express such requirements more clearly in modern C++.

### 58. What is `std::enable_if` used for?

**Answer:** It conditionally makes a template declaration or overload viable based on a compile-time boolean. It is a common SFINAE tool in older code; prefer a `requires` clause or constrained template in C++20 when appropriate.

### 59. What is a requires-expression?

**Answer:** It checks whether a set of expressions, types, or nested requirements is valid for template arguments. It can define a concept or constrain an overload without evaluating those expressions at runtime.

### 60. What is concept subsumption?

**Answer:** It is part of C++20 constraint ordering: a more constrained overload can be preferred when its normalized constraints subsume another candidate's constraints. Constraint equivalence is based on language rules, not arbitrary logical theorem proving.

### 61. What is class template argument deduction?

**Answer:** CTAD deduces class-template arguments from a variable's initializer and constructor deduction guides. It reduces repetition but does not always deduce the type a programmer intends; explicit arguments remain useful for clarity.

### 62. What is a deduction guide?

**Answer:** A deduction guide tells CTAD which class-template specialization to form from constructor arguments. It affects deduction only; it is not itself a constructor and does not change object initialization behavior.

### 63. What is a parameter pack?

**Answer:** A parameter pack represents zero or more template, function, or `auto` parameters. Pack expansion applies a pattern to each element, and the expansion context determines how the resulting sequence is formed.

### 64. What is a fold expression?

**Answer:** A fold expression applies a binary operator over a parameter pack. Unary folds over an empty pack are only valid for certain operators with defined identities; otherwise provide an initial value with a binary fold.

### 65. What is `if constexpr`?

**Answer:** It discards the non-selected branch during template instantiation when the condition is a constant expression. It supports type-dependent implementation paths, but does not replace overload constraints when a candidate should be excluded entirely.

### 66. What is explicit template instantiation?

**Answer:** It directs the compiler to instantiate a template specialization at a chosen point, often to control code generation or reduce repeated compilation. It does not eliminate the need for the template definition to be available where implicit instantiation occurs.

### 67. How does argument-dependent lookup work?

**Answer:** ADL adds functions from namespaces associated with the argument types to certain unqualified function calls. It enables non-member operators and customization functions to be found beside their types; it can also introduce surprising candidates if namespaces are broad.

### 68. What is the difference between a template declaration and an instantiation?

**Answer:** A template declaration describes a family of entities; an instantiation forms a concrete specialization for particular arguments. Errors in dependent code may appear only when an instantiation requires the invalid construct.

### 69. What does `std::invoke` unify?

**Answer:** It provides a consistent invocation mechanism for function objects, function pointers, member-function pointers, and member-data pointers. Generic code can use it instead of implementing separate call syntax for each callable category.

### 70. When should a function be overloaded instead of templated?

**Answer:** Use overloads when the supported cases are a small, explicit set or need distinct semantics. Use a template when one implementation meaningfully applies across a family of types; constrain it to express its real requirements.

### 71. What is a trailing return type useful for?

**Answer:** It allows a function's return type to refer to parameter names, as in `auto f(T x) -> decltype(x + x)`. It remains useful in templates even though C++14 added ordinary `auto` return-type deduction.

### 72. What does `decltype` inspect?

**Answer:** `decltype(expression)` yields a type according to rules that preserve value category for certain expressions; an unparenthesized id-expression yields the declared type of the named entity. Parentheses can therefore change a deduced reference result.

### 73. What is the difference between `decltype(auto)` and `auto` return deduction?

**Answer:** `auto` deduces a value type with top-level cv/reference generally removed. `decltype(auto)` applies `decltype` rules to the returned expression and can preserve references, so it can accidentally return a dangling reference if used carelessly.

### 74. What is a fold expression's identity value?

**Answer:** It is the value that leaves a reduction unchanged, such as `0` for addition or `1` for multiplication. A binary fold makes that initial value explicit and also handles an empty parameter pack when the operation is defined.

### 75. Why can unconstrained templates produce poor diagnostics?

**Answer:** The compiler may instantiate a long implementation path before discovering that an operation is unsupported. Concepts or carefully designed overloads state requirements at the interface and reject unsuitable arguments closer to the call site.

## Exceptions and standard-library design

### 76. What happens to local objects when an exception propagates?

**Answer:** Stack unwinding destroys fully constructed automatic objects in reverse order of completed construction. This is why RAII cleanup works on exceptional paths; objects not yet fully constructed do not have their class destructor called.

### 77. Why should exceptions usually be caught by reference?

**Answer:** Catching a polymorphic exception by `const` reference avoids slicing and an unnecessary copy while allowing virtual behavior. Catch by value only when an intentional local copy is needed and slicing is not a concern.

### 78. Why should catch handlers usually appear from specific to general?

**Answer:** A handler for a derived exception must precede a handler for its base, or the base handler catches it first and makes the derived handler unreachable. Catch `std::exception` at a boundary after more specific recovery handlers.

### 79. What does `noexcept` promise?

**Answer:** A non-throwing exception specification promises that no exception escapes the function. If one does, `std::terminate` is called; `noexcept` should reflect a real guarantee, not be used to suppress errors.

### 80. What does the `noexcept(expression)` operator do?

**Answer:** It is a compile-time query that reports whether an expression is potentially throwing under its exception specifications. It does not execute the expression.

### 81. Why are destructors expected not to throw?

**Answer:** A destructor invoked during stack unwinding must not let another exception escape, because that calls `std::terminate`. Standard destructors are generally non-throwing unless their members or bases change that property; cleanup failures need an explicit reporting strategy.

### 82. What is `std::terminate`?

**Answer:** It ends the program when the runtime cannot safely continue, including an exception escaping a non-throwing function or a second exception escaping during unwinding. It is not a recoverable error path.

### 83. How does copy-and-swap provide the strong exception guarantee?

**Answer:** Build a complete candidate state first; if construction fails, the original is untouched. Then commit with a non-throwing swap. The technique depends on the swap and candidate construction having the required behavior.

### 84. When is `std::bad_alloc` thrown?

**Answer:** A throwing allocation request generally reports failure by throwing `std::bad_alloc` when storage cannot be obtained. Non-throwing allocation forms can instead return null; container operations may also fail for other reasons such as length limits.

### 85. When might error codes be preferable to exceptions?

**Answer:** Error codes can make expected, frequent failures explicit in a low-level interface or where exception use is disallowed. Exceptions can simplify propagation when callers at a higher boundary can recover or report; choose a consistent contract for the API.

### 86. What does `optional::value()` do when no value is present?

**Answer:** It throws `std::bad_optional_access`. By contrast, dereferencing an empty `optional` does not provide that checked behavior; establish engagement before using `*` or `->`.

### 87. What is `std::variant`'s valueless-by-exception state?

**Answer:** A variant can become valueless if an operation that changes its active alternative throws after destroying the old value but before constructing the new one. Check `valueless_by_exception()` when the surrounding operation can expose that possibility.

### 88. When do you choose `vector` over `list`?

**Answer:** `vector` is the default for most sequences because contiguous storage improves locality and supports random access. `list` is useful only when its stable iterators and splice behavior justify per-node allocation and lack of efficient random access.

### 89. What is the difference between `map` and `unordered_map` key requirements?

**Answer:** `map` uses a strict weak ordering to organize keys; `unordered_map` requires hashing and equality consistent with one another. Neither container can safely use a key whose comparison or hash behavior changes while it is stored.

### 90. What happens when unordered-container keys collide?

**Answer:** Different keys with the same hash are resolved using equality checks within the bucket; collisions are permitted. Poor hash distribution can degrade performance toward linear lookup without changing correctness.

### 91. What is an iterator category?

**Answer:** It describes the operations and guarantees an iterator supports, such as single-pass input traversal, forward multi-pass traversal, bidirectional movement, random access, or contiguous access. Algorithms state the category or concept they require.

### 92. What must a sorting comparator satisfy?

**Answer:** It must induce a strict weak ordering: it must be irreflexive and transitive, with consistent equivalence classes. A comparator that violates these requirements can make sorting behavior undefined.

### 93. When is `stable_sort` useful?

**Answer:** It preserves the relative order of elements considered equivalent by the comparator. Use it when earlier ordering among equivalent values carries meaning; otherwise `sort` may be sufficient and can use less auxiliary storage.

### 94. What precondition does `lower_bound` require?

**Answer:** The searched range must be partitioned according to the comparison used by `lower_bound`; a fully sorted range is a common way to satisfy this. Applying it to an unpartitioned range does not provide a valid search result.

### 95. Why does the erase-remove idiom exist?

**Answer:** `std::remove` compacts retained values and returns a new logical end but does not change a sequence container's size. Erase the tail afterward, or use `std::erase`/`std::erase_if` where available.

### 96. What does `vector::reserve` guarantee?

**Answer:** It requests capacity for at least the specified number of elements without changing the vector's size. If it reallocates, existing iterators, pointers, and references are invalidated; it does not guarantee an exact capacity.

### 97. Is `emplace_back` always better than `push_back`?

**Answer:** No. `emplace_back` constructs from forwarded arguments, which can avoid spelling a temporary, but may make overload selection less clear or introduce unexpected constructor conversions. Prefer the form that communicates intent most clearly.

### 98. Why is `std::vector<bool>` unusual?

**Answer:** It is a space-optimized specialization that may pack bits and return proxy references rather than actual `bool&` values. Do not assume its element references or data access behave like those of an ordinary `vector<T>`.

### 99. What does `std::span` own?

**Answer:** Nothing; it is a non-owning view over contiguous elements. Its pointer and size do not keep the storage alive, and a span becomes invalid when its backing storage expires or is invalidated.

### 100. When should you use `std::array` instead of a built-in array?

**Answer:** `std::array<T, N>` owns a fixed-size sequence while participating in standard container interfaces and algorithms. Built-in arrays have different decay and parameter behavior; use them when required by an interface or low-level representation.

### 101. When is a named struct clearer than a tuple?

**Answer:** A struct gives fields semantic names and a place to document invariants. A tuple is useful for short, local product types or algorithm results; long-lived domain data is usually clearer with a named type.

### 102. What is `std::initializer_list`'s important storage property?

**Answer:** It refers to a compiler-created array of `const` elements whose lifetime is extended only in specified initialization contexts. It cannot move from its elements and is not a general replacement for a forwarding parameter pack.

### 103. What happens to a vector's elements when it reallocates?

**Answer:** It creates elements in new storage, typically by moving when that is safe or copying when required, then destroys the old elements. This is why move exception guarantees affect vector operations and why all element aliases are invalidated.

### 104. What is an allocator's upstream resource?

**Answer:** A memory resource may request more storage from an upstream resource when its own strategy cannot satisfy an allocation. `std::pmr::null_memory_resource()` throws instead, making exhaustion explicit rather than falling back to the global heap.

### 105. What does `monotonic_buffer_resource::release()` do?

**Answer:** It releases the resource's allocated blocks in bulk and resets its allocation state; it does not call destructors for objects placed in containers using that resource. Destroy those objects and their containers before releasing storage they rely on.

### 106. What is a `shared_ptr` control block?

**Answer:** It stores shared ownership bookkeeping, including the reference counts and usually the deleter. Copies share the control block; the managed object is destroyed when the last strong owner releases it.

### 107. What does `weak_ptr::lock()` do?

**Answer:** It atomically attempts to obtain a `shared_ptr` if the object is still alive, returning an empty pointer otherwise. This avoids a check-then-use race that would occur if code inspected expiration and acquired ownership separately.

### 108. Why can custom deleters change a `unique_ptr`'s type?

**Answer:** The deleter is part of the `unique_ptr` specialization and can affect its size and move behavior. A function-pointer deleter stores a pointer; an empty stateless deleter may be optimized away.

### 109. Is small-string optimization guaranteed?

**Answer:** No. Implementations commonly store short strings inside the string object, but the standard does not require that optimization or specify its capacity. Do not make correctness or ABI assumptions based on it.

### 110. Why is `std::from_chars` useful for parsing?

**Answer:** It is locale-independent and reports both an error code and the first unparsed character without allocating or throwing for parse errors. Callers must check the error and require complete input consumption when trailing characters are invalid.

### 111. Why prefer `std::filesystem::path` to concatenated path strings?

**Answer:** `path` models filesystem paths and provides operations such as joining components with platform-specific separators. It does not guarantee that a path exists or is safe; filesystem operations still need error handling and security checks.

### 112. Does `std::format` use the current C locale?

**Answer:** Its default formatting is locale-independent. Locale-aware overloads take a locale explicitly; use them when localized presentation is intended rather than relying on process-global formatting state.

### 113. Why use `std::chrono::duration` instead of an integer count?

**Answer:** A duration carries its unit in the type, making many conversions explicit and reducing unit-mismatch errors. Conversions between periods may truncate or require explicit casts depending on the conversion.

### 114. Is a `sys_time` timestamp the same as a local calendar date?

**Answer:** No. `sys_time` represents a point on the system-clock timeline, while a local date depends on a time zone and calendar rules. Convert using an explicit zone and account for ambiguous or nonexistent local times.

### 115. What is `std::numbers` for?

**Answer:** It provides typed mathematical constants such as `std::numbers::pi_v<T>` without embedding approximate literals throughout code. These constants do not make floating-point calculations exact.

## Containers, algorithms, and ranges

### 116. What is the difference between an input range and a forward range?

**Answer:** An input range supports single-pass reading; iterating it can consume shared state. A forward range is multi-pass and provides stronger iterator guarantees, allowing independent traversals.

### 117. What is a range sentinel?

**Answer:** A sentinel is an end marker comparable with an iterator but need not have the iterator's type. This allows ranges whose end cannot be represented as an iterator with the same operations, such as some stream ranges.

### 118. What is a borrowed range?

**Answer:** A borrowed range permits iterators obtained from the range object to remain valid after that range object itself is destroyed. It does not guarantee the underlying elements outlive their owner; the source storage must still remain valid.

### 119. What does a projection do in a ranges algorithm?

**Answer:** A projection transforms each element for the algorithm's comparison or operation without requiring a separate transformed container. For example, sorting records by a member can use a pointer-to-member projection.

### 120. When is `filter_view`'s predicate evaluated?

**Answer:** A filter view is lazy: its predicate is evaluated as the view is traversed, not when the view is created. Repeated traversal can repeat predicate work, and the source must remain valid while iterating.

### 121. What is the difference between `views::filter` and `views::transform`?

**Answer:** `filter` selects which source elements are visited; `transform` maps each visited element to a result. Composing them describes a lazy pipeline and generally creates no intermediate container.

### 122. What is `views::iota`?

**Answer:** It creates a range of incrementing values, often lazily, from a starting value to a bound or as an unbounded sequence. Ensure the range is bounded where an algorithm requires termination.

### 123. How can a view become invalid even if the view object still exists?

**Answer:** Its source container may be destroyed, reallocated, or otherwise mutated in a way that invalidates iterators. The view's own lifetime does not control the backing storage's lifetime or stability.

### 124. What does `std::ranges::to` do?

**Answer:** C++23's `ranges::to` materializes a range into a container when the required construction and insertion operations are available. It is a conversion, not a guarantee of zero allocations or a particular container complexity.

### 125. What are `views::zip` and `views::enumerate` useful for?

**Answer:** `zip` traverses multiple ranges together, while `enumerate` pairs elements with indices. They are C++23 library facilities whose implementation support can vary; check the relevant feature-test macro and provide a loop fallback when needed.

## Concurrency and synchronization

### 126. Why must a `std::thread` be joined or detached?

**Answer:** Destroying a joinable `std::thread` calls `std::terminate`. Joining waits for completion; detaching relinquishes direct ownership of completion and requires every object the thread accesses to outlive it.

### 127. What synchronization does `join()` provide?

**Answer:** Completion of the joined thread synchronizes with the successful return from `join()`. The joining thread can then safely observe writes sequenced before the worker finished, assuming no other unsynchronized concurrent accesses occurred.

### 128. Why is detaching a thread risky?

**Answer:** A detached thread can outlive the scope that started it and any referenced local state. Its completion, errors, and shutdown are harder to coordinate; prefer an owned thread or task unless detachment is specifically required.

### 129. What does `std::lock_guard` do?

**Answer:** It locks a mutex on construction and unlocks it on destruction, including exception paths. It is appropriate for simple lexical critical sections where the lock need not be manually released early.

### 130. When would you use `std::unique_lock` instead?

**Answer:** Use `unique_lock` when the lock must be deferred, temporarily unlocked, transferred, or used with a condition variable. It has more state and overhead than `lock_guard`, so use it when those capabilities are needed.

### 131. What is the correct condition-variable wait pattern?

**Answer:** Hold a `unique_lock` and wait with a predicate, such as `condition.wait(lock, [&]{ return ready; })`. The predicate handles spurious wakeups and rechecks state under the same mutex used by the producer.

### 132. Why can a condition variable wake spuriously?

**Answer:** The standard permits waits to return without a corresponding notification. Code must always recheck the condition in a loop or use the predicate overload rather than treating wakeup itself as proof that the condition holds.

### 133. How can consistent lock ordering prevent deadlock?

**Answer:** If every path acquires multiple locks in the same global order, circular wait among those locks is prevented. `std::scoped_lock` can acquire multiple mutexes using a deadlock-avoidance algorithm when locking them together is appropriate.

### 134. What does `std::scoped_lock` provide?

**Answer:** It acquires one or more mutexes and releases them at scope exit. For multiple mutexes it uses deadlock-avoidance locking, but it does not solve higher-level deadlocks involving locks acquired at different times.

### 135. What is the difference between an atomic operation and a mutex-protected operation?

**Answer:** An atomic operation provides indivisible access to its atomic object and can establish memory ordering. A mutex protects a larger critical section and multiple related invariants; atomics do not automatically make a multi-step algorithm atomic.

### 136. What does `memory_order_relaxed` guarantee?

**Answer:** It guarantees atomicity and a modification order for that atomic object, but does not create synchronization or order unrelated memory accesses. Use it only when no inter-thread publication or ordering is required.

### 137. What does `memory_order_seq_cst` add?

**Answer:** It provides sequential consistency for sequentially consistent atomic operations, giving them a single total order consistent with each thread's program order. It is easier to reason about than weaker orders, but may impose more constraints than necessary.

### 138. What is compare-exchange used for?

**Answer:** It atomically compares an object with an expected value and conditionally replaces it. On failure it updates the expected value; weak compare-exchange may fail spuriously and is normally used in a retry loop.

### 139. What is the ABA problem?

**Answer:** A compare-exchange can observe a value change from A to B and back to A, then incorrectly conclude nothing changed. Solutions depend on the data structure and may include version tags, hazard pointers, or other reclamation schemes.

### 140. What is false sharing?

**Answer:** Independent variables written by different threads can occupy the same cache line, causing coherence traffic despite no data race. Padding or layout changes may help after measurement; it is a performance issue, not a correctness bug.

### 141. What is the difference between lock-free and wait-free progress?

**Answer:** Lock-free guarantees system-wide progress: some operation completes despite contention. Wait-free guarantees each operation completes within a bounded number of its own steps. Neither means an operation is automatically faster than a mutex.

### 142. Does the C++ standard guarantee fairness for mutexes?

**Answer:** No general fairness or starvation-freedom guarantee is provided for `std::mutex`. If fairness is essential, design an explicit policy or use a platform facility with documented guarantees.

### 143. When should you use `std::latch`?

**Answer:** Use a latch for one-shot coordination: participants decrement a count and waiters proceed when it reaches zero. It cannot be reset for another phase.

### 144. How is `std::barrier` different from `std::latch`?

**Answer:** A barrier coordinates a fixed group through reusable phases; each phase completes after the expected arrivals. A latch is one-shot and is simpler when only one completion event is needed.

### 145. What does a semaphore model?

**Answer:** A semaphore tracks a count of permits that threads can acquire and release. It can limit concurrent access to a resource pool, but unlike a mutex it does not inherently identify an owning thread.

### 146. What are `promise` and `future` for?

**Answer:** A promise publishes one result or exception to an associated future. The future waits for and retrieves that shared state; the pair is useful for one-shot result transfer, not repeated message queues.

### 147. What does `shared_future` change?

**Answer:** It allows multiple consumers to wait for and retrieve the same shared result. A normal `future` is move-only and has a single consumer of its result interface.

### 148. Why specify `std::launch::async` with `std::async`?

**Answer:** Without an explicit policy, `std::async` may choose deferred execution, in which the callable runs on a waiting thread. `launch::async` requests asynchronous execution; still retain and observe the future to manage completion and exceptions.

### 149. Are function-local static initializations thread-safe?

**Answer:** Since C++11, concurrent first entry into a block-scope static declaration waits for one initialization to finish. Recursive re-entry during its own initialization is not a safe way to initialize it.

### 150. What does `thread_local` mean?

**Answer:** Each thread has a distinct instance with thread storage duration. It avoids synchronization for that object's independent instances, but does not make objects shared through pointers or other globals thread-safe.

### 151. What is `std::atomic_ref`?

**Answer:** It provides atomic operations on an existing suitably aligned object while the `atomic_ref` exists. All concurrent accesses must obey its atomicity rules; mixing ordinary conflicting accesses with atomic ones is not made safe automatically.

### 152. What does a stop token do?

**Answer:** A stop token communicates a cooperative cancellation request to a task. The task must check or register for the request and decide how to stop; the token does not interrupt arbitrary blocking operations by itself.

## Coroutines and modules

### 153. What is a coroutine promise type?

**Answer:** The promise type defines how a coroutine's return object is created and how it handles suspension, yielded or returned values, and unhandled exceptions. The compiler uses promise conventions to build the coroutine frame and control flow.

### 154. What is the purpose of `initial_suspend` and `final_suspend`?

**Answer:** `initial_suspend` controls whether the coroutine begins running immediately or suspends at first. `final_suspend` controls its state at completion and can keep the frame available for its owner to inspect and destroy.

### 155. What is the difference between `co_yield` and `co_return`?

**Answer:** `co_yield` produces a value and suspends according to the promise's `yield_value`; `co_return` completes the coroutine and may provide its final result. Their meaning depends on the return object's promise protocol.

### 156. Who owns a coroutine frame?

**Answer:** Ownership must be defined by the coroutine's return object or another explicit handle owner. Destroy the frame exactly once after it is no longer executing; leaking or double-destroying a handle is an error.

### 157. What happens to an uncaught exception inside a coroutine?

**Answer:** The compiler routes it through `promise.unhandled_exception()`. The promise may store an `exception_ptr`, translate the error, or terminate; a generator should define how consumers learn about failures.

### 158. What does an awaiter provide?

**Answer:** An awaiter participates in `co_await` through operations such as `await_ready`, `await_suspend`, and `await_resume`. It defines readiness, suspension behavior, and the resumed result or error; it does not by itself guarantee a scheduler exists.

### 159. How can a coroutine frame outlive the call that created it?

**Answer:** A coroutine can suspend and retain parameters and locals in its frame after the initial call returns. The return object or handle owner controls when the frame is resumed and destroyed, so captured references still need valid external lifetimes.

### 160. What makes a generator synchronous or asynchronous?

**Answer:** A pull generator typically resumes when its consumer requests the next value and is synchronous. Asynchronous generation requires suspension integrated with an asynchronous event source or scheduler; `co_yield` alone does not provide that.

### 161. What is a module interface unit?

**Answer:** It is a module translation unit that declares a named module and exports declarations intended for importers. It is compiled to module metadata and object code according to the toolchain's module build process.

### 162. What is the difference between `export` and `import`?

**Answer:** `export` makes a declaration part of a module's public interface. `import` makes an exported module interface available to the importing translation unit; it is not a textual include of the entire implementation.

### 163. What is a module partition?

**Answer:** A partition is a translation unit belonging to a named module, used to divide its implementation or interface. Partitions are imported within the module using module-qualified partition names and still participate in a coordinated build.

### 164. What is a header unit?

**Answer:** A header unit is a header compiled into an importable unit under implementation and build-system rules. It is distinct from a named module interface and may have different macro and compatibility behavior.

### 165. Why do modules need build-system support?

**Answer:** Importing translation units depend on compiled module interfaces, so the build must discover and order those dependencies and track compiler-generated module artifacts. A compiler language flag alone does not create a correct dependency graph.

### 166. Does importing a module automatically expose all of its dependencies?

**Answer:** No. A module exposes declarations it exports, and transitive visibility follows module interface rules rather than textual inclusion. Do not rely on a module importer to acquire unrelated declarations accidentally.

### 167. What is the global module fragment used for?

**Answer:** It is the portion before a module declaration where preprocessing directives, especially includes needed for declarations in the module unit, can appear. It supports interoperability with headers but does not make those header declarations exported automatically.

### 168. Are modules a replacement for every header?

**Answer:** No. Modules offer different dependency and interface properties, but headers remain useful for compatibility, templates and build ecosystems, and toolchains with incomplete module support. Migration should be driven by a concrete benefit.

### 169. What is an IFC in the MSVC module workflow?

**Answer:** An IFC is an implementation-specific compiled module interface artifact consumed by importing translation units. Its format and command-line handling are toolchain details; do not treat it as a portable C++ source format.

### 170. Can a module implementation hide all ABI concerns?

**Answer:** No. Modules change source-level visibility and compilation, but exported types and functions still have ABI, layout, and compatibility considerations when crossing binary boundaries.

## Portability, correctness, and tooling

### 171. What is the difference between compiling and linking?

**Answer:** Compilation translates a translation unit and checks its local declarations and definitions. Linking combines object files and libraries to resolve external references and produce a program or library; a program can compile successfully and still fail to link.

### 172. What does `inline` mean for a function?

**Answer:** Primarily, it permits equivalent definitions in multiple translation units under the ODR when the function is defined in each. It does not require the optimizer to substitute the function body at each call.

### 173. What is the PImpl idiom?

**Answer:** A class holds a pointer to an implementation type defined in its source file, hiding private layout and reducing header dependencies. It can stabilize ABI and compile times but adds indirection and ownership/copy design choices.

### 174. What is ABI compatibility?

**Answer:** ABI compatibility means separately compiled components agree on binary-level details such as calling conventions, symbol names, object layout, and exception behavior. Source compatibility does not guarantee ABI compatibility.

### 175. How do undefined, unspecified, and implementation-defined behavior differ?

**Answer:** Undefined behavior has no requirements imposed by the standard. Unspecified behavior permits one of several outcomes without requiring documentation of which. Implementation-defined behavior requires the implementation to document its choice.

### 176. What does “sequenced before” mean?

**Answer:** It is a relation ordering evaluations within a thread. When evaluations are unsequenced and conflict on the same memory location, behavior may be undefined; do not rely on a presumed left-to-right order unless the language specifies one.

### 177. What happens on signed integer overflow?

**Answer:** Signed arithmetic overflow is undefined behavior in standard C++. Check bounds or use a type and operation with explicitly managed range; do not rely on two's-complement wraparound for signed values.

### 178. Does unsigned integer arithmetic overflow?

**Answer:** Unsigned arithmetic is reduced modulo one more than the maximum representable value. This defined wraparound can still be a logic or security bug, especially in size and bounds calculations.

### 179. What is a narrowing conversion?

**Answer:** It is a conversion that can lose information, such as converting a wider integer to a smaller one or a floating value to an integer. List-initialization rejects many narrowing conversions, but not every lossy conversion in every context.

### 180. What is strict aliasing about?

**Answer:** It limits which glvalue types may be used to access an object's stored value. Violating the permitted aliasing rules can make optimizations invalidate assumptions; use `std::memcpy` or `std::bit_cast` for object-representation transfers where appropriate.

### 181. What does `std::bit_cast` guarantee?

**Answer:** It creates a value of one trivially copyable type from the object representation of another trivially copyable type of equal size, subject to its constraints. It does not promise a particular endianness or meaningful interpretation of arbitrary bits.

### 182. How can code inspect native endianness?

**Answer:** C++20 provides `std::endian` in `<bit>` to describe the implementation's scalar byte order when known. Portable file formats should specify byte order explicitly rather than serialize native object representations blindly.

### 183. Why is floating-point equality often unsuitable for computed results?

**Answer:** Many decimal values are not exactly representable in binary floating point, and operation order can change rounding. Compare using a domain-appropriate tolerance or exact scaled representation when the domain requires decimal precision.

### 184. What special comparison behavior does NaN have?

**Answer:** NaN compares unequal to itself and ordinary ordered comparisons with it are false. Algorithms requiring a strict weak ordering need comparators that define coherent behavior for NaNs.

### 185. How can you avoid cross-translation-unit static initialization problems?

**Answer:** Prefer explicit dependency injection or construct dependent state within a function-local static whose initialization is synchronized. Avoid hidden dependencies between global constructors in unrelated translation units.

### 186. What is a feature-test macro?

**Answer:** It is a library- or language-provided macro indicating support for a facility or revision. Check the required threshold, not merely whether a vaguely related header or language-mode option exists.

### 187. Does selecting `-std=c++latest` or `/std:c++latest` prove C++26 library support?

**Answer:** No. A language mode controls which syntax and wording the compiler enables; standard-library implementation is separate and may lag. Verify the specific feature-test macro and vendor documentation.

### 188. What does a sanitizer do?

**Answer:** A sanitizer instruments a build to detect classes of runtime defects, such as many memory errors or data races, during exercised executions. It cannot prove the absence of bugs and may not support every compiler, platform, or feature.

### 189. When are compiler warnings useful?

**Answer:** Warnings can reveal suspicious conversions, unused values, lifetime mistakes, and portability concerns before runtime. Enable appropriate warning levels and address meaningful warnings, but do not treat warning-free code as proof of correctness.

### 190. What is the difference between `assert` and a test check?

**Answer:** `assert` may be compiled out when `NDEBUG` is defined. A test framework check normally remains active in Release builds; use assertions for programmer invariants and always-on checks for test expectations.

### 191. What does `static_assert` verify?

**Answer:** It checks a constant expression during compilation and can stop the build with a diagnostic if a type or value property is false. It cannot test runtime data or replace runtime validation.

### 192. What does `[[nodiscard]]` communicate?

**Answer:** It asks the implementation to diagnose a discarded result because ignoring it may be suspicious. It is not a runtime guarantee, and callers can still explicitly discard a result.

### 193. What does `[[deprecated]]` do?

**Answer:** It requests a diagnostic when a deprecated entity is used, optionally with a message. It helps migration but does not remove the entity or guarantee every toolchain emits the same warning.

### 194. Why are macros more dangerous than `constexpr` values or functions?

**Answer:** Macros perform token substitution without normal scope, type checking, or evaluation rules and can evaluate arguments multiple times. Prefer constants, functions, templates, or language facilities unless preprocessing is specifically needed.

### 195. What is a translation unit?

**Answer:** It is the source after preprocessing, including included text, that the compiler processes as a unit. Ordinary header-based builds compile multiple translation units and rely on declarations and ODR rules to connect them.

### 196. What is source compatibility versus binary compatibility?

**Answer:** Source compatibility means existing source continues to compile against a changed interface. Binary compatibility means already-compiled clients continue to work without recompilation; layout or calling-convention changes can break the latter while preserving the former.

### 197. Why should support claims name both compiler and standard library?

**Answer:** Language syntax is implemented by the compiler front end, while library facilities are provided by the standard-library implementation; their support levels can differ. Reproducible claims record both toolchain versions and relevant feature-test macros.

### 198. How should you discuss a proposed or evolving C++ feature in an interview?

**Answer:** Separate current standardized wording from proposals and vendor extensions, and state the compiler/library version being discussed. Do not present syntax from an old proposal as portable standard C++.

### 199. What makes a useful C++ interview answer?

**Answer:** State the rule, explain its consequence, identify a failure mode or tradeoff, and give a small example. Clarify which standard version or implementation assumption matters rather than bluffing when a detail is uncertain.

### 200. Is this list a complete checklist of modern C++?

**Answer:** No. C++ is too broad and continues to evolve; this is a 200-question practice set covering selected high-value concepts, not every language rule, library facility, proposal, ABI detail, or interviewer's preferred topic.