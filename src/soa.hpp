#pragma once

#include <memory>
#include <tuple>
#include <vector>

/**
 * https://en.wikipedia.org/wiki/AoS_and_SoA
 */
template <typename... Ts>
class SoA {
 public:
  struct Ref {
    size_t type, id;
  };

  template <typename T, size_t I = 0, typename... Args>
  auto Add(Args &&... args) {
    using Type = typename std::tuple_element_t<I, DataType>::value_type;
    if constexpr (std::is_same_v<Type, T>) {
      std::get<I>(data_).emplace_back(std::forward<Args>(args)...);
      return Ref{I, std::get<I>(data_).size() - 1};
    } else {
      static_assert(I + 1 < std::tuple_size_v<DataType>, "Unknown type");
      return Add<T, I + 1>(std::forward<Args>(args)...);
    }
  }

  template <typename T, size_t I = 0, typename... Args>
  auto AddIfNew(Args &&... args) {
    using Type = typename std::tuple_element_t<I, DataType>::value_type;
    if constexpr (std::is_same_v<Type, T>) {
      T x{std::forward<Args>(args)...};
      auto &vec = std::get<I>(data_);
      if (vec.empty() || vec.back() != x) {
        vec.push_back(std::move(x));
      }
      return Ref{I, std::get<I>(data_).size() - 1};
    } else {
      static_assert(I + 1 < std::tuple_size_v<DataType>, "Unknown type");
      return AddIfNew<T, I + 1>(std::forward<Args>(args)...);
    }
  }

  template <size_t I = 0, typename Base>
  auto Add(std::unique_ptr<const Base> shape) {
    using Type = typename std::tuple_element_t<I, DataType>::value_type;
    if (const auto x = dynamic_cast<const Type *>(shape.get())) {
      std::get<I>(data_).push_back(std::move(*x));
      return Ref{I, std::get<I>(data_).size() - 1};
    } else if constexpr (I + 1 < std::tuple_size_v<DataType>) {
      return Add<I + 1>(std::move(shape));
    } else {
      throw std::runtime_error("Unknown type");
    }
  }

  template <size_t I = 0, typename F>
  auto Call(const Ref &ref, F f) const {
    if (ref.type == I) {
      return f(std::get<I>(data_)[ref.id]);
    } else if constexpr (I + 1 < std::tuple_size_v<DataType>) {
      return Call<I + 1>(ref, f);
    } else {
      throw std::runtime_error("Unknown type");
    }
  }

 private:
  using DataType = std::tuple<std::vector<Ts>...>;
  DataType data_;
};
