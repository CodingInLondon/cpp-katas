# Modern C++ Features as Used in Bitcoin Core

A survey of the features listed in [`cpp-top-features.md`](./cpp-top-features.md) mapped to **real code** in Bitcoin Core **v30.2**, which targets **C++20** (`CMakeLists.txt:78 set(CMAKE_CXX_STANDARD 20)`).

Every reference below is a real line under `src/`. Features from `cpp-top-features.md` are marked ✅; extras found in the code but **not** in that file are marked ➕. Features from the file that Core deliberately does **not** use are marked ❌.

---

## C++11

| Feature | Real example | One-liner use case |
|---|---|---|
| ✅ `auto` deduction | `coins.cpp:49 const auto [ret, inserted] = ...` | Avoid spelling out long iterator/pair types. |
| ✅ Move semantics | `net.cpp:496 sock = std::move(conn.sock);` | Transfer socket ownership without copying. |
| ✅ Lambdas | `net_processing.cpp:5325 auto addr_already_known = [&peer](...)` | Inline predicate capturing local state for a callback. |
| ✅ `unique_ptr` / `shared_ptr` | `net.h:679 const std::unique_ptr<Transport> m_transport;` / `net.h:691 std::shared_ptr<Sock> m_sock` | RAII ownership of transport; shared ownership of a live socket. |
| ✅ Range-based for | `validation.cpp:127 for (const uint256& hash : locator.vHave)` | Clean iteration over a container. |
| ➕ `enum class` | `rpc/util.h:40 enum class PSBTError;` | Scoped, type-safe error enums (no implicit int). |
| ➕ `nullptr` | `coins.cpp:20 return nullptr;` | Type-safe null instead of `NULL`/`0`. |
| ➕ `constexpr` constants | `consensus/amount.h:15 static constexpr CAmount COIN = 100000000;` | Compile-time consensus constants. |
| ➕ Variadic templates | `tinyformat.h:988 template<typename... Args>` | Type-safe formatting over any argument list. |
| ➕ `std::atomic` | `net.h:705 std::atomic<std::chrono::seconds> m_last_send{0s};` | Lock-free concurrent timestamp updates. |

## C++14

| Feature | Real example | One-liner use case |
|---|---|---|
| ✅ Generic lambdas | `rpc/blockchain.cpp:2454 [&](const auto& txout){...}` | One predicate works across element types passed to `std::any_of`. |
| ✅ `make_unique` | `init.cpp:1259 node.mempool = std::make_unique<CTxMemPool>(...)` | Exception-safe construction, no raw `new`. |
| ✅ Return-type deduction | `util/subprocess.h:1292 auto _dup2_ = [](int fd, int to_fd){...}` | Let compiler infer return type of helper. |
| ✅ Relaxed `constexpr` | `util/bitset.h:38 unsigned inline constexpr PopCount(I v)` | Real logic (loops/branches) evaluated at compile time. |
| ➕ Digit separators | `logging.h:107 DEFAULT_MAX_LOG_BUFFER{1'000'000}` | Readable large literals / bit masks (`protocol.h:387`). |
| ➕ `make_shared` | `net_processing.cpp:1565 PeerRef peer = std::make_shared<Peer>(...)` | Single-allocation shared object creation. |

*(`decltype(auto)` from the file isn't idiomatic in Core; return-type deduction covers the same ground.)*

## C++17

| Feature | Real example | One-liner use case |
|---|---|---|
| ✅ Structured bindings | `coins.cpp:49 const auto [ret, inserted] = cacheCoins.try_emplace(...)` | Unpack a map insert result by name. |
| ✅ `if constexpr` | `serialize.h:803 if constexpr (BasicByte<T>)` | Pick an optimized serialize path at compile time. |
| ✅ `std::optional` | `node/kernel_notifications.h:62 std::optional<uint256> TipBlock()` | "Maybe a tip hash" instead of sentinel. |
| ✅ `std::string_view` | `rpc/util.h:102 ParseHashV(..., std::string_view name)` | Pass RPC param names with zero copying. |
| ✅ `std::variant` | `addresstype.h:143 using CTxDestination = std::variant<...>` | Type-safe union of all address kinds. |
| ➕ `if`-with-initializer | `coins.cpp:68 if (auto it{FetchCoin(outpoint)}; it != ...)` | Scope a lookup result to the `if` only. |
| ➕ `[[nodiscard]]` | `base58.h:31 [[nodiscard]] bool DecodeBase58(...)` | Force callers to check a decode's success. |
| ➕ `[[maybe_unused]]` | `deploymentstatus.h:14 ..., [[maybe_unused]] VersionBitsCache& ...)` | Silence unused-param warnings in conditional builds. |
| ➕ Fold expressions | `serialize.h:987 (::Serialize(s, args), ...);` | Serialize a whole parameter pack in one line. |
| ➕ `inline` variables | `util/types.h:10 inline constexpr bool ALWAYS_FALSE{false};` | Header-defined global with no ODR duplication. |
| ➕ `std::byte` | `span.h:98 unsigned char* UCharCast(std::byte* c)` | Represent raw bytes without integer semantics. |
| ➕ `std::filesystem` | `util/fs.h:24 using namespace std::filesystem;` | Portable path handling (wrapped for safety). |
| ➕ `std::clamp` | `txmempool.cpp:406 std::clamp<int>(opts.check_ratio, 0, 1'000'000)` | Bound a config value to a valid range. |
| ➕ CTAD | `outputtype.h:25 static constexpr auto OUTPUT_TYPES = std::array{...}` | Deduce `std::array` type/size from initializer. |

## C++20

| Feature | Real example | One-liner use case |
|---|---|---|
| ✅ Concepts | `random.h:147 concept RandomNumberGenerator = requires(...)` ; `serialize.h:740 concept Serializable` | Constrain templates to types with the required interface, with clear errors. |
| ✅ Ranges (views) | `net_processing.cpp:2061 for (... : vHashes \| std::views::reverse)` | Iterate in reverse without a manual reverse loop. |
| ❌ Coroutines | *not used* | Core's networking is an explicit event/socket loop, not `co_await`. |
| ❌ Modules | *not used* | Still header-based (toolchain portability). |
| ❌ `std::format` | *not used* | Uses in-tree `tinyformat` (`logging.h:342 tfm::format`) instead. |
| ➕ `std::span` | `serialize.h:56 s.write(std::as_bytes(std::span{&obj, 1}))` | Non-owning view over contiguous bytes for I/O. |
| ➕ Three-way `<=>` | `arith_uint256.h:216 friend ... operator<=>(...)` | Generate all comparison operators from one. |
| ➕ `std::ranges` algorithms | `external_signer.cpp:83 std::ranges::equal(...)` ; `i2p.cpp:313 std::ranges::find(...)` | Pass containers directly, no begin/end pairs. |
| ➕ Designated initializers | `net.cpp:730 return {.transport_type = ..., .session_id = {}};` | Self-documenting aggregate/option construction. |
| ➕ `consteval` | `util/translation.h:59 consteval TranslatedLiteral(const char* str, ...)` | Guarantee translation literals are built at compile time. |
| ➕ `using enum` | `versionbits.cpp:12 using enum ThresholdState;` | Drop enum-class prefixes inside a scope. |
| ➕ Bit ops `<bit>` | `random.h:259 std::bit_width(maxval)` ; `util/bitset.h:88 std::countr_zero(...)` | Portable, branch-free bit counting. |
| ➕ `std::endian` | `compat/endian.h:15 if constexpr (std::endian::native == std::endian::little)` | Compile-time byte-order selection. |
| ➕ `std::to_array` | `protocol.h:270 std::to_array<std::string>({...})` | Build a fixed array with deduced size. |
| ➕ `[[likely]]`/`[[unlikely]]` | `random.cpp:431 ... [[unlikely]]` | Branch-prediction hint on a rare path. |
| ➕ `std::counting_semaphore` | `semaphore_grant.h:16 std::counting_semaphore<LeastMaxValue>* sem;` | Standard-library connection-slot limiting (replaced Core's custom semaphore). |
| ➕ `std::source_location` | `logging.h:23 #include <source_location>` | Automatic file/line/function in log records. |

---

## Big picture

Core adopts the *low-level, zero-cost, safety* side of each standard aggressively — spans, concepts, `constexpr`/`consteval`, bit ops, atomics, RAII pointers — but deliberately avoids the heavyweight C++20 additions. **Coroutines, modules, and `std::format` are absent**, since Core keeps a hand-rolled event loop, header-based builds, and its own `tinyformat`.
