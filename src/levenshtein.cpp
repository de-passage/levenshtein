#include "levenshtein.hpp"
#include <cstring>
#include <cstdlib>
#include <algorithm>

namespace {
  inline int min(int l, int m, int r) {
    return std::min(l, std::min(m, r));
  }
}

int levenshtein(const char* source, const char* target) noexcept {
  const int source_len = strlen(source);
  const int target_len = strlen(target);

  return levenshtein(source, source_len, target, target_len);
}

int levenshtein(const char* source, int source_len, const char* target, int target_len) noexcept {
  int* previous_row_distance = static_cast<int*>(malloc((target_len + 1) * sizeof(int)));
  int* current_row_distance = static_cast<int*>(malloc((target_len + 1) * sizeof(int)));

  for (int i = 0; i < target_len; ++i) previous_row_distance[i] = 0;

  for (int i = 0; i < source_len; ++i) {
    current_row_distance[0] = i + 1;

    for (int j = 0; j < target_len; ++j) {
      int deletion_cost = previous_row_distance[j + 1] + 1;
      int insertion_cost = current_row_distance[j] + 1;
      int substitution_cost = previous_row_distance[j];

      if (source[i] != target[j]) {
        ++substitution_cost;
      }

      current_row_distance[j + 1] = min(deletion_cost, insertion_cost, substitution_cost);
    }
    int* tmp = current_row_distance;
    current_row_distance = previous_row_distance;
    previous_row_distance = tmp;
  }

  int result = previous_row_distance[target_len];
  free(previous_row_distance);
  free(current_row_distance);
  return result;
}
