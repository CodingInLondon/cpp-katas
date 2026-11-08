# cpp-katas
C++ is dead, long live C++

This is a sandbox where I experiment with modern C++ language features and low-latency patterns. Most of the work is currently happening in the ``feed_handler`` directory.



[cpp-top-features.md](cpp-top-features.md): Overview of new features since C++11 (the top 5 in each revision)

[cpp-features-in-bitcoin-core.md](cpp-features-in-bitcoin-core.md): How many of these new features is actually used in Bitcoin Core (which is on C++20)

[low-latency-patterns.md](low-latency-patterns.md): Study of low-latency patterns in the Bitcoin Core code base.

## Directory structure

```
.
├── src/
│   ├── *.cpp            scratch pads, standalone katas
│   ├── feed-handler/    training lab to practice low-latency patterns in a running system.
│   │   └── claude       Test harnesses and bare-metal benchmarks
│   │
│   ├── modern/          one C++ example per language feature, grouped by standard (Claude-generated)
│   │   ├── cpp11/ ... cpp23/
│   │   └── build.sh     builds the examples
│   └── canons/          reference implementations (arena, pool, message parser)
│
└── docs/                rendered version of the md files (HTML, PDF)
```





