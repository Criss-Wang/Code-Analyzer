#include "results.h"

template<typename T>
int Results<T>::AddResult(T item) {
  try {
    result_lst_.push_back(item);
    return 1;
  } catch (exception &e) {
    return 0;
  }
}

template<typename T>
vector<T> Results<T>::GetResult() {
  return result_lst_;
}
