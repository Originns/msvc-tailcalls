#pragma once
#include <cstdint>
#include <cstring>
#include <type_traits>

template <std::size_t N>
class Arguments {
public:
    template <typename T>
    T* get(std::size_t index) {
        static_assert(sizeof(T) <= sizeof(std::uint64_t), "Type must not be larger than 8 bytes");
        if (index >= N) return nullptr;
        return reinterpret_cast<T*>(&data_[index]);
    }

    template <typename T>
    void set(std::size_t index, T value) {
        static_assert(sizeof(T) <= sizeof(std::uint64_t), "Type must not be larger than 8 bytes");
        if (index >= N) return;
        *reinterpret_cast<T*>(&data_[index]) = value;
    }

private:
    std::uint64_t data_[N];
};