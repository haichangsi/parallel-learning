#include <iostream>
#include <vector>
#include <random>

#pragma once


auto& prng_engine() {
	thread_local static std::mt19937 engine{std::random_device{}()};
	return engine;
}

template <typename T>
T partition(std::vector<T> &A, T p, T q) {
	T x = A[q];
	T i = p-1;
	for (T j=p; j<q; j++) {
		if (A[j] <= x) {
			i++;
			std::swap(A[i], A[j]);
		}
	}
	std::swap(A[i+1], A[q]);
	return i+1;
}

template <typename T>
T rand_partition(std::vector<T> &A, T p, T q) {
	// random pivot
	std::uniform_int_distribution<T> dist(p, q);
	// check with
	T r = dist(prng_engine());
	std::swap(A[r], A[q]);
	return partition(A, p, q);
}

template <typename T>
T rand_select_helper(std::vector<T> &A, T p, T q, T i) {
	T r, k;

	if (p==q) {
	  return A[p];
	}

	r = rand_partition(A, p, q);
	k = r-p+1;

	if (i==k) {
		return A[r];
	}
	if (i<k) {
		return rand_select_helper(A, p, r-1, i);
	} else {
		return rand_select_helper(A, r+1, q, i-k);
	}
}

template <typename T>
T rand_select(std::vector<T> A, T p, T q, T i) {
	std::vector<T> B = A;
	// std::copy(A.begin(), A.end(), B.begin());
	return rand_select_helper(B, p, q, i);
}
