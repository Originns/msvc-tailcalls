#pragma once
#include <cstdint>
#include <cstring>
#include <type_traits>

template <typename Ret, std::size_t N>
class Arguments {
public:
    template <typename T>
    __forceinline T* get(std::size_t index) {
        if (index >= N) return nullptr;
        if (sizeof(Ret) > sizeof(std::uint64_t))
        {
            // return value of anything larger than 8 bytes 
            // will instead be caller allocated and
            // passed as pointer in rcx
            index++;
        }

        if (index <= 3 && std::is_floating_point<T>::value)
        {
            return reinterpret_cast<T*>(&xmmdata[index]);
        }

        if (sizeof(T) > sizeof(std::uint64_t))
        {
            return *reinterpret_cast<T**>(&data_[index]);
        }

        return reinterpret_cast<T*>(&data_[index]);
    }

private:
    std::uint64_t xmmdata[4];
    std::uint64_t reserved; // callees return address
    std::uint64_t data_[N];
};