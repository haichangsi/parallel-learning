#include <time.h>
#include <benchmark/benchmark.h>

#include "ith_search.hpp"


template <typename T>
void vector_random_init(std::vector<T>& A, std::size_t n) {
	srand(time(NULL));
	for (std::size_t i=0; i<n; i++) {
		A.push_back(rand()%100);
	}
}

static void BM_rand_select(benchmark::State& state) {
	std::vector<int> A;
	vector_random_init(A, state.range(0));
	for (auto _ : state) {
		benchmark::DoNotOptimize(rand_select(A, 0, int(A.size())-1, 5));
	}
	state.SetComplexityN(state.range(0));
}


BENCHMARK(BM_rand_select)
	->RangeMultiplier(2)
	->Range(1<<10, 1<<18)
	->Complexity(benchmark::oN);

BENCHMARK_MAIN();

