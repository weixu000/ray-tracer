#pragma once

#include <memory>

template <typename Base, typename... EntryArgs>
struct RegistryFactory {
  template <EntryArgs... args, template <auto...> typename T>
  struct Entry {
    using Type = T<args...>;
    static bool Match(EntryArgs... params) { return ((args == params) && ...); }
  };

  template <typename Entry, typename... Entries>
  struct Registry {
    template <typename... Args>
    static std::unique_ptr<Base> Get(EntryArgs... entry_args, Args&&... args) {
      if (Entry::Match(entry_args...)) {
        return std::make_unique<typename Entry::Type>(
            std::forward<Args>(args)...);
      } else if constexpr (sizeof...(Entries)) {
        return Registry<Entries...>::Get(entry_args...,
                                         std::forward<Args>(args)...);
      } else {
        throw;
      }
    }
  };
};