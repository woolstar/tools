#include <benchmark/benchmark.h>
#include <random>

// Version 1: full reverse
int is_palindrome_v1(int n) {
    if (n < 0) return 0;

    int original = n;
    int reversed = 0;

    while (n > 0) {
        reversed = reversed * 10 + (n % 10);
        n /= 10;
    }

    return original == reversed;
}

// Version 2: half reverse
int is_palindrome_v2(int n) {
    if (n < 0) return 0;
    if (n < 10) return 1;
    if (n % 10 == 0) return 0;

    int reversed_half = 0;
    while (n > reversed_half) {
        reversed_half = reversed_half * 10 + (n % 10);
        n /= 10;
    }

    return n == reversed_half || n == reversed_half / 10;
}

// Version 3: fastest
int is_palindrome_v3(int n) {
    if (n < 10) return 1;
    if (n % 10 == 0) return 0;

      // save extra digit version of n
    int reversed_half = 0;
    int pren = n ;
    while (n > reversed_half) {
        pren = n ;
        reversed_half = reversed_half * 10 + (n % 10);
        n /= 10;
    }

    return n == reversed_half || pren == reversed_half ;
}

int is_palindrome_v4(int n) {
    if (n < 10) return 1;
    if (n % 10 == 0) return 0;

      // early exit if match
    int reversed_half = 0;
    while (n > reversed_half) {
        reversed_half = reversed_half * 10 + (n % 10);
        if ( reversed_half == n) return true ;
        n /= 10;
        if ( reversed_half == n) return true ;
    }

    return false ;
}

static const std::vector<int> make_inputs(size_t count) {
    std::mt19937 rng(42);
    std::uniform_int_distribution<int> dist(0, std::numeric_limits<int>::max());
    std::vector<int> v(count);
    for (auto& x : v) x = dist(rng);
    return v;
}

static const std::vector<int> inputs = make_inputs(1 << 20);

static void BM_FullReverse(benchmark::State& state) {
    size_t i = 0;
    volatile int sink = 0;
    for (auto _ : state) {
        sink ^= is_palindrome_v1(inputs[i++ & ((1 << 20) - 1)]);
    }
    benchmark::DoNotOptimize(sink);
}

static void BM_HalfReverse(benchmark::State& state) {
    size_t i = 0;
    volatile int sink = 0;
    for (auto _ : state) {
        sink ^= is_palindrome_v2(inputs[i++ & ((1 << 20) - 1)]);
    }
    benchmark::DoNotOptimize(sink);
}

static void BM_SaveReverse(benchmark::State& state) {
    size_t i = 0;
    volatile int sink = 0;
    for (auto _ : state) {
        sink ^= is_palindrome_v3(inputs[i++ & ((1 << 20) - 1)]);
    }
    benchmark::DoNotOptimize(sink);
}

static void BM_BothReverse(benchmark::State& state) {
    size_t i = 0;
    volatile int sink = 0;
    for (auto _ : state) {
        sink ^= is_palindrome_v4(inputs[i++ & ((1 << 20) - 1)]);
    }
    benchmark::DoNotOptimize(sink);
}

BENCHMARK(BM_FullReverse);
BENCHMARK(BM_HalfReverse);
BENCHMARK(BM_SaveReverse);
BENCHMARK(BM_BothReverse);

BENCHMARK_MAIN();
