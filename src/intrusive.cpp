#include <chrono>
#include <iostream>

// ── Intrusive hooks ───────────────────────────────────────────────────────────
// Each hook is an independent doubly-linked node embedded in the Order.
// The two hooks let an Order live in two separate sorted lists simultaneously.

struct PriceNode {
    PriceNode* price_prev{nullptr};
    PriceNode* price_next{nullptr};
};

struct TimeNode {
    TimeNode* time_prev{nullptr};
    TimeNode* time_next{nullptr};
};

// ── Generic intrusive sorted list ─────────────────────────────────────────────
// Hook  — the embedded node type (PriceNode or TimeNode)
// Owner — the full object type that inherits from Hook (Order)
// Less  — strict-weak comparator on Owner*

template<typename Hook, typename Owner, typename Less>
struct SortedIntrusiveList {
    Hook sentinel;   // sentinel: sentinel.next == first, sentinel.prev == last
    Less less;

    // Pointers to the sentinel's "next" and "prev" fields, via Hook interface
    Hook*& front_hook() { return sentinel.*(&Hook::price_next); } // resolved below
    Hook*& back_hook()  { return sentinel.*(&Hook::price_prev); }

    SortedIntrusiveList() { next_of(&sentinel) = prev_of(&sentinel) = &sentinel; }

    // ── pointer accessors (avoids member-pointer verbosity) ───────────────────
    static Hook*& next_of(Hook* h);
    static Hook*& prev_of(Hook* h);

    // Insert keeping ascending order
    void insert(Owner* owner) {
        Hook* node = static_cast<Hook*>(owner);
        // Find first existing node that is greater than owner
        Hook* pos = next_of(&sentinel);
        while (pos != &sentinel && !less(owner, static_cast<Owner*>(pos)))
            pos = next_of(pos);
        // Link before pos
        prev_of(node)        = prev_of(pos);
        next_of(node)        = pos;
        next_of(prev_of(pos)) = node;
        prev_of(pos)         = node;
    }

    void remove(Owner* owner) {
        Hook* node = static_cast<Hook*>(owner);
        next_of(prev_of(node)) = next_of(node);
        prev_of(next_of(node)) = prev_of(node);
        next_of(node) = prev_of(node) = nullptr;
    }

    template<typename Fn>
    void for_each(Fn fn) const {
        for (Hook* h = next_of(const_cast<Hook*>(&sentinel)); h != &sentinel; h = next_of(h))
            fn(static_cast<Owner*>(h));
    }

    static Hook*& next_of_impl(Hook* h, PriceNode*) { return h->price_next; }
    static Hook*& prev_of_impl(Hook* h, PriceNode*) { return h->price_prev; }
    static Hook*& next_of_impl(Hook* h, TimeNode*)  { return h->time_next; }
    static Hook*& prev_of_impl(Hook* h, TimeNode*)  { return h->time_prev; }
};

template<typename Hook, typename Owner, typename Less>
Hook*& SortedIntrusiveList<Hook,Owner,Less>::next_of(Hook* h) {
    return next_of_impl(h, static_cast<Hook*>(nullptr));
}
template<typename Hook, typename Owner, typename Less>
Hook*& SortedIntrusiveList<Hook,Owner,Less>::prev_of(Hook* h) {
    return prev_of_impl(h, static_cast<Hook*>(nullptr));
}

// ── Domain type ───────────────────────────────────────────────────────────────

using Timestamp = std::chrono::steady_clock::time_point;

struct Order : PriceNode, TimeNode {
    double    price;
    int       quantity;
    Timestamp timestamp;

    Order(double p, int q, Timestamp ts)
        : price{p}, quantity{q}, timestamp{ts} {}
};

struct ByPrice {
    bool operator()(const Order* a, const Order* b) const { return a->price < b->price; }
};
struct ByTime {
    bool operator()(const Order* a, const Order* b) const { return a->timestamp < b->timestamp; }
};

using PriceList = SortedIntrusiveList<PriceNode, Order, ByPrice>;
using TimeList  = SortedIntrusiveList<TimeNode,  Order, ByTime>;

// ── Main ──────────────────────────────────────────────────────────────────────

int main() {
    using clk = std::chrono::steady_clock;
    using ms  = std::chrono::milliseconds;

    Order o1{100.5, 10, clk::now()};
    Order o2{ 99.0,  5, clk::now() + ms{50}};
    Order o3{101.0, 20, clk::now() + ms{25}};
    Order o4{ 98.5, 15, clk::now() + ms{75}};

    PriceList by_price;
    TimeList  by_time;

    for (Order* o : {&o1, &o2, &o3, &o4}) {
        by_price.insert(o);
        by_time.insert(o);
    }

    std::cout << "--- Sorted by price ---\n";
    by_price.for_each([](const Order* o) {
        std::cout << "  price=" << o->price << "  qty=" << o->quantity << '\n';
    });

    Timestamp base = o1.timestamp;
    std::cout << "\n--- Sorted by timestamp ---\n";
    by_time.for_each([&](const Order* o) {
        auto ms_off = std::chrono::duration_cast<ms>(o->timestamp - base).count();
        std::cout << "  t+" << ms_off << "ms  price=" << o->price
                  << "  qty=" << o->quantity << '\n';
    });

    // Remove o2 from both lists
    by_price.remove(&o2);
    by_time.remove(&o2);

    std::cout << "\nAfter removing o2 (price=99.0):\n--- Sorted by price ---\n";
    by_price.for_each([](const Order* o) {
        std::cout << "  price=" << o->price << "  qty=" << o->quantity << '\n';
    });
}
